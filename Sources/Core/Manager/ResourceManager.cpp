#include "ResourceManager.h"
#include <Common\Constants.h>
#include <Manager\TextureManager.h>
#include <Manager\BufferManager.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>
#include <setslots.h>

#define GResourceManager Engine::g_ResourceManager

namespace Engine
{
    ResourceManager g_ResourceManager;

    void ResourceManager::Init()
    {
		CreateDepthBuffer();
    }

    void ResourceManager::Destroy()
    {
		DestroyDepthBuffer();
		DestroyDescriptorAllocators();
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

    /*vk::DescriptorSet ResourceManager::GetDescriptorSet(uint32_t slot) const
    {
        THROW_IF(slot > 8, "Set slot must not be greater than 8!");
        THROW_IF(slot == 0, "Set slot 0 is reserved for materials!");
        return mDescSets[slot - 1];
    }*/
	
	vk::DescriptorSetLayout ResourceManager::GetDescriptorSetLayoutAt(uint32_t slot) const
	{
		THROW_IF(slot > 8, "Set slot must not be greater than 8!");
		THROW_IF(slot == 0, "Set slot 0 is reserved for materials!");
		return mDescLayout[slot - 1];
	}

	void ResourceManager::WriteDescriptorAtSlot(uint32_t slot)
	{
		THROW_IF(slot > 8, "Set slot must not be greater than 8!");
		THROW_IF(slot == 0, "Set slot 0 is reserved for materials!");
		
		uint32_t index = slot - 1;
		vk::WriteDescriptorSet writeDescSet;
		writeDescSet.descriptorCount = 1;
		writeDescSet.descriptorType = vk::DescriptorType::eUniformBuffer;
		writeDescSet.dstArrayElement = 0;
		writeDescSet.dstBinding = 0;
		writeDescSet.dstSet = mDescSets[index];
		vk::DescriptorBufferInfo bufferInfo(mLights.GetBuffer(), 0, VK_WHOLE_SIZE);
		writeDescSet.pBufferInfo = &bufferInfo;
		g_vkDevice.updateDescriptorSets({ writeDescSet }, { });
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
		constexpr uint32_t frameIndex = LIGHTSOURCE_SLOT - 1;
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

