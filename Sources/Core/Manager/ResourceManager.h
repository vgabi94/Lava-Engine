#pragma once

#include <vulkan\vulkan.hpp>
#include <array>
#include <Engine\GpuArrayBuffer.h>
#include <Engine\GpuBuffer.h>
#include <Engine\DescriptorAllocator.h>
#include <Common\WorldStructs.h>
#include <Engine\Texture.h>
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

        vk::DescriptorSet GetDescriptorSet(uint32_t slot) const;
		vk::DescriptorSetLayout GetDescriptorSetLayoutAt(uint32_t slot) const;
		
		template<typename T>
		void WriteBufferToDescriptorSlot(uint32_t slot, const T& buffer)
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
			vk::DescriptorBufferInfo bufferInfo(buffer.GetBuffer(), 0, VK_WHOLE_SIZE);
			writeDescSet.pBufferInfo = &bufferInfo;
			g_vkDevice.updateDescriptorSets({ writeDescSet }, { });
		}

		GpuArrayBuffer<LightSource>& GetLightsBuffer() { return mLights; };
		GpuBuffer<FrameConsts>& GetFrameConstsBuffer() { return mFrameConsts; }

		uint32_t AddIBLProbeInfo(const IBLProbeInfo& probe);
		void ExecuteIBLPasses();

		//const Texture& GetIrradMap(uint32_t ind) {  } TODO
		const Texture& GetPrefEnvMap(uint32_t ind);
		//const Texture& GetBrdfMap(uint32_t ind) {  } TODO

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
		GpuBuffer<FrameConsts> mFrameConsts;

		//std::vector<Texture> mIrradPasses; TODO
		std::vector<uint32_t> mPrefEnvPasses;
		//std::vector<Texture> mBrdfMaps; TODO
		bool mIBLdone;
    };

    extern ResourceManager g_ResourceManager;
}