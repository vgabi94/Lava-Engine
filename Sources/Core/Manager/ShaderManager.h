#pragma once

#include <vulkan\vulkan.hpp>
#include <unordered_map>
#include <Common\Constants.h>

#define GShaderManager Engine::g_ShaderManager

namespace Engine
{
    struct Shader
    {
        vk::ShaderModule module;
        vk::ShaderStageFlagBits stage;
    };

    class ShaderManager
    {
        friend class PipelineManager;

    public:
        void Init();
        void Destroy();
        void LoadShaderModule(const char * path);

        std::vector<vk::PipelineShaderStageCreateInfo> CreateShaderStages(const std::vector<std::string>& path);

    private:
        std::unordered_map<std::string, Shader> mShaderModule;

        vk::PipelineShaderStageCreateInfo CreateShaderStage(vk::ShaderModule mod, vk::ShaderStageFlagBits stageFlag);
        void DestroyShaderModules();
    };

    extern ShaderManager g_ShaderManager;
    vk::ShaderStageFlagBits GetShaderStageFlag(const std::string &filePath);
}