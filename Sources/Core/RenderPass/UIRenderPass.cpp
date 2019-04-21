#include "UIRenderPass.h"
#include <Manager\ResourceManager.h>
#include <Manager\MaterialManager.h>
#include <Manager\PipelineManager.h>
#include <Manager\UIManager.h>
#include <Manager\BufferManager.h>
#include <Engine\Swapchain.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>

namespace Engine
{
	MEM_POOL_DEFINE(UIRenderPass);

	void UIRenderPass::Init()
	{
		RenderPass::Init();
		mPipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		CreateRenderPass();
		CreateFramebuffers();
		CreateCommandPoolAndBuffer();

		LOG_INFO("[LOG] UIRenderPass init {0:#x}\n", (uint64_t)this);
	}

	void UIRenderPass::Destroy()
	{
		RenderPass::Destroy();
		_Destroy();
		LOG_INFO("[LOG] UIRenderPass destroy {0:#x}\n", (uint64_t)this);
		mAllocator.deleteElement(this);
	}

	void UIRenderPass::Recreate()
	{
		_Destroy();
		Init();
		LOG_INFO("[LOG] UIRenderPass recreate {0:#x}\n", (uint64_t)this);
	}

	void UIRenderPass::Setup()
	{
		RecordCommandBuffer(GSwapchain.GetCurrentFrameIndex());
	}

	vk::SubmitInfo UIRenderPass::GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore& signalSem)
	{
		mWaitStages = { waitStage };
		vk::SubmitInfo submitInfo(
			1, &waitSem,
			mWaitStages.data(),
			1, &mCommandBuffer[GSwapchain.GetCurrentFrameIndex()],
			1, &signalSem
		);
		return submitInfo;
	}

	void UIRenderPass::_Destroy()
	{
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(0));
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(1));
		DestroyRenderPass();
		DestroyCommandPool();
		DestroyFramebuffers();
	}

	void UIRenderPass::CreateRenderPass()
	{
		vk::AttachmentDescription depthAttachment({},
			g_ResourceManager.GetDepthFormat(),
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eLoad,
			vk::AttachmentStoreOp::eDontCare,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eDepthStencilAttachmentOptimal,
			vk::ImageLayout::eDepthStencilAttachmentOptimal
		);

		vk::AttachmentDescription colorAttachment({},
			GSwapchain.GetImageFormat(),
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eLoad,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eColorAttachmentOptimal,
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

	void UIRenderPass::CreateFramebuffers()
	{
		mFramebuffer.resize(GSwapchain.GetImageCount());

		for (size_t i = 0; i < mFramebuffer.size(); i++)
		{
			vk::ImageView imageViews[] = { GSwapchain.GetImageViewAt(i), g_ResourceManager.GetDepthImageView() };

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

	void UIRenderPass::CreateCommandPoolAndBuffer()
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

	void UIRenderPass::RecordCommandBuffer(uint32_t imageIndex)
	{
		vk::CommandBufferBeginInfo beginInfo(
			vk::CommandBufferUsageFlagBits::eSimultaneousUse |
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		mCommandBuffer[imageIndex].begin(beginInfo);

		vk::RenderPassBeginInfo renderPassInfo(
			mRenderPass,
			mFramebuffer[imageIndex],
			vk::Rect2D({ 0, 0 }, GSwapchain.GetExtent())
		);

		mCommandBuffer[imageIndex].beginRenderPass(renderPassInfo,
			vk::SubpassContents::eInline);

		g_UIManager.Draw(mCommandBuffer[imageIndex]);

		mCommandBuffer[imageIndex].endRenderPass();

		mCommandBuffer[imageIndex].end();
	}

	void UIRenderPass::DestroyRenderPass()
	{
		g_vkDevice.destroyRenderPass(mRenderPass);
	}

	void UIRenderPass::DestroyCommandPool()
	{
		g_vkDevice.destroyCommandPool(mCommandPool);
	}

	void UIRenderPass::DestroyFramebuffers()
	{
		for (auto& var : mFramebuffer)
		{
			g_vkDevice.destroyFramebuffer(var);
		}
	}
}
