#pragma once

#include <vulkan\vulkan.hpp>
#include <vector>
#include <vk_mem_alloc.h>

#define GTextureManager Engine::g_TextureManager
#define TextureAt(i) g_TextureManager.GetTexture(i);

namespace Engine
{
    struct Texture
    {
		friend class TextureManager;

        int mWidth, mHeight;
        int mDepth;
        int mChannels;
        vk::Image mImage;
        vk::Sampler mSampler;
        vk::ImageView mImageView;
	
		void Destroy();
		
		void* GetMappedBit() { if (mVmaAllocationInfo) return mVmaAllocationInfo->pMappedData; else return nullptr; }
	
	private:
		VmaAllocation mImageAllocation;
		VmaAllocationInfo* mVmaAllocationInfo;
    };

    class TextureManager
    {
        static constexpr uint32_t TEXTURE_INIT_CAPACITY = 16;

    public:
        void Init();
        void Destroy();

        void ExecuteOperations();

        vk::Image CreateImage2D(vk::Extent3D extent, vk::ImageUsageFlags imageUsageFlags,
            VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
            VmaAllocation& vmaAllocation, VmaAllocationInfo* vmaAllocationInfo,
            vk::Format format = vk::Format::eR8G8B8A8Unorm);

		vk::Image CreateCubeMap(vk::Extent3D extent, uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
			VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
			VmaAllocation& vmaAllocation, VmaAllocationInfo* vmaAllocationInfo,
			vk::Format format = vk::Format::eR8G8B8A8Unorm);

		vk::ImageView CreateCubeMapView(vk::Image image, uint32_t mipLevels,
			vk::Format format = vk::Format::eR8G8B8A8Unorm,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);
        
        vk::ImageView CreateImageView2D(vk::Image image,
            vk::Format format = vk::Format::eR8G8B8A8Unorm,
            vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

        void TransitionImageLayout(vk::Image image, vk::Format format,
            vk::ImageLayout initialLayout, vk::ImageLayout finalLayout,
			vk::ImageSubresourceRange imageRange =
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

		void TransitionImageLayout(vk::CommandBuffer cmdBuf, vk::Image image,
			vk::ImageLayout initialLayout, vk::ImageLayout finalLayout,
			vk::ImageSubresourceRange imageRange =
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

		// ---- Create texture functions ---- //
		Texture CreateTexture2D(uint32_t width, uint32_t height, uint32_t depth, vk::ImageUsageFlags imageUsageFlags,
			VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
			vk::Format format = vk::Format::eR8G8B8A8Unorm,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

		Texture CreateCubeMapTexture(uint32_t width, uint32_t height, uint32_t depth,
			uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
			VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
			vk::Format format = vk::Format::eR8G8B8A8Unorm,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

		// ---- Loading functions ---- //
        uint32_t LoadTex2D(const char* path);
		uint32_t LoadTexHDR(const char* path);
		// --------------------------- //
        
        /// <summary>
        /// Creates a 1x1 texture given the color rgba
        /// </summary>
        uint32_t CreateTextureFromColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        
		// ---- Samplers ---- //
        vk::Sampler CreateSampler();
		vk::Sampler CreateSamplerCube();
		vk::Sampler CreateSamplerPrenv(uint32_t numMips);
		vk::Sampler CreateHDRSampler();
		// ------------------ //

        const Texture& GetTexture(uint32_t index) const
        {
            assert(index < mTexture.size());
            return mTexture[index];
        }

    private:
        struct UploadRequest
        {
            uint32_t imageIndex;
            vk::Buffer stagBuffer;
            VmaAllocation stagAllocation;
        };

        typedef std::vector<Texture> TextureList;
        typedef std::vector<UploadRequest> UploadRequestList;

        TextureList mTexture;
        UploadRequestList mUploadRequest;

        vk::CommandPool mCommandPool;
        vk::CommandBuffer mTransferCmdBuffer;
        vk::Fence mUploadFence;
        //std::vector<VmaAllocation> mImageAllocation;
    };

    extern TextureManager g_TextureManager;
}