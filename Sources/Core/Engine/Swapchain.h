#pragma once

#include <vulkan\vulkan.hpp>
#include <RenderPass\FramePass.h>

#define GSwapchain Vulkan::g_Swapchain

namespace Vulkan
{
    class Swapchain
    {
    public:
        void Init();
        void Destroy();

        vk::Format GetImageFormat() const { return mImageFormat; }
        
        uint32_t GetCurrentFrameIndex() const { return mCurrentImageIndex; }
        uint32_t GetPreviousFrameIndex() const { return mPreviousImageIndex; }
        
        uint32_t GetImageCount() const { return static_cast<uint32_t>(mImage.size()); }
        const vk::ImageView& GetImageViewAt(size_t index) const { return mImageView[index]; }
        
        vk::Extent2D GetExtent() const { return mExtent; }
        Engine::FramePass* GetFramePass() const { return mFramePass; }

        // Render one frame and display the previous
        void Update();

    private:
        void _Init();

        vk::SwapchainKHR mSwapchain;
        std::vector<vk::Image> mImage;
        vk::Extent2D mExtent;
        vk::Format mImageFormat;
        std::vector<vk::ImageView> mImageView;
        Engine::FramePass* mFramePass;

        vk::Semaphore mImageAvailableSem;
        vk::Semaphore mRenderFinishedSem;
        uint32_t mPreviousImageIndex;
        uint32_t mCurrentImageIndex;

        void CreateSwapchain();
        void CreateImageViews();
        void RecreateSwapchain();

        void DestroyImageViews();
    };

    extern Swapchain g_Swapchain;
}