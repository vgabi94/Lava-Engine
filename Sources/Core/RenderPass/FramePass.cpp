#include "FramePass.h"
#include <Engine\Swapchain.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>
#include <Manager\WorldManager.h>
#include <Manager\TextureManager.h>
#include <Manager\BufferManager.h>

namespace Engine
{
    MEM_POOL_DEFINE(FramePass);

    void FramePass::Init()
    {
        CreateDepthResources();
        CreateRenderPass();
        CreateFramebuffers();
        CreateCommandPoolAndBuffer();
        CreateFences();

        LOG_INFO("[LOG] FramePass init {0:#x}\n", (uint64_t)this);
    }

    void FramePass::Destroy()
    {
        _Destroy();
        LOG_INFO("[LOG] FramePass destroy {0:#x}\n", (uint64_t)this);
        mAllocator.deleteElement(this);
    }

    void FramePass::Recreate()
    {
        _Destroy();
        Init();
        LOG_INFO("[LOG] FramePass recreate {0:#x}\n", (uint64_t)this);
    }

    void FramePass::Setup()
    {
        CurrentWorld->RecordWorldCommandBuffers(GSwapchain.GetCurrentFrameIndex());
        RecordCommandBuffer(GSwapchain.GetCurrentFrameIndex());
    }

    void FramePass::Render()
    {
        GRAPHICS_QUEUE.submit({ mSubmitInfo }, mFence[GSwapchain.GetCurrentFrameIndex()]);
    }

    void FramePass::Wait()
    {
        uint32_t frameIndex = GSwapchain.GetCurrentFrameIndex();
        GDevice.WaitForFence(mFence[frameIndex]);
        GDevice.ResetFence(mFence[frameIndex]);
    }

    void FramePass::CreateSubmitInfo(vk::Semaphore & imageSem, vk::Semaphore & renderSem)
    {
        mWaitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        vk::SubmitInfo submitInfo(
            1, &imageSem,
            mWaitStages.data(),
            1, &mCommandBuffer[GSwapchain.GetCurrentFrameIndex()],
            1, &renderSem
        );
        mSubmitInfo = submitInfo;
    }

    void FramePass::_Destroy()
    {
        GDevice.WaitForFence(mFence[0]);
        GDevice.WaitForFence(mFence[1]);
        DestroyRenderPass();
        DestroyCommandPool();
        DestroyFramebuffers();
        DestroyDepthResources();
        DestroyFences();
    }

    void FramePass::CreateRenderPass()
    {
        vk::AttachmentDescription depthAttachment({},
            mDepthFormat,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eDontCare,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        );

        vk::AttachmentDescription colorAttachment({},
            GSwapchain.GetImageFormat(),
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR
        );

        vk::AttachmentReference colorAttachRef(0, vk::ImageLayout::eColorAttachmentOptimal);
        vk::AttachmentReference depthAttachRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::SubpassDescription subpass({},
            vk::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, &colorAttachRef,
            nullptr, &depthAttachRef
        );

        vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput, {},
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
            {});

        vk::AttachmentDescription attachments[] = { colorAttachment, depthAttachment };
        vk::RenderPassCreateInfo renderPassInfo({},
            2, attachments,
            1, &subpass, 1, &dependency
        );

        mRenderPass = g_vkDevice.createRenderPass(renderPassInfo);
    }

    void FramePass::CreateDepthResources()
    {
        vk::Extent3D extent = { GSwapchain.GetExtent().width, GSwapchain.GetExtent().height, 1 };
        mDepthFormat = vk::Format::eD32Sfloat;
        
        mDepthImage = g_TextureManager.CreateImage2D(extent,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            VMA_MEMORY_USAGE_GPU_ONLY, 0, mDepthAlloc, nullptr, mDepthFormat);

        mDepthImageView = g_TextureManager.CreateImageView2D(mDepthImage,
            mDepthFormat, vk::ImageAspectFlagBits::eDepth);

        g_TextureManager.TransitionImageLayout(mDepthImage, mDepthFormat,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    void FramePass::CreateFences()
    {
        uint32_t size = GSwapchain.GetImageCount();
        mFence.resize(size);
        vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

        for (size_t i = 0; i < size; i++)
        {
            mFence[i] = g_vkDevice.createFence(fenceInfo);
        }
    }

    void FramePass::CreateFramebuffers()
    {
        mFramebuffer.resize(GSwapchain.GetImageCount());

        for (size_t i = 0; i < mFramebuffer.size(); i++)
        {
            vk::ImageView imageViews[] = { GSwapchain.GetImageViewAt(i), mDepthImageView };

            vk::FramebufferCreateInfo info({},
                mRenderPass,
                2,
                imageViews,
                GSwapchain.GetExtent().width,
                GSwapchain.GetExtent().height,
                1);

            vk::Framebuffer fbuff = g_vkDevice.createFramebuffer(info);
            mFramebuffer[i] = fbuff;
        }
    }

    void FramePass::CreateCommandPoolAndBuffer()
    {
        vk::CommandPoolCreateInfo info(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
            vk::CommandPoolCreateFlagBits::eTransient,
            GRAPHICS_FAMILY_INDEX);
        mCommandPool = g_vkDevice.createCommandPool(info);

        vk::CommandBufferAllocateInfo allocInfo(
            mCommandPool,
            vk::CommandBufferLevel::ePrimary,
            (uint32_t)mFramebuffer.size());

        mCommandBuffer = g_vkDevice.allocateCommandBuffers(allocInfo);
    }

    void FramePass::RecordCommandBuffer(uint32_t imageIndex)
    {
        vk::CommandBufferBeginInfo beginInfo(
            vk::CommandBufferUsageFlagBits::eSimultaneousUse |
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        mCommandBuffer[imageIndex].begin(beginInfo);

        vk::ClearValue clearValues[] =
        {
            vk::ClearColorValue(std::array<float, 4>{0.f, 0.f, 0.f, 1.f}),
            vk::ClearDepthStencilValue(1.0f, 0.0f)
        };
        vk::RenderPassBeginInfo renderPassInfo(
            mRenderPass,
            mFramebuffer[imageIndex],
            vk::Rect2D({ 0, 0 }, GSwapchain.GetExtent()),
            2,
            clearValues);

        mCommandBuffer[imageIndex].beginRenderPass(renderPassInfo,
            vk::SubpassContents::eSecondaryCommandBuffers);

        mCommandBuffer[imageIndex].executeCommands(CurrentWorld->GetWorldCommandBuffer(imageIndex));

        mCommandBuffer[imageIndex].endRenderPass();

        mCommandBuffer[imageIndex].end();
    }

    void FramePass::DestroyRenderPass()
    {
        g_vkDevice.destroyRenderPass(mRenderPass);
    }

    void FramePass::DestroyCommandPool()
    {
        g_vkDevice.destroyCommandPool(mCommandPool);
    }

    void FramePass::DestroyDepthResources()
    {
        g_vkDevice.destroyImageView(mDepthImageView);
        vmaDestroyImage(GVmaAllocator, mDepthImage, mDepthAlloc);
    }

    void FramePass::DestroyFences()
    {
        for (auto& f : mFence)
        {
            g_vkDevice.destroyFence(f);
        }
    }

    void FramePass::DestroyFramebuffers()
    {
        for (auto& var : mFramebuffer)
        {
            g_vkDevice.destroyFramebuffer(var);
        }
    }
}
