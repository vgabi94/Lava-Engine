#pragma once

#include <vulkan\vulkan.hpp>
#include <MemoryPool.h>
#include <string>
#include <Common\Constants.h>
#include <any>

namespace Engine
{
    struct Uniform
    {
        vk::DescriptorType mType;
        std::any mValue;
    };

    struct Material
    {
        friend class MaterialManager;

        std::string mPipeType;

        void InitializeUniforms();

        void UpdateUniform(uint32_t binding, const std::any& value);
        void UpdateUniform(const std::string&, const std::any& value);

        void Bind(vk::CommandBuffer cmdBuff);

        MEM_POOL_DECLARE(Material);

    private:
        void WriteDescriptorsIfDirty();

        bool dirty;
        std::vector<Uniform> mUniforms;
        vk::DescriptorSet mDescSet;
    };
}