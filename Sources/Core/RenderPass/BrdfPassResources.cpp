#include "BrdfPassResources.h"
#include <Manager\TextureManager.h>
#include <Engine\Device.h>

namespace Engine
{
	void BrdfPassResources::Create()
	{
		mBrdfLutIndex = g_TextureManager.CreateTexture2D(DIM, DIM, 1, 1,
			vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, FORMAT);
		mBrdfLut = TextureAt(mBrdfLutIndex);
		mBrdfLut.mSampler = g_TextureManager.CreateSamplerBrdf();
		mSemaphore = GDevice.CreateSemaphore();
	}

	void BrdfPassResources::Destroy()
	{
		//mBrdfLut.Destroy();
		g_vkDevice.destroyFramebuffer(mFramebuffer);
		g_vkDevice.destroySemaphore(mSemaphore);
	}

	void BrdfPassResources::InitFamebuffer(vk::RenderPass pass)
	{
		vk::ImageView imageViews[] = { mBrdfLut.mImageView };

		vk::FramebufferCreateInfo info({},
			pass,
			1,
			&mBrdfLut.mImageView,
			mBrdfLut.mWidth,
			mBrdfLut.mHeight,
			1);

		vk::Framebuffer fbuff = g_vkDevice.createFramebuffer(info);
		mFramebuffer = fbuff;
	}

	void BrdfPassResources::InitCmdBuffer(vk::CommandPool cmdPool)
	{
		vk::CommandBufferAllocateInfo allocInfo(
			cmdPool,
			vk::CommandBufferLevel::ePrimary,
			1);

		mCmdBuffer = g_vkDevice.allocateCommandBuffers(allocInfo)[0];
	}
}
