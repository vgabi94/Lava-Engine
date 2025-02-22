#pragma once

#include "RenderPass.h"
#include <MemoryPool.h>
#include <Manager\TextureManager.h>
#include <Common\Constants.h>
#include <Common\MathTypes.h>
#include "PrenvPassResources.h"

namespace Engine
{
	class PrenvPass : public RenderPass
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

		//const Texture& GetPrefEnvMap() const { return mRes->mPrefilterdEnvMap; }
		//uint32_t GetPrefEnvMapIndex() const { return mRes->mPrefilterdEnvMapIndex; }

		void SetResources(PrenvPassResources* res) { mRes = res; }

		vk::RenderPass GetRenderpass() const { return mRenderPass; }
		vk::CommandPool GetCommandPool() const { return mCommandPool; }

		MEM_POOL_DECLARE(PrenvPass);

	private:
		void _Destroy();

		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateCommandPoolAndBuffer();
		void RecordCommandBuffer();

		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyCommandPool();

		uint32_t mCountVBO;
		struct Material* mMaterial;
		uint32_t mVBO;
		/*Texture mOffscreen;
		uint32_t mPrefilterdEnvMapIndex;
		Texture mPrefilterdEnvMap;
		uint32_t mNumMips;
		std::array<Matrix4, 6> mCubeMatrices;*/
		PrenvPassResources* mRes;
	};
}