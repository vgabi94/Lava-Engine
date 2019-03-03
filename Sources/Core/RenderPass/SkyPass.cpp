#include "SkyPass.h"
#include <Manager\PrimitiveManager.h>
#include <Manager\ResourceManager.h>
#include <Manager\MaterialManager.h>
#include <Manager\PipelineManager.h>
#include <Manager\WorldManager.h>
#include <Manager\BufferManager.h>
#include <Engine\Swapchain.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>
#include <Common\PushConstantsStructs.h>

namespace Engine
{
	void SkyPass::Init()
	{
		RenderPass::Init();
		mPipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		
		CreateRenderPass();
		CreateFramebuffers();
		CreateCommandPoolAndBuffer();

		LOG_INFO("[LOG] SkyPass init {0:#x}\n", (uint64_t)this);
	}

	void SkyPass::PostShaderLoadInit()
	{
		VertexPosList skyBox = g_PrimitiveManager.GetSkyBox();
		mVBO = CreateVertexBuffer(skyBox);
		mCountVBO = skyBox.size();
		mMaterial = g_MaterialManager.NewMaterial("sky");
	}

	void SkyPass::Destroy()
	{
		RenderPass::Destroy();
		_Destroy();
		LOG_INFO("[LOG] SkyPass destroy {0:#x}\n", (uint64_t)this);
		mAllocator.deleteElement(this);
	}

	void SkyPass::Recreate()
	{
		_Destroy();
		Init();
		LOG_INFO("[LOG] SkyPass recreate {0:#x}\n", (uint64_t)this);
	}

	void SkyPass::Setup()
	{
		RecordCommandBuffer(GSwapchain.GetCurrentFrameIndex());
	}

	vk::SubmitInfo SkyPass::GetSubmitInfo(vk::Semaphore & waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore & signalSem)
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
	
	void SkyPass::_Destroy()
	{
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(0));
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(1));
		DestroyRenderPass();
		DestroyCommandPool();
		DestroyFramebuffers();
	}
	
	void SkyPass::CreateRenderPass()
	{
		vk::AttachmentDescription depthAttachment({},
			g_ResourceManager.GetDepthFormat(),
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eLoad,
			vk::AttachmentStoreOp::eDontCare,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthStencilAttachmentOptimal
		);

		vk::AttachmentDescription colorAttachment({},
			GSwapchain.GetImageFormat(),
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eLoad,
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
	
	void SkyPass::CreateFramebuffers()
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
	
	void SkyPass::CreateCommandPoolAndBuffer()
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
	
	void SkyPass::RecordCommandBuffer(uint32_t imageIndex)
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

		mCommandBuffer[imageIndex].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		vk::Viewport viewport(0.f, 0.f, (float)GWINDOW_WIDTH, (float)GWINDOW_HEIGHT, 0.f, 1.f);
		mCommandBuffer[imageIndex].setViewport(0, { viewport });

		vk::Rect2D scissor({}, { GWINDOW_WIDTH, GWINDOW_HEIGHT });
		mCommandBuffer[imageIndex].setScissor(0, { scissor });

		const Pipeline& pipe = PipelineOfType(mMaterial->mPipeType);
		vk::Pipeline pipeline = pipe.mPipeline;
		mCommandBuffer[imageIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		SkyPS pc;
		pc.ViewProj = CurrentWorld->mSkyViewProj;
		pc.exposure = 16.0f; // TODO params
		pc.gamma = 2.2f;

		mCommandBuffer[imageIndex].pushConstants(pipe.mPipelineLayout, vk::ShaderStageFlagBits::eVertex
			| vk::ShaderStageFlagBits::eFragment,
			0, sizeof(SkyPS), &pc);

		mMaterial->Bind(mCommandBuffer[imageIndex]);

		vk::Buffer vbo = BufferAt(mVBO);
		vk::DeviceSize vertOffset = 0;

		mCommandBuffer[imageIndex].bindVertexBuffers(0, 1, &vbo, &vertOffset);

		mCommandBuffer[imageIndex].draw(mSkyBox.size(), 1, 0, 0);

		mCommandBuffer[imageIndex].endRenderPass();

		mCommandBuffer[imageIndex].end();
	}
	
	void SkyPass::DestroyRenderPass()
	{
		g_vkDevice.destroyRenderPass(mRenderPass);
	}
	
	void SkyPass::DestroyFramebuffers()
	{
		for (auto& var : mFramebuffer)
		{
			g_vkDevice.destroyFramebuffer(var);
		}
	}
	
	void SkyPass::DestroyCommandPool()
	{
		g_vkDevice.destroyCommandPool(mCommandPool);
	}
}

