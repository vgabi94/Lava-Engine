#include "BrdfPass.h"
#include <Manager\MaterialManager.h>
#include <Manager\PipelineManager.h>
#include <Manager\RenderpassManager.h>
#include "BrdfPassResources.h"

namespace Engine
{
	MEM_POOL_DEFINE(BrdfPass);

	void BrdfPass::Init()
	{
		RenderPass::Init();
		// Used for renderpass synchronization 
		mPipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		CreateRenderPass();
		CreateFramebuffers();
		CreateCommandPoolAndBuffer();

		LOG_INFO("[LOG] BrdfPass init {0:#x}\n", (uint64_t)this);
	}

	void BrdfPass::PostShaderLoadInit()
	{
		mMaterial = g_MaterialManager.NewMaterial("brdf");

		/*mBrdfLutFormat = vk::Format::eR16G16Sfloat;
		constexpr uint32_t dim = 512;

		mBrdfLutIndex = g_TextureManager.CreateTexture2D(dim, dim, 1, 1,
			vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, mBrdfLutFormat);
		mBrdfLut = TextureAt(mBrdfLutIndex);
		mBrdfLut.mSampler = g_TextureManager.CreateSamplerBrdf();*/
	}

	void BrdfPass::Destroy()
	{
		RenderPass::Destroy();
		_Destroy();
		LOG_INFO("[LOG] BrdfPass destroy {0:#x}\n", (uint64_t)this);
		mAllocator.deleteElement(this);
	}

	void BrdfPass::Recreate()
	{
		/*_Destroy();
		Init();
		LOG_INFO("[LOG] BrdfPass recreate {0:#x}\n", (uint64_t)this);*/
	}

	void BrdfPass::Setup()
	{
		RecordCommandBuffer();
	}

	vk::SubmitInfo BrdfPass::GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore& signalSem)
	{
		mWaitStages = { waitStage };
		vk::SubmitInfo submitInfo(
			1, &waitSem,
			mWaitStages.data(),
			1, &mRes->mCmdBuffer,
			1, &signalSem
		);
		return submitInfo;
	}

	vk::SubmitInfo BrdfPass::GetSubmitInfo(vk::Semaphore & signalSem)
	{
		vk::SubmitInfo submitInfo(
			0, nullptr,
			mWaitStages.data(),
			1, &mRes->mCmdBuffer,
			1, &signalSem
		);
		return submitInfo;
	}

	vk::SubmitInfo BrdfPass::GetSubmitInfo(vk::Semaphore & waitSem, vk::PipelineStageFlags waitStage)
	{
		std::array<vk::PipelineStageFlags, 1> waitStages{ waitStage };
		vk::SubmitInfo submitInfo(
			1, &waitSem,
			waitStages.data(),
			1, &mRes->mCmdBuffer,
			0, nullptr
		);
		return submitInfo;
	}

	vk::SubmitInfo BrdfPass::GetSubmitInfo()
	{
		vk::SubmitInfo submitInfo(
			0, nullptr,
			nullptr,
			1, &mRes->mCmdBuffer,
			0, nullptr
		);
		return submitInfo;
	}

	void BrdfPass::_Destroy()
	{
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(0));
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(1));
		DestroyRenderPass();
		DestroyCommandPool();
		//DestroyFramebuffers();
	}

	void BrdfPass::CreateRenderPass()
	{
		vk::AttachmentDescription colorAttachment({},
			BrdfPassResources::FORMAT,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eShaderReadOnlyOptimal
		);

		vk::AttachmentReference colorAttachRef(0, vk::ImageLayout::eColorAttachmentOptimal);

		vk::SubpassDescription subpass({},
			vk::PipelineBindPoint::eGraphics,
			0, nullptr,
			1, &colorAttachRef,
			nullptr, nullptr
		);

		std::array<vk::SubpassDependency, 2> dependencies;
		dependencies[0] = vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlagBits::eMemoryRead,
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
			vk::DependencyFlagBits::eByRegion);
		dependencies[1] = vk::SubpassDependency(0, VK_SUBPASS_EXTERNAL,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
			vk::AccessFlagBits::eMemoryRead,
			vk::DependencyFlagBits::eByRegion);

		vk::RenderPassCreateInfo renderPassInfo({},
			1, &colorAttachment,
			1, &subpass, 2, dependencies.data()
		);

		mRenderPass = g_vkDevice.createRenderPass(renderPassInfo);
	}

	void BrdfPass::CreateFramebuffers()
	{
		//! This is created in BrdfPassResources
		/*mFramebuffer.resize(1);

		vk::ImageView imageViews[] = { mR.mImageView };

		vk::FramebufferCreateInfo info({},
			mRenderPass,
			1,
			&mBrdfLut.mImageView,
			mBrdfLut.mWidth,
			mBrdfLut.mHeight,
			1);

		vk::Framebuffer fbuff = g_vkDevice.createFramebuffer(info);
		mFramebuffer[0] = fbuff;*/
	}

	void BrdfPass::CreateCommandPoolAndBuffer()
	{
		vk::CommandPoolCreateInfo info(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
			vk::CommandPoolCreateFlagBits::eTransient,
			GRAPHICS_FAMILY_INDEX);
		mCommandPool = g_vkDevice.createCommandPool(info);

		//! Created in BrdfPassResources
		/*vk::CommandBufferAllocateInfo allocInfo(
			mCommandPool,
			vk::CommandBufferLevel::ePrimary,
			(uint32_t)mFramebuffer.size());

		mCommandBuffer = g_vkDevice.allocateCommandBuffers(allocInfo);*/
	}

	void BrdfPass::RecordCommandBuffer()
	{
		vk::CommandBuffer& cmdBuf = mRes->mCmdBuffer;
		uint32_t dim = mRes->mBrdfLut.mWidth;

		vk::CommandBufferBeginInfo beginInfo(
			vk::CommandBufferUsageFlagBits::eSimultaneousUse |
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		cmdBuf.begin(beginInfo);

		vk::ClearValue clearValues[] =
		{
			vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})
		};

		const Pipeline& pipe = PipelineOfType(mMaterial->mPipeType);
		const vk::Pipeline& pipeline = pipe.mPipeline;

		vk::RenderPassBeginInfo renderPassInfo(
			mRenderPass,
			mRes->mFramebuffer,
			vk::Rect2D({ 0, 0 }, { dim, dim }),
			1,
			clearValues);

		cmdBuf.beginRenderPass(renderPassInfo,
			vk::SubpassContents::eInline);

		vk::Viewport viewport(0.f, 0.f, (float)dim, (float)dim, 0.f, 1.f);
		cmdBuf.setViewport(0, { viewport });

		vk::Rect2D scissor({}, { dim, dim });
		cmdBuf.setScissor(0, { scissor });

		cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		cmdBuf.draw(3, 1, 0, 0);
		
		cmdBuf.endRenderPass();
		cmdBuf.end();
	}

	void BrdfPass::DestroyRenderPass()
	{
		g_vkDevice.destroyRenderPass(mRenderPass);
	}

	void BrdfPass::DestroyCommandPool()
	{
		g_vkDevice.destroyCommandPool(mCommandPool);
	}

	void BrdfPass::DestroyFramebuffers()
	{
		for (auto& var : mFramebuffer)
		{
			g_vkDevice.destroyFramebuffer(var);
		}
	}
}
