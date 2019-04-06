#include "ResourceManager.h"
#include <Common\Constants.h>
#include <Manager\TextureManager.h>
#include <Manager\BufferManager.h>
#include <Manager\RenderpassManager.h>
#include <RenderPass\PrenvPass.h>
#include <RenderPass\BrdfPass.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>
#include <setslots.h>

#define GResourceManager Engine::g_ResourceManager

namespace Engine
{
	template<typename T>
	static void SetupPasses(const std::vector<T*> passes)
	{
		for (auto pass : passes)
		{
			pass->Setup();
		}
	}
	
	template<typename T>
	static void ExecutePasses(const std::vector<T*> passes)
	{
		std::vector<vk::SubmitInfo> submitInfos;
		submitInfos.reserve(passes.size());
		vk::Fence fence = GDevice.CreateFence();
		
		if (passes.size() == 1)
		{
			submitInfos.push_back(passes[0]->GetSubmitInfo());
		}
		else
		{
			submitInfos.push_back(passes[0]->GetSubmitInfo(passes[0]->GetSemaphore()));

			for (size_t i = 1; i < passes.size() - 1; i++)
			{
				submitInfos.push_back(
					passes[i]->GetSubmitInfo(
						passes[i - 1]->GetSemaphore(),
						passes[i - 1]->GetPipelineStage(),
						passes[i]->GetSemaphore()
					)
				);
			}

			const uint32_t last = passes.size() - 1;
			submitInfos.push_back(
				passes[last]->GetSubmitInfo(
					passes[last - 1]->GetSemaphore(),
					passes[last - 1]->GetPipelineStage()
				)
			);
		}

		GRAPHICS_QUEUE.submit(submitInfos, fence);
		GDevice.WaitForFence(fence);
		g_vkDevice.destroyFence(fence);
	}

    ResourceManager g_ResourceManager;

    void ResourceManager::Init()
    {
		CreateDepthBuffer();
		InitDescriptorAllocatorsAndSets();
		mFrameConsts.Init();
		mIBLdone = false;
    }

    void ResourceManager::Destroy()
    {
		DestroyDepthBuffer();
		DestroyDescriptorAllocators();
		DestroyRenderPassResources();
		mLights.Destroy();
		mFrameConsts.Destroy();
    }

