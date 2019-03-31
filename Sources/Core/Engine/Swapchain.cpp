#include "Swapchain.h"
#include "Engine.h"
#include "Device.h"
#include <Manager\TextureManager.h>
#include <Manager\RenderpassManager.h>
#include <Manager\WorldManager.h>

namespace Vulkan
{
    Swapchain g_Swapchain;

    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        {
            return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
        }

        for (const auto& f : availableFormats)
        {
            if (f.format == vk::Format::eB8G8R8A8Unorm &&
                f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return f;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
    {
        for (const auto& p : availablePresentModes)
        {
            if (p == vk::PresentModeKHR::eMailbox
                || p == vk::PresentModeKHR::eImmediate)
            {
                return p;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilites)
    {
        if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilites.currentExtent;
        }
        else
        {
            vk::Extent2D actualExent{ WINDOW_WIDTH, WINDOW_HEIGHT };
            actualExent.width = std::max(capabilites.minImageExtent.width,
                std::min(capabilites.maxImageExtent.width, actualExent.width));
            actualExent.height = std::max(capabilites.minImageExtent.height,
                std::min(capabilites.maxImageExtent.height, actualExent.height));

            return actualExent;
        }
    }

    // --------------------- SWAPCHAIN ------------------------ //
    void Swapchain::Init()
    {
        _Init();
		GRenderpassManager.InitPasses();
		mFramePass = GRenderpassManager.GetPass<Engine::FramePass>(RP::FRAME);
        LOG_INFO("[LOG] Swapchain init\n");
    }

    void Swapchain::Destroy()
    {
        DestroyImageViews();
        g_vkDevice.destroySwapchainKHR(mSwapchain);
        g_vkDevice.destroySemaphore(mImageAvailableSem);
        g_vkDevice.destroySemaphore(mRenderFinishedSem);
        LOG_INFO("[LOG] Swapchain destroy\n");
    }

    void Swapchain::Update()
    {
        THROW_IF(g_CurrentWorld == nullptr, "Current world cannot be null!");
//#ifdef _DEBUG
//        // Must wait for queue when validation layers are enabled
//        // otherwise the memory will grow very rapidly
//        PRESENTATION_QUEUE.waitIdle();
//#endif
        auto res = g_vkDevice.acquireNextImageKHR(
            mSwapchain,
            std::numeric_limits<uint64_t>::max(),
            mImageAvailableSem, nullptr);

        mCurrentImageIndex = res.value;
        if (res.result == vk::Result::eErrorOutOfDateKHR)
        {
            RecreateSwapchain();
            return;
        }

		GDevice.WaitForFence(GRenderpassManager.GetFenceAt(mCurrentImageIndex));
		GDevice.ResetFence(GRenderpassManager.GetFenceAt(mCurrentImageIndex));
		
		GRenderpassManager.SetupPasses();
		GRenderpassManager.RenderPasses(mImageAvailableSem, vk::PipelineStageFlagBits::eColorAttachmentOutput, mRenderFinishedSem);

        mPreviousImageIndex = mCurrentImageIndex;

        vk::PresentInfoKHR presentInfo(1, &mRenderFinishedSem, 1, &mSwapchain, &mCurrentImageIndex);
        try
        {
            PRESENTATION_QUEUE.presentKHR(presentInfo);
        }
        catch (const vk::SystemError&)
        {
            RecreateSwapchain();
        }
    }

    void Swapchain::_Init()
    {
        mImageAvailableSem = g_Device.CreateSemaphore();
        mRenderFinishedSem = g_Device.CreateSemaphore();
        CreateSwapchain();
        CreateImageViews();
        mPreviousImageIndex = -1;
        mCurrentImageIndex = 0;
    }
    
    void Swapchain::CreateSwapchain()
    {
        SwapChainSupportDetails scsd = g_Device.QuerySwapChainSupport(g_vkPhysicalDevice, g_vkSurface);
        vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(scsd.formats);
        vk::PresentModeKHR presentMode = ChooseSwapPresentMode(scsd.presentModes);
        vk::Extent2D extent = ChooseSwapExtent(scsd.capabilities);

        uint32_t imageCount = scsd.capabilities.minImageCount;
        if (scsd.capabilities.maxImageCount > 0 && imageCount > scsd.capabilities.maxImageCount)
        {
            imageCount = scsd.capabilities.maxImageCount;
        }

        vk::SharingMode sharingMode;
        uint32_t queueFamilyCount;
        uint32_t* queuePtr;

        uint32_t queueFamilyIndices[] = {
            GRAPHICS_FAMILY_INDEX,
            PRESENTATION_FAMILY_INDEX
        };

        if (GRAPHICS_FAMILY_INDEX != PRESENTATION_FAMILY_INDEX)
        {
            sharingMode = vk::SharingMode::eConcurrent;
            queueFamilyCount = 2;
            queuePtr = queueFamilyIndices;
        }
        else
        {
            sharingMode = vk::SharingMode::eExclusive;
            queueFamilyCount = 0;
            queuePtr = nullptr;
        }

        vk::SwapchainCreateInfoKHR createInfo({},
            g_vkSurface, imageCount, surfaceFormat.format,
            surfaceFormat.colorSpace, extent, 1,
            vk::ImageUsageFlagBits::eColorAttachment, sharingMode,
            queueFamilyCount, queuePtr, scsd.capabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, VK_TRUE,
            nullptr);

        mSwapchain = g_vkDevice.createSwapchainKHR(createInfo);

        mExtent = extent;
        mImageFormat = surfaceFormat.format;

		mImage = g_vkDevice.getSwapchainImagesKHR(mSwapchain);
    }

    void Swapchain::CreateImageViews()
    {
        mImageView.resize(mImage.size());
        for (unsigned int i = 0; i < mImageView.size(); i++)
        {
            mImageView[i] = GTextureManager.CreateImageView2D(mImage[i], 1, mImageFormat);
        }
    }

    void Swapchain::RecreateSwapchain()
    {
        assert(g_CurrentWorld != nullptr);
        g_vkDevice.waitIdle();
        Destroy();
        g_CurrentWorld->FreeWorldCommandBuffers();
        _Init();
        mFramePass->Recreate();
        g_CurrentWorld->CreateWorldCommandBuffers();
        g_CurrentWorld->mDirty = WORLD_DIRTY;
        LOG_INFO("[LOG] Swapchain recreate\n");
    }
    
    void Swapchain::DestroyImageViews()
    {
        for (unsigned int i = 0; i < mImageView.size(); i++)
        {
            g_vkDevice.destroyImageView(mImageView[i]);
        }
    }
}
