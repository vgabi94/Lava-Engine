#pragma once

#include <vulkan\vulkan.hpp>
#include <array>
#include <Engine\GpuArrayBuffer.h>
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

        vk::DescriptorSetLayout GetDescriptorSetLayoutAt(uint32_t index);

		GpuArrayBuffer<LightSource>& GetLightsBuffer() { return mLights; };

    private:
		// Global descriptor set layouts used by renderpasses
        std::array<vk::DescriptorSetLayout, 8> mDescLayouts;

		vk::Image mDepthImage;
		vk::Format mDepthFormat;
		vk::ImageView mDepthImageView;
		VmaAllocation mDepthAlloc;

		GpuArrayBuffer<LightSource> mLights;
    };

    extern ResourceManager g_ResourceManager;
}