#include "PrenvPassResources.h"
#include <Manager\TextureManager.h>
#include <Engine\Device.h>

namespace Engine
{
	void PrenvPassResources::Create()
	{
		mNumMips = static_cast<uint32_t>(std::floor(std::log2(DIM))) + 1;

		mPrefilterdEnvMapIndex = g_TextureManager.CreateCubeMapTexture(DIM, DIM, 1, mNumMips,
			vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, FORMAT);
		mPrefilterdEnvMap.mSampler = g_TextureManager.CreateSamplerPrenv(mNumMips);
		mPrefilterdEnvMap = TextureAt(mPrefilterdEnvMapIndex);

		uint32_t index = g_TextureManager.CreateTexture2D(DIM, DIM, 1, 1,
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, FORMAT);
		mOffscreen = TextureAt(index);
		// TODO: maybe layout transition here?

		mSemaphore = GDevice.CreateSemaphore();
	}

	void PrenvPassResources::Destroy()
	{
		//mPrefilterdEnvMap.Destroy();
		g_vkDevice.destroyFramebuffer(mFramebuffer);
		g_vkDevice.destroySemaphore(mSemaphore);
	}

	void PrenvPassResources::InitCmdBuffer(vk::CommandPool cmdPool)
	{
		vk::CommandBufferAllocateInfo allocInfo(
			cmdPool,
			vk::CommandBufferLevel::ePrimary,
			(uint32_t)1);

		mCmdBuffer = g_vkDevice.allocateCommandBuffers(allocInfo)[0];
	}
	
	void PrenvPassResources::InitFramebuffer(vk::RenderPass renderpass)
	{
		vk::ImageView imageViews[] = { mOffscreen.mImageView };

		vk::FramebufferCreateInfo info({},
			renderpass,
			1,
			&mOffscreen.mImageView,
			mOffscreen.mWidth,
			mOffscreen.mHeight,
			1);

		vk::Framebuffer fbuff = g_vkDevice.createFramebuffer(info);
		mFramebuffer = fbuff;
	}
}
