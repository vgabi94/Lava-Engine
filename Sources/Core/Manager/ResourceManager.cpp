#include "ResourceManager.h"
#include <Common\Constants.h>
#include <Manager\TextureManager.h>
#include <Manager\BufferManager.h>
#include <Engine\Device.h>
#include <Engine\Engine.h>

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

    vk::DescriptorSetLayout ResourceManager::GetDescriptorSetLayoutAt(uint32_t index)
    {
        THROW_IF(index > 7, "Set index must not be greater than 7!");
        THROW_IF(index == 0, "Set index 0 is reserved for materials!");
        return mDescLayouts[index];
    }
}

