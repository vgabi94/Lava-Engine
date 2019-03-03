#pragma once

#include "RenderPass.h"
#include <MemoryPool.h>
#include <Common\Constants.h>

namespace Engine
{
	class SkyPass : public RenderPass
	{
	public:
		void Init() override;
		void PostShaderLoadInit() override;
		void Destroy() override;

		void Recreate() override;
		void Setup() override;

		vk::SubmitInfo GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStages, vk::Semaphore& signalSem) override;

		MEM_POOL_DECLARE(SkyPass);

	private:
		void _Destroy();

		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPoolAndBuffer();
		void RecordCommandBuffer(uint32_t imageIndex);

		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyCommandPool();

		uint32_t mCountVBO;
		struct Material* mMaterial;
		uint32_t mVBO;
	};
}