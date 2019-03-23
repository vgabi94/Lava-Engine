#pragma once

#include <vulkan\vulkan.hpp>
#include <array>
#include <Engine\GpuArrayBuffer.h>
#include <Engine\DescriptorAllocator.h>
#include <buffers.h>

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

        //vk::DescriptorSet GetDescriptorSet(uint32_t slot) const;
		vk::DescriptorSetLayout GetDescriptorSetLayoutAt(uint32_t slot) const;
		void WriteDescriptorAtSlot(uint32_t slot);

		GpuArrayBuffer<LightSource>& GetLightsBuffer() { return mLights; };

    private:
		void InitDescriptorAllocatorsAndSets();
		void DestroyDescriptorAllocators();

		// Global descriptor sets used by various renderpasses
		static constexpr uint32_t DESC_SET_SIZE = 8;
        std::array<vk::DescriptorSet, DESC_SET_SIZE> mDescSets;
		std::array<vk::DescriptorSetLayout, DESC_SET_SIZE> mDescLayout;
		std::array<DescriptorAllocator, DESC_SET_SIZE> mDescAllocators;

		vk::Image mDepthImage;
		vk::Format mDepthFormat;
		vk::ImageView mDepthImageView;
		VmaAllocation mDepthAlloc;

		GpuArrayBuffer<LightSource> mLights;
    };

    extern ResourceManager g_ResourceManager;
}