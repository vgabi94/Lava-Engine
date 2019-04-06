#pragma once

#include <vulkan\vulkan.hpp>
#include <Engine\Texture.h>
#include <Common\MathTypes.h>

namespace Engine
{
	struct PrenvPassResources
	{
		static constexpr vk::Format FORMAT = vk::Format::eR16G16B16A16Sfloat;
		static constexpr uint32_t DIM = 512;

		void Create();
		void Destroy();
		void InitCmdBuffer(vk::CommandPool cmdPool);
		void InitFramebuffer(vk::RenderPass renderpass);

		uint32_t mPrefilterdEnvMapIndex;
		Texture mPrefilterdEnvMap;
		uint32_t mNumMips;
		std::array<Matrix4, 6> mCubeMatrices;
		vk::CommandBuffer mCmdBuffer;
		vk::Semaphore mSemaphore;
		vk::Framebuffer mFramebuffer;
		Texture mOffscreen;
	};
}