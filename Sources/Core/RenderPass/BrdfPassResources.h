#pragma once

#include <vulkan\vulkan.hpp>
#include <Engine\Texture.h>
#include <Common\MathTypes.h>

namespace Engine
{
	struct BrdfPassResources
	{
		static constexpr vk::Format FORMAT = vk::Format::eR16G16Sfloat;
		static constexpr uint32_t DIM = 512;

		void Create();
		void Destroy();
		void InitFamebuffer(vk::RenderPass pass);
		void InitCmdBuffer(vk::CommandPool cmdPool);

		uint32_t mBrdfLutIndex;
		Texture mBrdfLut;
		vk::Framebuffer mFramebuffer;
		vk::CommandBuffer mCmdBuffer;
		vk::Semaphore mSemaphore;
		//vk::RenderPass mRenderPass;
	};
}