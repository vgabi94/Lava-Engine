#include "ResourceManager.h"
#include <Common\Constants.h>
#include <Manager\TextureManager.h>
#include <Manager\BufferManager.h>
#include <Manager\RenderpassManager.h>
#include <RenderPass\PrenvPass.h>
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
		mLights.Destroy();
		mFrameConsts.Destroy();
    }

	void ResourceManager::CreateDepthBuffer()
	{
		vk::Extent3D extent = { g_Engine.GetWidth(), g_Engine.GetHeight(), 1 };
		mDepthFormat = vk::Format::eD32Sfloat;

		mDepthImage = g_TextureManager.CreateImage2D(extent,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			VMA_MEMORY_USAGE_GPU_ONLY, 0, mDepthAlloc, nullptr, mDepthFormat);

		mDepthImageView = g_TextureManager.CreateImageView2D(mDepthImage,
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
		uint32_t ind = mPrefEnvPasses.size();
		uint32_t passIndex;

		// Prefiltered environment map pass
		PrenvPass* prenv = g_RenderpassManager.AddPassTask<PrenvPass>(passIndex);
		std::copy(probe.matrices, probe.matrices + 6, prenv->mCubeMatrices.begin());
		mPrefEnvPasses.push_back(passIndex);
		
		return ind;
	}

	void ResourceManager::ExecuteIBLPasses()
	{
		if (mIBLdone) return;

		//std::vector<PrenvPass*> irradPasses;  TODO
		std::vector<PrenvPass*> prenvPasses;
		prenvPasses.reserve(mPrefEnvPasses.size());
		//std::vector<PrenvPass*> brdfPasses; TODO

		for (size_t i = 0; i < mPrefEnvPasses.size(); i++)
		{
			prenvPasses[i] = g_RenderpassManager.GetPassTaskAt<PrenvPass>(mPrefEnvPasses[i]);
		}
		// TODO irrad and brdf

		SetupPasses(prenvPasses);
		//TODO irrad and brdf

		ExecutePasses(prenvPasses);
		// TODO irrad and brdf

		mIBLdone = true;
	}

	inline const Texture & ResourceManager::GetPrefEnvMap(uint32_t ind)
	{
		THROW_IF(ind >= mPrefEnvPasses.size(), "Map index out of range!");
		return g_RenderpassManager.GetPassTaskAt<PrenvPass>(mPrefEnvPasses[ind])->mPrefilterdEnvMap;
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
}

