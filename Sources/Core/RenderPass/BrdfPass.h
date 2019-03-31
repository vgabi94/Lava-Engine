#pragma once

#include "RenderPass.h"
#include <MemoryPool.h>
#include <Manager\TextureManager.h>
#include <Common\Constants.h>
#include <Common\MathTypes.h>

namespace Engine
{
	class BrdfPass : public RenderPass
	{
		friend class ResourceManager;

	public:
		void Init() override;
		void PostShaderLoadInit() override;
		void Destroy() override;

		void Recreate() override;
		void Setup() override;

		vk::SubmitInfo GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStages, vk::Semaphore& signalSem) override;
		vk::SubmitInfo GetSubmitInfo(vk::Semaphore& signalSem) override;
		vk::SubmitInfo GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStages) override;
		vk::SubmitInfo GetSubmitInfo() override;

		const Texture& GetBrdfLut() const { return mBrdfLut; }
		uint32_t GetBrdfLutIndex() const { return mBrdfLutIndex; }

		MEM_POOL_DECLARE(BrdfPass);

	private:
		void _Destroy();

		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPoolAndBuffer();
		void RecordCommandBuffer();

		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyCommandPool();

		struct Material* mMaterial;
		uint32_t mBrdfLutIndex;
		Texture mBrdfLut;
		vk::Format mBrdfLutFormat;
	};
}