	void ResourceManager::CreateDepthBuffer()
	{
		vk::Extent3D extent = { g_Engine.GetWidth(), g_Engine.GetHeight(), 1 };
		mDepthFormat = vk::Format::eD32Sfloat;

		mDepthImage = g_TextureManager.CreateImage2D(extent, 1,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, mDepthAlloc, nullptr, mDepthFormat);

		mDepthImageView = g_TextureManager.CreateImageView2D(mDepthImage, 1,
			mDepthFormat, vk::ImageAspectFlagBits::eDepth);

		g_TextureManager.TransitionImageLayout(mDepthImage, mDepthFormat,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	}

	void ResourceManager::DestroyDepthBuffer()
	{
		g_vkDevice.destroyImageView(mDepthImageView);
		vmaDestroyImage(GVmaAllocator, mDepthImage, mDepthAlloc);
	}

    vk::DescriptorSet ResourceManager::GetDescriptorSet(uint32_t slot) const
    {
        THROW_IF(slot > 8, "Set slot must not be greater than 8!");
        THROW_IF(slot == 0, "Set slot 0 is reserved for materials!");
        return mDescSets[slot - 1];
    }
	
	vk::DescriptorSetLayout ResourceManager::GetDescriptorSetLayoutAt(uint32_t slot) const
	{
		THROW_IF(slot > 8, "Set slot must not be greater than 8!");
		THROW_IF(slot == 0, "Set slot 0 is reserved for materials!");
		return mDescLayout[slot - 1];
	}

	uint32_t ResourceManager::AddIBLProbeInfo(const IBLProbeInfo& probe)
	{
		uint32_t ind = mPrenvRes.size();

		// Prefiltered environment map pass
		PrenvPass* prenv = g_RenderpassManager.GetPass<PrenvPass>(RPConst::PRENV);
		PrenvPassResources prenvRes;
		prenvRes.Create();
		prenvRes.InitFramebuffer(prenv->GetRenderpass());
		prenvRes.InitCmdBuffer(prenv->GetCommandPool());
		std::copy(probe.matrices, probe.matrices + 6, prenvRes.mCubeMatrices.begin());
		mPrenvRes.push_back(prenvRes);

		// Brdf lut pass
		BrdfPass* brdf = g_RenderpassManager.GetPass<BrdfPass>(RPConst::BRDF);
		BrdfPassResources brdfRes;
		brdfRes.Create();
		brdfRes.InitFamebuffer(brdf->GetRenderpass());
		brdfRes.InitCmdBuffer(brdf->GetCommandPool());
		mBrdfRes.push_back(brdfRes);

		// TODO irad passes
		
		return ind;
	}

	void ResourceManager::ExecuteIBLPasses()
	{
		if (mIBLdone) return;

		//? TODO irrad ?

		// Setup passes
		PrenvPass* prenvPass = g_RenderpassManager.GetPass<PrenvPass>(RPConst::PRENV);
		BrdfPass* brdfPass = g_RenderpassManager.GetPass<BrdfPass>(RPConst::BRDF);
		for (size_t i = 0; i < mPrenvRes.size(); i++)
		{
			prenvPass->SetResources(&mPrenvRes[i]);
			brdfPass->SetResources(&mBrdfRes[i]);
			prenvPass->Setup();
			brdfPass->Setup();
		}

		// Execute passes
		std::vector<vk::SubmitInfo> submitInfosPrenv;
		std::vector<vk::SubmitInfo> submitInfosBrdf;
		submitInfosPrenv.reserve(mPrenvRes.size());
		submitInfosBrdf.reserve(mBrdfRes.size());
		vk::Fence fence1 = GDevice.CreateFence();
		vk::Fence fence2 = GDevice.CreateFence();

		if (mPrenvRes.size() == 1)
		{
			submitInfosPrenv.push_back(prenvPass->GetSubmitInfo());
			submitInfosBrdf.push_back(brdfPass->GetSubmitInfo());
		}
		else
		{
			submitInfosPrenv.push_back(prenvPass->GetSubmitInfo(mPrenvRes[0].mSemaphore));
			submitInfosBrdf.push_back(brdfPass->GetSubmitInfo(mBrdfRes[0].mSemaphore));

			for (size_t i = 1; i < mPrenvRes.size() - 1; i++)
			{
				prenvPass->SetResources(&mPrenvRes[i]);
				submitInfosPrenv.push_back(
					prenvPass->GetSubmitInfo(
						mPrenvRes[i - 1].mSemaphore,
						prenvPass->GetPipelineStage(),
						mPrenvRes[i].mSemaphore
					)
				);

				brdfPass->SetResources(&mBrdfRes[i]);
				submitInfosBrdf.push_back(
					brdfPass->GetSubmitInfo(
						mBrdfRes[i - 1].mSemaphore,
						brdfPass->GetPipelineStage(),
						mBrdfRes[i].mSemaphore
					)
				);
			}

			const uint32_t last = mPrenvRes.size() - 1;

			prenvPass->SetResources(&mPrenvRes[last]);
			submitInfosPrenv.push_back(
				prenvPass->GetSubmitInfo(
					mPrenvRes[last - 1].mSemaphore,
					prenvPass->GetPipelineStage()
				)
			);

			brdfPass->SetResources(&mBrdfRes[last]);
			submitInfosBrdf.push_back(
				brdfPass->GetSubmitInfo(
					mBrdfRes[last - 1].mSemaphore,
					brdfPass->GetPipelineStage()
				)
			);
		}

		GRAPHICS_QUEUE.submit(submitInfosPrenv, fence1);
		GRAPHICS_QUEUE.submit(submitInfosBrdf, fence2);
		GDevice.WaitForFence(fence1);
		GDevice.WaitForFence(fence2);
		g_vkDevice.destroyFence(fence1);
		g_vkDevice.destroyFence(fence2);

		mIBLdone = true;
	}

	uint32_t ResourceManager::GetPrefEnvMap(uint32_t ind) const
	{
		THROW_IF(ind >= mPrenvRes.size(), "Env map passes index out of range!");
		return mPrenvRes[ind].mPrefilterdEnvMapIndex;
	}

	uint32_t ResourceManager::GetBrdfMap(uint32_t ind) const
	{
		THROW_IF(ind >= mBrdfRes.size(), "Brdf lut passes index out of range!");
		return mBrdfRes[ind].mBrdfLutIndex;
	}

	void ResourceManager::InitDescriptorAllocatorsAndSets()
	{
		std::vector<vk::DescriptorPoolSize> poolSizes;
		poolSizes.resize(1);
		vk::DescriptorSetLayoutCreateInfo descSetCI;

		// Light source desc allocator
		constexpr uint32_t lightIndex = LIGHTSOURCE_SLOT - 1;
		
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = 1;

		vk::DescriptorSetLayoutBinding binding(0, poolSizes[0].type, 1, vk::ShaderStageFlagBits::eAll);
		descSetCI.bindingCount = 1;
		descSetCI.pBindings = &binding;

		mDescLayout[lightIndex] = g_vkDevice.createDescriptorSetLayout(descSetCI);
		mDescAllocators[lightIndex].Init(poolSizes, mDescLayout[lightIndex]);
		mDescSets[lightIndex] = mDescAllocators[lightIndex].AllocateDescriptorSet();

		// Frame consts desc allocator
		constexpr uint32_t frameIndex = FRAMECONSTS_SLOT - 1;
		// same pool size and binding as light source
		mDescLayout[frameIndex] = g_vkDevice.createDescriptorSetLayout(descSetCI);
		mDescAllocators[frameIndex].Init(poolSizes, mDescLayout[frameIndex]);
		mDescSets[frameIndex] = mDescAllocators[frameIndex].AllocateDescriptorSet();
	}
	
	void ResourceManager::DestroyDescriptorAllocators()
	{
		for (auto& descAlloc : mDescAllocators)
		{
			descAlloc.Destroy();
		}
	}
	
	void ResourceManager::DestroyRenderPassResources()
	{
		for (auto& res : mPrenvRes)
		{
			res.Destroy();
		}

		for (auto& res : mBrdfRes)
		{
			res.Destroy();
		}
	}
}

