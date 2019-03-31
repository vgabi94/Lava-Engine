#pragma once
#include <Engine\Texture.h>
#include <string>
#include <unordered_map>

#define GTextureManager Engine::g_TextureManager
#define TextureAt(i) g_TextureManager.GetTexture(i);

namespace Engine
{
    class TextureManager
    {
        static constexpr uint32_t TEXTURE_INIT_CAPACITY = 16;

    public:
        void Init();
        void Destroy();

        void ExecuteOperations();

        vk::Image CreateImage2D(vk::Extent3D extent, uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
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
        
        vk::ImageView CreateImageView2D(vk::Image image, uint32_t mipLevels,
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
		uint32_t CreateTexture2D(uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
			VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
			vk::Format format = vk::Format::eR8G8B8A8Unorm,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

		uint32_t CreateCubeMapTexture(uint32_t width, uint32_t height, uint32_t depth,
			uint32_t mipLevels, vk::ImageUsageFlags imageUsageFlags,
			VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
			vk::Format format = vk::Format::eR8G8B8A8Unorm,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

		// ---- Loading functions ---- //
        uint32_t LoadTex2D(const char* path, bool genmips = false);
		uint32_t LoadTexHDR(const char* path, bool genmips = false);
		// --------------------------- //
        
        /// <summary>
        /// Creates a 1x1 texture given the color rgba
        /// </summary>
        uint32_t CreateTextureFromColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		uint32_t GetColorTexture(const std::string& name);
        
		// ---- Samplers ---- //
        vk::Sampler CreateSampler();
		vk::Sampler CreateSamplerCube();
		vk::Sampler CreateSamplerPrenv(uint32_t numMips);
		vk::Sampler CreateHDRSampler();
		vk::Sampler CreateSamplerBrdf() { return CreateSamplerPrenv(1); }
		// ------------------ //

		void GenerateMipmaps(vk::CommandBuffer cmdbuf, vk::Image image,
			uint32_t width, uint32_t height, uint32_t miplevels, uint32_t layers = 1/*,
			vk::ImageLayout oldLayout = vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout newLayout = vk::ImageLayout::eShaderReadOnlyOptimal*/);

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
			bool genmips;
        };

        typedef std::vector<Texture> TextureList;
        typedef std::vector<UploadRequest> UploadRequestList;

		std::unordered_map<std::string, uint32_t> mColorTextures;

        TextureList mTexture;
        UploadRequestList mUploadRequest;

        vk::CommandPool mCommandPool;
        vk::CommandBuffer mTransferCmdBuffer;
        vk::Fence mUploadFence;
        //std::vector<VmaAllocation> mImageAllocation;
    };

    extern TextureManager g_TextureManager;
}