#pragma once

#include <vulkan\vulkan.hpp>
#include <array>
#include <vk_mem_alloc.h>

namespace Engine
{
    class ResourceManager
    {
    public:
        void Init();
        void Destroy();

		void CreateDepthBuffer();
		void DestroyDepthBuffer();

		vk::Image GetDepthImage() const { return mDepthImage; }
		vk::Format GetDepthFormat() const { return mDepthFormat; }
		vk::ImageView GetDepthImageView() const { return mDepthImageView; }

        vk::DescriptorSetLayout GetDescriptorSetLayoutAt(uint32_t index);

    private:
        std::array<vk::DescriptorSetLayout, 8> mDescLayouts;

		vk::Image mDepthImage;
		vk::Format mDepthFormat;
		vk::ImageView mDepthImageView;
		VmaAllocation mDepthAlloc;
    };

    extern ResourceManager g_ResourceManager;
}