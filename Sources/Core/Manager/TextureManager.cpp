#include "TextureManager.h"
#include "BufferManager.h"
#include <Engine\TaskScheduler.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Engine\Device.h>

namespace Engine
{
    TextureManager g_TextureManager;

    void TextureManager::Init()
    {
        mTexture.reserve(TEXTURE_INIT_CAPACITY);
        //mImageAllocation.reserve(TEXTURE_INIT_CAPACITY);
        mUploadFence = GDevice.CreateFence();

        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eTransient,
            GRAPHICS_FAMILY_INDEX);
        mCommandPool = g_vkDevice.createCommandPool(poolInfo);

        LOG_INFO("[LOG] TextureManager Init\n");
    }

    void TextureManager::Destroy()
    {
        g_vkDevice.destroyFence(mUploadFence);

        for (size_t i = 0; i < mTexture.size(); ++i)
        {
            //g_vkDevice.destroySampler(mTexture[i].mSampler);
            //g_vkDevice.destroyImageView(mTexture[i].mImageView);
            //vmaDestroyImage(GVmaAllocator, mTexture[i].mImage, mImageAllocation[i]);
			mTexture[i].Destroy();
        }

        g_vkDevice.destroyCommandPool(mCommandPool);

        LOG_INFO("[LOG] TextureManager Destroy\n");
    }

    void TextureManager::ExecuteOperations()
    {
        if (!mUploadRequest.empty())
        {
            std::vector<vk::ImageMemoryBarrier> preTransferTransition(
                mUploadRequest.size(),
                vk::ImageMemoryBarrier(
                    {}, vk::AccessFlagBits::eTransferWrite,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal,
                    VK_QUEUE_FAMILY_IGNORED,
                    VK_QUEUE_FAMILY_IGNORED,
                    {}, // Image will be set in the loop
                    vk::ImageSubresourceRange(
                        vk::ImageAspectFlagBits::eColor,
                        0, 1, 0, 1
                    ))
            );

            std::vector<vk::ImageMemoryBarrier> postTransferTransition(
                mUploadRequest.size(),
                vk::ImageMemoryBarrier(
                    vk::AccessFlagBits::eTransferWrite,
                    vk::AccessFlagBits::eShaderRead,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    VK_QUEUE_FAMILY_IGNORED,
                    VK_QUEUE_FAMILY_IGNORED,
                    {}, // Image will be set in the loop
                    vk::ImageSubresourceRange(
                        vk::ImageAspectFlagBits::eColor,
                        0, 1, 0, 1
                    ))
            );

            std::vector<vk::BufferImageCopy> bufferImageCopy(
                mUploadRequest.size(),
                vk::BufferImageCopy(
                    0, 0, 0,
                    vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
                    vk::Offset3D(0, 0, 0),
                    {} // To be specified in the loop
                )
            );

            for (size_t i = 0; i < mUploadRequest.size(); ++i)
            {
                size_t j = mUploadRequest[i].imageIndex;
                preTransferTransition[i].image = mTexture[j].mImage;
                
                bufferImageCopy[i].imageExtent = vk::Extent3D(
                    mTexture[j].mWidth,
                    mTexture[j].mHeight,
                    mTexture[j].mDepth
                );

                postTransferTransition[i].image = mTexture[j].mImage;
            }

            vk::CommandBufferAllocateInfo cmdBuffAllocateInfo(mCommandPool, vk::CommandBufferLevel::ePrimary, 1);
            mTransferCmdBuffer = g_vkDevice.allocateCommandBuffers(cmdBuffAllocateInfo)[0];
            vk::CommandBufferBeginInfo cmdBuffBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

            mTransferCmdBuffer.begin(cmdBuffBeginInfo);

            mTransferCmdBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eTransfer, 
                (vk::DependencyFlagBits)0, {}, {}, preTransferTransition
            );

            for (size_t i = 0; i < mUploadRequest.size(); ++i)
            {
                size_t j = mUploadRequest[i].imageIndex;
                
                mTransferCmdBuffer.copyBufferToImage(
                    mUploadRequest[i].stagBuffer,
                    mTexture[j].mImage,
                    vk::ImageLayout::eTransferDstOptimal,
                    { bufferImageCopy[i] }
                );
            }

            mTransferCmdBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader,
                (vk::DependencyFlagBits)0, {}, {}, postTransferTransition
            );

            mTransferCmdBuffer.end();

            vk::SubmitInfo subInfo(0, nullptr, nullptr, 1, &mTransferCmdBuffer, 0, nullptr);
            TEXTURE_TRANSFER_QUEUE.submit(1, &subInfo, mUploadFence);

            LOG_INFO("[LOG] TextureManager start upload\n");

            GDevice.WaitForFence(mUploadFence);

            LOG_INFO("[LOG] TextureManager end upload\n");

            GDevice.ResetFence(mUploadFence);
            for (auto& req : mUploadRequest)
            {
                vmaDestroyBuffer(GVmaAllocator, req.stagBuffer, req.stagAllocation);
            }
            mUploadRequest.clear();
            g_vkDevice.freeCommandBuffers(mCommandPool, { mTransferCmdBuffer });
        }
        
    }

    vk::Image TextureManager::CreateImage2D(vk::Extent3D extent, vk::ImageUsageFlags imageUsageFlags,
        VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
        VmaAllocation & vmaAllocation, VmaAllocationInfo * vmaAllocationInfo,
        vk::Format format)
    {
        VkImage rawImage;
        vk::Image image;

        vk::ImageCreateInfo imageCreateInfo({}, vk::ImageType::e2D,
            format, extent, 1, 1,
            vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, imageUsageFlags,
            vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined);

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.flags = vmaAllocationFlags;
        allocCreateInfo.usage = vmaMemoryUsage;

        assert(
            vmaCreateImage(
                GVmaAllocator,
                &(VkImageCreateInfo)imageCreateInfo,
                &allocCreateInfo,
                &rawImage,
                &vmaAllocation,
                vmaAllocationInfo) == VK_SUCCESS
        );

        image = rawImage;
        return image;
    }

	vk::Image TextureManager::CreateCubeMap(vk::Extent3D extent, uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
		VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
		VmaAllocation & vmaAllocation, VmaAllocationInfo * vmaAllocationInfo,
		vk::Format format)
	{
		VkImage rawImage;
		vk::Image image;

		vk::ImageCreateInfo imageCreateInfo(vk::ImageCreateFlagBits::eCubeCompatible,
			vk::ImageType::e2D,
			format, extent, mipLevels, 6,
			vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, imageUsageFlags,
			vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined);

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = vmaAllocationFlags;
		allocCreateInfo.usage = vmaMemoryUsage;

		assert(
			vmaCreateImage(
				GVmaAllocator,
				&(VkImageCreateInfo)imageCreateInfo,
				&allocCreateInfo,
				&rawImage,
				&vmaAllocation,
				vmaAllocationInfo) == VK_SUCCESS
		);

		image = rawImage;
		return image;
	}

	vk::ImageView TextureManager::CreateCubeMapView(vk::Image image, uint32_t mipLevels, vk::Format format, vk::ImageAspectFlags aspectFlags)
	{
		vk::ImageViewCreateInfo imageViewCI({},
			image,
			vk::ImageViewType::eCube,
			format,
			{},
			vk::ImageSubresourceRange(aspectFlags, 0, mipLevels, 0, 6)
		);

		return g_vkDevice.createImageView(imageViewCI);
	}

    uint32_t TextureManager::LoadTex2D(const char * path)
    {
        Texture tex;
        tex.mDepth = 1;
        stbi_uc* pixels = stbi_load(path, &tex.mWidth, &tex.mHeight, &tex.mChannels, STBI_rgb_alpha);
        vk::DeviceSize imageSize = tex.mWidth * tex.mHeight * STBI_rgb_alpha;
        assert(pixels);
        
        VmaAllocation stagAllocation;
        VmaAllocationInfo stagAllocInfo;
        vk::Buffer stagBuffer;

        stagBuffer = g_BufferManager.CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
            VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT, stagAllocation, &stagAllocInfo);

        memcpy(stagAllocInfo.pMappedData, pixels, static_cast<size_t>(imageSize));
        stbi_image_free(pixels);

        VmaAllocation imageAllocation;

        vk::Extent3D extent( (uint32_t)tex.mWidth, (uint32_t)tex.mHeight, (uint32_t)tex.mDepth );
        tex.mImage = CreateImage2D(extent,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            VMA_MEMORY_USAGE_GPU_ONLY, 0, imageAllocation, nullptr);

        tex.mImageView = CreateImageView2D(tex.mImage, vk::Format::eR8G8B8A8Unorm);
        tex.mSampler = CreateSampler();
		tex.mImageAllocation = imageAllocation;

        uint32_t index = mTexture.size();
        mTexture.push_back(tex);
        //mImageAllocation.push_back(imageAllocation);

        UploadRequest req;
        req.imageIndex = index;
        req.stagBuffer = stagBuffer;
        req.stagAllocation = stagAllocation;
        mUploadRequest.push_back(req);

        return index;
    }

    vk::ImageView TextureManager::CreateImageView2D(vk::Image image, vk::Format format,
        vk::ImageAspectFlags aspectFlags)
    {
        vk::ImageViewCreateInfo imageViewCI({},
            image,
            vk::ImageViewType::e2D,
            format,
            {},
            vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1)
        );

        return g_vkDevice.createImageView(imageViewCI);
    }

	uint32_t TextureManager::LoadTexHDR(const char* path)
	{
		Texture tex;
		tex.mDepth = 1;
		float* pixels = stbi_loadf(path, &tex.mWidth, &tex.mHeight, &tex.mChannels, STBI_rgb_alpha);
		vk::DeviceSize imageSize = tex.mWidth * tex.mHeight * STBI_rgb_alpha * sizeof(float);
		assert(pixels);

		VmaAllocation stagAllocation;
		VmaAllocationInfo stagAllocInfo;
		vk::Buffer stagBuffer;

		stagBuffer = g_BufferManager.CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
			VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT, stagAllocation, &stagAllocInfo);

		memcpy(stagAllocInfo.pMappedData, pixels, static_cast<size_t>(imageSize));
		stbi_image_free(pixels);

		VmaAllocation imageAllocation;

		vk::Extent3D extent((uint32_t)tex.mWidth, (uint32_t)tex.mHeight, (uint32_t)tex.mDepth);
		tex.mImage = CreateImage2D(extent,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, imageAllocation, nullptr, vk::Format::eR32G32B32A32Sfloat);

		tex.mImageView = CreateImageView2D(tex.mImage, vk::Format::eR32G32B32A32Sfloat);
		tex.mSampler = CreateHDRSampler();
		tex.mImageAllocation = imageAllocation;

		uint32_t index = mTexture.size();
		mTexture.push_back(tex);
		//mImageAllocation.push_back(imageAllocation);

		UploadRequest req;
		req.imageIndex = index;
		req.stagBuffer = stagBuffer;
		req.stagAllocation = stagAllocation;
		mUploadRequest.push_back(req);

		return index;
	}

    void TextureManager::TransitionImageLayout(vk::Image image, vk::Format format,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageSubresourceRange imageRange)
    {
        vk::AccessFlags srcAccessFlags = {};
        vk::AccessFlags dstAccessFlags = {};
        vk::PipelineStageFlags srcStage = {};
        vk::PipelineStageFlags dstStage = {};

        if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            imageRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

            if (format == vk::Format::eD32SfloatS8Uint ||
                format == vk::Format::eD24UnormS8Uint)
            {
                imageRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
            }
        }
        else
        {
            imageRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        }

        if (oldLayout == vk::ImageLayout::eUndefined
            && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            dstAccessFlags = vk::AccessFlagBits::eTransferWrite;

            srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            dstStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal
            && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            srcAccessFlags = vk::AccessFlagBits::eTransferRead;
            dstAccessFlags = vk::AccessFlagBits::eShaderRead;

            srcStage = vk::PipelineStageFlagBits::eTransfer;
            dstStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined
            && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            dstAccessFlags = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition!");
        }
        
        vk::ImageMemoryBarrier imageBarrier(srcAccessFlags, dstAccessFlags,
            oldLayout, newLayout, 0, 0, image, imageRange);

        vk::CommandBufferAllocateInfo bufferAI(mCommandPool, vk::CommandBufferLevel::ePrimary, 1);
        vk::CommandBuffer buffer = g_vkDevice.allocateCommandBuffers(bufferAI)[0];
        vk::CommandBufferBeginInfo bufferBI(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        vk::Fence fence = GDevice.CreateFence();

        buffer.begin(bufferBI);
        buffer.pipelineBarrier(srcStage, dstStage, {}, {}, {}, { imageBarrier });
        buffer.end();

        vk::SubmitInfo subInfo(0, nullptr, nullptr, 1, &buffer, 0, nullptr);
		LAYOUT_TRANSITION_QUEUE.submit(1, &subInfo, fence);

        LOG_INFO("[LOG] Image layout transition start \n");

        GDevice.WaitForFence(fence);

        LOG_INFO("[LOG] Image layout transition end \n");
        g_vkDevice.destroyFence(fence);
        g_vkDevice.freeCommandBuffers(mCommandPool, { buffer });
    }

	void TextureManager::TransitionImageLayout(vk::CommandBuffer cmdBuf, vk::Image image,
		vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageSubresourceRange imageRange)
	{
		vk::AccessFlags srcAccessFlags = {};
		vk::AccessFlags dstAccessFlags = {};
		vk::PipelineStageFlags srcStage = {};
		vk::PipelineStageFlags dstStage = {};

		if (oldLayout == vk::ImageLayout::eUndefined
			&& newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			dstAccessFlags = vk::AccessFlagBits::eTransferWrite;

			srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal
			&& newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			srcAccessFlags = vk::AccessFlagBits::eTransferRead;
			dstAccessFlags = vk::AccessFlagBits::eShaderRead;

			srcStage = vk::PipelineStageFlagBits::eTransfer;
			dstStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else if (oldLayout == vk::ImageLayout::eUndefined
			&& newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
		{
			dstAccessFlags = vk::AccessFlagBits::eDepthStencilAttachmentRead |
				vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transition!");
		}

		vk::ImageMemoryBarrier imageBarrier(srcAccessFlags, dstAccessFlags,
			oldLayout, newLayout, 0, 0, image, imageRange);

		cmdBuf.pipelineBarrier(srcStage, dstStage, {}, {}, {}, { imageBarrier });
	}

	Texture TextureManager::CreateTexture2D(uint32_t width, uint32_t height, uint32_t depth,
		vk::ImageUsageFlags imageUsageFlags, VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
		vk::Format format, vk::ImageAspectFlags aspectFlags)
	{
		Texture tex;
		tex.mWidth = width;
		tex.mHeight = height;
		tex.mDepth = depth;
		tex.mImage = CreateImage2D(vk::Extent3D(width, height, depth), imageUsageFlags, vmaMemoryUsage, vmaAllocationFlags,
			tex.mImageAllocation, nullptr, format);
		tex.mImageView = CreateImageView2D(tex.mImage, format, aspectFlags);
		return tex;
	}

	Texture TextureManager::CreateCubeMapTexture(uint32_t width, uint32_t height, uint32_t depth,
		uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
		VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
		vk::Format format, vk::ImageAspectFlags aspectFlags)
	{
		Texture tex;
		tex.mWidth = width;
		tex.mHeight = height;
		tex.mDepth = depth;
		tex.mImage = CreateCubeMap(vk::Extent3D(width, height, depth), mipLevels, imageUsageFlags, vmaMemoryUsage, vmaAllocationFlags,
			tex.mImageAllocation, nullptr, format);
		tex.mImageView = CreateCubeMapView(tex.mImage, mipLevels, format, aspectFlags);
		return tex;
	}

    uint32_t TextureManager::CreateTextureFromColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        Texture tex;
        tex.mDepth = 1;
        tex.mWidth = 1;
        tex.mHeight = 1;
        stbi_uc pixels[] = { r, g, b, a };
        vk::DeviceSize imageSize = tex.mWidth * tex.mHeight * 4;
        assert(pixels);

        VmaAllocation stagAllocation;
        VmaAllocationInfo stagAllocInfo;
        vk::Buffer stagBuffer;

        stagBuffer = g_BufferManager.CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
            VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT, stagAllocation, &stagAllocInfo);

        memcpy(stagAllocInfo.pMappedData, pixels, static_cast<size_t>(imageSize));

        VmaAllocation imageAllocation;

        vk::Extent3D extent((uint32_t)tex.mWidth, (uint32_t)tex.mHeight, (uint32_t)tex.mDepth);
        tex.mImage = CreateImage2D(extent,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            VMA_MEMORY_USAGE_GPU_ONLY, 0, imageAllocation, nullptr);

        tex.mImageView = CreateImageView2D(tex.mImage, vk::Format::eR8G8B8A8Unorm);
        tex.mSampler = CreateSampler();
		tex.mImageAllocation = imageAllocation;

        uint32_t index = mTexture.size();
        mTexture.push_back(tex);
        //mImageAllocation.push_back(imageAllocation);

        UploadRequest req;
        req.imageIndex = index;
        req.stagBuffer = stagBuffer;
        req.stagAllocation = stagAllocation;
        mUploadRequest.push_back(req);

        return index;
    }

    vk::Sampler TextureManager::CreateSampler()
    {
        vk::SamplerCreateInfo samplerCI({},
            vk::Filter::eLinear,
            vk::Filter::eNearest,
            {},
            vk::SamplerAddressMode::eRepeat,
            vk::SamplerAddressMode::eRepeat,
            vk::SamplerAddressMode::eRepeat,
            0.0f, VK_TRUE, 16, VK_FALSE,
            vk::CompareOp::eAlways, 0.0f, 0.0f
        );

        return g_vkDevice.createSampler(samplerCI);
    }
	
	vk::Sampler TextureManager::CreateSamplerCube()
	{
		vk::SamplerCreateInfo samplerCI({},
			vk::Filter::eLinear,
			vk::Filter::eLinear,
			{},
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			0.0f, VK_TRUE, 16, VK_FALSE,
			vk::CompareOp::eAlways, 0.0f, 0.0f,
			vk::BorderColor::eFloatOpaqueWhite
		);

		return g_vkDevice.createSampler(samplerCI);
	}

	vk::Sampler TextureManager::CreateSamplerPrenv(uint32_t numMips)
	{
		vk::SamplerCreateInfo samplerCI({},
			vk::Filter::eLinear,
			vk::Filter::eLinear,
			vk::SamplerMipmapMode::eLinear,
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			0.0f, VK_FALSE, 0.0f, VK_FALSE,
			vk::CompareOp::eAlways, 0.0f, numMips,
			vk::BorderColor::eFloatOpaqueWhite
		);

		return g_vkDevice.createSampler(samplerCI);
	}

	vk::Sampler TextureManager::CreateHDRSampler()
	{
		vk::SamplerCreateInfo samplerCI({},
			vk::Filter::eLinear,
			vk::Filter::eLinear,
			{},
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			0.0f, VK_TRUE, 16, VK_FALSE,
			vk::CompareOp::eAlways, 0.0f, 0.0f
		);

		return g_vkDevice.createSampler(samplerCI);
	}
	
	void Texture::Destroy()
	{
		g_vkDevice.destroySampler(mSampler);
		g_vkDevice.destroyImageView(mImageView);
		vmaDestroyImage(GVmaAllocator, mImage, mImageAllocation);
	}
}

/* EXPORTED INTERFACE */
extern "C"
{
	LAVA_API uint32_t LoadHDR(const char* path)
	{
		return Engine::g_TextureManager.LoadTexHDR(path);
	}

    LAVA_API uint32_t Load2D(const char* path)
    {
        return Engine::g_TextureManager.LoadTex2D(path);
    }

    LAVA_API uint32_t CreateFromColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return Engine::g_TextureManager.CreateTextureFromColor(r, g, b, a);
    }
}