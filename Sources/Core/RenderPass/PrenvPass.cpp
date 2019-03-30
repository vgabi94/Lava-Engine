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
		// Used for renderpass synchronization 
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

	vk::SubmitInfo PrenvPass::GetSubmitInfo(vk::Semaphore & signalSem)
	{
		vk::SubmitInfo submitInfo(
			0, nullptr,
			mWaitStages.data(),
			1, &mCommandBuffer[0],
			1, &signalSem
		);
		return submitInfo;
	}

	vk::SubmitInfo PrenvPass::GetSubmitInfo(vk::Semaphore & waitSem, vk::PipelineStageFlags waitStage)
	{
		std::array<vk::PipelineStageFlags, 1> waitStages{ waitStage };
		vk::SubmitInfo submitInfo(
			1, &waitSem,
			waitStages.data(),
			1, &mCommandBuffer[0],
			0, nullptr
		);
		return submitInfo;
	}

	vk::SubmitInfo PrenvPass::GetSubmitInfo()
	{
		vk::SubmitInfo submitInfo(
			0, nullptr,
			nullptr,
			1, &mCommandBuffer[0],
			0, nullptr
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
		uint32_t dim = mOffscreen.mWidth;

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
			vk::Rect2D({ 0, 0 }, { dim, dim }),
			1,
			clearValues);

		vk::Viewport viewport(0.f, 0.f, (float)dim, (float)dim, 0.f, 1.f);
		cmdBuf.setViewport(0, { viewport });

		vk::Rect2D scissor({}, { dim, dim });
		cmdBuf.setScissor(0, { scissor });

		{ // Prepare prenv cube map for transfer operation
			vk::ImageSubresourceRange imageRange(vk::ImageAspectFlagBits::eColor, 0, mNumMips, 0, 6);
			g_TextureManager.TransitionImageLayout(cmdBuf, mPrefilterdEnvMap.mImage,
				vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, imageRange);
		}

		const Pipeline& pipe = PipelineOfType(mMaterial->mPipeType);
		const vk::Pipeline& pipeline = pipe.mPipeline;

		mMaterial->UpdateUniform(0, CurrentWorld->mSkySettings.hdrTex);

		SkyPS skyPS;
		skyPS.exposure = CurrentWorld->mSkySettings.exposure;
		skyPS.gamma = CurrentWorld->mSkySettings.gamma;

		PrenvPS prenvPS;
		prenvPS.numSamples = 32u;

		for (uint32_t m = 0; m < mNumMips; m++)
		{
			prenvPS.roughness = (float)m / (float)(mNumMips - 1);
			for (uint32_t f = 0; f < 6; f++)
			{
				viewport.width = static_cast<float>(dim * std::pow(0.5f, m));
				viewport.height = static_cast<float>(dim * std::pow(0.5f, m));
				cmdBuf.setViewport(0, { viewport });

				cmdBuf.beginRenderPass(renderPassInfo,
					vk::SubpassContents::eInline);

				skyPS.ViewProj = mCubeMatrices[f];

				cmdBuf.pushConstants(pipe.mPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(SkyPS), &skyPS);
				cmdBuf.pushConstants(pipe.mPipelineLayout, vk::ShaderStageFlagBits::eFragment, 0, sizeof(PrenvPS), &prenvPS);

				cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

				mMaterial->Bind(cmdBuf);

				vk::Buffer vbo = BufferAt(mVBO);
				vk::DeviceSize vertOffset = 0;

				cmdBuf.bindVertexBuffers(0, 1, &vbo, &vertOffset);

				cmdBuf.draw(mCountVBO, 1, 0, 0);

				cmdBuf.endRenderPass();

				{ // Prepare offscreen image for transfer into cube face
					vk::ImageSubresourceRange imageRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
					g_TextureManager.TransitionImageLayout(cmdBuf, mOffscreen.mImage, vk::ImageLayout::eColorAttachmentOptimal,
						vk::ImageLayout::eTransferSrcOptimal, imageRange);
				}

				vk::ImageCopy copyRegion = vk::ImageCopy(
					vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
					vk::Offset3D(0, 0, 0),
					vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, m, f, 1),
					vk::Offset3D(0, 0, 0),
					vk::Extent3D((uint32_t)viewport.width, (uint32_t)viewport.height, 1)
				);

				cmdBuf.copyImage(mOffscreen.mImage, vk::ImageLayout::eTransferSrcOptimal,
					mPrefilterdEnvMap.mImage, vk::ImageLayout::eTransferDstOptimal,
					1, &copyRegion);

				{ // Transfer offscreen image back to color attach optimal
					vk::ImageSubresourceRange imageRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
					g_TextureManager.TransitionImageLayout(cmdBuf, mOffscreen.mImage, vk::ImageLayout::eTransferSrcOptimal,
						vk::ImageLayout::eColorAttachmentOptimal, imageRange);
				}
			}
		}

		{ // Prepare prenv cube map for shader operations
			vk::ImageSubresourceRange imageRange(vk::ImageAspectFlagBits::eColor, 0, mNumMips, 0, 6);
			g_TextureManager.TransitionImageLayout(cmdBuf, mPrefilterdEnvMap.mImage,
				vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, imageRange);
		}

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