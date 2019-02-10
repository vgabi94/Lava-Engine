#pragma once

#include <vulkan\vulkan.hpp>
#include <array>

namespace Engine
{
    class ResourceManager
    {
    public:
        void Init();
        void Destroy();

        vk::DescriptorSetLayout GetDescriptorSetLayoutAt(uint32_t index);

    private:
        std::array<vk::DescriptorSetLayout, 8> mDescLayouts;
    };

    extern ResourceManager g_ResourceManager;
}