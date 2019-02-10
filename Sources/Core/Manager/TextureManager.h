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
        int mWidth, mHeight;
        int mDepth;
        int mChannels;
        vk::Image mImage;
        vk::Sampler mSampler;
        vk::ImageView mImageView;
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
        
        vk::ImageView CreateImageView2D(vk::Image image,
            vk::Format format = vk::Format::eR8G8B8A8Unorm,
            vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

        void TransitionImageLayout(vk::Image image, vk::Format format,
            vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);

        uint32_t LoadTex2D(const char* path);
        
        /// <summary>
        /// Creates a 1x1 texture given the color rgba
        /// </summary>
        uint32_t CreateTextureFromColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        
        vk::Sampler CreateSampler();

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
        std::vector<VmaAllocation> mImageAllocation;
    };

    extern TextureManager g_TextureManager;
}