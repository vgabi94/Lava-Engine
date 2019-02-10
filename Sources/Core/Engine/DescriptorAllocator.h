#pragma once

#include "Device.h"
#include <vector>
#include <unordered_set>

namespace Engine
{
    class DescriptorAllocator
    {
        friend struct Pipeline;

    public:
        static constexpr uint32_t INIT_NUM_POOLS = 4;
        // Maximum number of sets that can be allocated from the pool
        static constexpr uint32_t MAX_NUM_SETS = 1 << 14;
        // Maximum number of each type of descriptor that can be allocated from the pool
        // 12 descriptors per set
        //static constexpr uint32_t MAX_NUM_DESCRIPTOR = MAX_NUM_SETS * 12;

        void Init(const std::vector<vk::DescriptorPoolSize>& poolSizes, vk::DescriptorSetLayout descriptorSetLayout);
        void Destroy();

        vk::DescriptorSet AllocateDescriptorSet();
        void ReleaseDescriptorSet(vk::DescriptorSet descSet);
        //std::vector<vk::DescriptorSet> AllocateDescriptorSets(vk::DescriptorSetLayout descriptorSetLayout, uint32_t count);

    private:
        void AllocateDescriptorPool();

        std::vector<vk::DescriptorPool> mDescriptorPool;
        std::vector<vk::DescriptorPoolSize> mPoolSizes;
        vk::DescriptorSetLayout mDescriptorSetLayout;
        std::unordered_set<VkDescriptorSet> mFreeDescriptors;
        std::unordered_set<VkDescriptorSet> mUsedDescriptors;
        uint32_t mPoolIndex;
    };
}