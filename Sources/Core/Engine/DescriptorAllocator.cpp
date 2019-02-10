#include "DescriptorAllocator.h"
#include <Common\Constants.h>

namespace Engine
{
    void DescriptorAllocator::Init(const std::vector<vk::DescriptorPoolSize>& poolSizes
        , vk::DescriptorSetLayout descriptorSetLayout)
    {
        mDescriptorPool.reserve(INIT_NUM_POOLS);
        mPoolIndex = 0;
        mPoolSizes = poolSizes;
        mDescriptorSetLayout = descriptorSetLayout;
        AllocateDescriptorPool();
    }
    
    void DescriptorAllocator::Destroy()
    {
        for (size_t i = 0; i < mDescriptorPool.size(); i++)
        {
            g_vkDevice.destroyDescriptorPool(mDescriptorPool[i]);
        }
    }

    vk::DescriptorSet DescriptorAllocator::AllocateDescriptorSet()
    {
        if (!mFreeDescriptors.empty())
        {
            vk::DescriptorSet descSet = *mFreeDescriptors.cbegin();
            mFreeDescriptors.erase(mFreeDescriptors.cbegin());
            mUsedDescriptors.insert(descSet);
            LOG_INFO("Descriptor set reallocation successful!");
            return descSet;
        }
        else
        {
            vk::DescriptorSetAllocateInfo descSetAI(mDescriptorPool[mPoolIndex], 1, &mDescriptorSetLayout);
            vk::DescriptorSet descSet;

            try
            {
                descSet = g_vkDevice.allocateDescriptorSets(descSetAI)[0];
                LOG_INFO("Descriptor set allocation successful!");
            }
            catch (vk::OutOfPoolMemoryError&)
            {
                LOG_ERROR("Descriptor pool allocation failed!");
                mPoolIndex = mDescriptorPool.size();
                AllocateDescriptorPool();
                descSet = AllocateDescriptorSet();
            }

            return descSet;
        }
    }

    void DescriptorAllocator::ReleaseDescriptorSet(vk::DescriptorSet descSet)
    {
        if (mUsedDescriptors.find(descSet) == mUsedDescriptors.end())
            THROW("This descriptor set was not allocated from this pool!");

        mUsedDescriptors.erase(descSet);
        mFreeDescriptors.insert(descSet);
    }

    /*std::vector<vk::DescriptorSet> DescriptorAllocator::AllocateDescriptorSets(vk::DescriptorSetLayout descriptorSetLayout, uint32_t count)
    {
        return std::vector<vk::DescriptorSet>();
    }*/
    
    void DescriptorAllocator::AllocateDescriptorPool()
    {
        vk::DescriptorPoolCreateInfo descriptorPoolCI({},
            MAX_NUM_SETS,
            static_cast<uint32_t>(mPoolSizes.size()),
            mPoolSizes.data());
        mDescriptorPool.push_back(g_vkDevice.createDescriptorPool(descriptorPoolCI));
        LOG_INFO("Descriptor pool allocation successful!");
    }
}

