#include "PrenvPass.h"
#include <Manager\PrimitiveManager.h>
#include <Manager\ResourceManager.h>
#include <Manager\MaterialManager.h>
#include <Manager\PipelineManager.h>
#include <Manager\WorldManager.h>
#include <Manager\BufferManager.h>
#include <Manager\RenderpassManager.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>
#include <Common\PushConstantsStructs.h>
#include <cmath>

namespace Engine
{
	MEM_POOL_DEFINE(PrenvPass);

	void PrenvPass::Init()
	{
		RenderPass::Init();
		mPipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		CreateRenderPass();
		CreateFramebuffers();
		CreateCommandPoolAndBuffer();

		LOG_INFO("[LOG] PrenvPass init {0:#x}\n", (uint64_t)this);
	}

	void PrenvPass::PostShaderLoadInit()
	{
		VertexPosList skyBox = g_PrimitiveManager.GetSkyBox();
		mVBO = CreateVertexBuffer(skyBox);
		mCountVBO = skyBox.size();
		mMaterial = g_MaterialManager.NewMaterial("prenv");

		mPrefEnvMapFormat = vk::Format::eR16G16B16A16Sfloat;
		constexpr uint32_t dim = 512;
		mNumMips = static_cast<uint32_t>(std::floor(std::log2(dim))) + 1;

		mPrefilterdEnvMap = g_TextureManager.CreateCubeMapTexture(dim, dim, 1, mNumMips,
			vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, mPrefEnvMapFormat);
		mPrefilterdEnvMap.mSampler = g_TextureManager.CreateSamplerPrenv(mNumMips);

		mOffscreen = g_TextureManager.CreateTexture2D(dim, dim, 1,
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, mPrefEnvMapFormat);
		// TODO: maybe layout transition here?
	}

	void PrenvPass::Destroy()
	{
		RenderPass::Destroy();
		_Destroy();
		LOG_INFO("[LOG] PrenvPass destroy {0:#x}\n", (uint64_t)this);
		mAllocator.deleteElement(this);
	}

	void PrenvPass::Recreate()
	{
		_Destroy();
		Init();
		LOG_INFO("[LOG] PrenvPass recreate {0:#x}\n", (uint64_t)this);
	}

	void PrenvPass::Setup()
	{
		RecordCommandBuffer();
	}

	vk::SubmitInfo PrenvPass::GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore& signalSem)
	{
		mWaitStages = { waitStage };
		vk::SubmitInfo submitInfo(
			1, &waitSem,
			mWaitStages.data(),
			1, &mCommandBuffer[0],
			1, &signalSem
		);
		return submitInfo;
	}

	void PrenvPass::_Destroy()
	{
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(0));
		GDevice.WaitForFence(g_RenderpassManager.GetFenceAt(1));
		DestroyRenderPass();
		DestroyCommandPool();
		DestroyFramebuffers();
	}

	void PrenvPass::CreateRenderPass()
	{
		vk::AttachmentDescription colorAttachment({},
			mPrefEnvMapFormat,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal
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

	void PrenvPass::CreateFramebuffers()
	{
		mFramebuffer.resize(1);
		
		vk::ImageView imageViews[] = { mOffscreen.mImageView };

		vk::FramebufferCreateInfo info({},
			mRenderPass,
			1,
			&mOffscreen.mImageView,
			mOffscreen.mWidth,
			mOffscreen.mHeight,
			1);

		vk::Framebuffer fbuff = g_vkDevice.createFramebuffer(info);
		mFramebuffer[0] = fbuff;
	}

	void PrenvPass::CreateCommandPoolAndBuffer()
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

	void PrenvPass::RecordCommandBuffer()
	{
		vk::CommandBuffer& cmdBuf = mCommandBuffer[0];

		vk::CommandBufferBeginInfo beginInfo(
			vk::CommandBufferUsageFlagBits::eSimultaneousUse |
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		cmdBuf.begin(beginInfo);

		vk::ClearValue clearValues[] =
		{
			vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.2f, 0.0f})
		};

		vk::RenderPassBeginInfo renderPassInfo(
			mRenderPass,
			mFramebuffer[0],
			vk::Rect2D({ 0, 0 }, { mOffscreen.mWidth, mOffscreen.mHeight }),
			1,
			clearValues);

		cmdBuf.beginRenderPass(renderPassInfo,
			vk::SubpassContents::eInline);

		vk::Viewport viewport(0.f, 0.f, (float)mOffscreen.mWidth, (float)mOffscreen.mHeight, 0.f, 1.f);
		cmdBuf.setViewport(0, { viewport });

		vk::Rect2D scissor({}, { mOffscreen.mWidth, mOffscreen.mHeight });
		cmdBuf.setScissor(0, { scissor });

		vk::ImageSubresourceRange imageRange(vk::ImageAspectFlagBits::eColor, 0, mNumMips, 0, 6);
		// We will copy offscreen result intro cubeMap so prepare it for transfer dst
		g_TextureManager.TransitionImageLayout(cmdBuf, mPrefilterdEnvMap.mImage,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, imageRange);

		const Pipeline& pipe = PipelineOfType(mMaterial->mPipeType);

		// TODO delete hdrEnv
		mMaterial->UpdateUniform(0, CurrentWorld->mSkySettings.hdrEnv);

		vk::Pipeline pipeline = pipe.mPipeline;
		cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		SkyPS pc;
		pc.ViewProj = CurrentWorld->mSkyViewProj;
		pc.exposure = CurrentWorld->mSkySettings.exposure;
		pc.gamma = CurrentWorld->mSkySettings.gamma;

		cmdBuf.pushConstants(pipe.mPipelineLayout, vk::ShaderStageFlagBits::eVertex
			| vk::ShaderStageFlagBits::eFragment,
			0, sizeof(SkyPS), &pc);

		mMaterial->Bind(cmdBuf);

		// TODO render all cube faces

		cmdBuf.endRenderPass();

		cmdBuf.end();
	}

	void PrenvPass::DestroyRenderPass()
	{
		g_vkDevice.destroyRenderPass(mRenderPass);
	}

	void PrenvPass::DestroyCommandPool()
	{
		g_vkDevice.destroyCommandPool(mCommandPool);
	}

	void PrenvPass::DestroyFramebuffers()
	{
		for (auto& var : mFramebuffer)
		{
			g_vkDevice.destroyFramebuffer(var);
		}
	}
}