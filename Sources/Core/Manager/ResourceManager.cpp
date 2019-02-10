#include "ResourceManager.h"
#include <Common\Constants.h>

#define GResourceManager Engine::g_ResourceManager

namespace Engine
{
    ResourceManager g_ResourceManager;

    void ResourceManager::Init()
    {
    }

    void ResourceManager::Destroy()
    {
    }

    vk::DescriptorSetLayout ResourceManager::GetDescriptorSetLayoutAt(uint32_t index)
    {
        THROW_IF(index > 7, "Set index must not be greater than 7!");
        THROW_IF(index == 0, "Set index 0 is reserved for materials!");
        return mDescLayouts[index];
    }
}

