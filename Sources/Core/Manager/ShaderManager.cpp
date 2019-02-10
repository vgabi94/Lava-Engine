#include "ShaderManager.h"
#include <Engine\Device.h>
#include <fstream>

namespace Engine
{
    ShaderManager g_ShaderManager;

    void ShaderManager::Init()
    {
        mShaderModule.reserve(2);
    }

    void ShaderManager::Destroy()
    {
        DestroyShaderModules();
    }

    vk::ShaderStageFlagBits GetShaderStageFlag(const std::string &filePath)
    {
        if (filePath.find(".vert") != std::string::npos)
        {
            return vk::ShaderStageFlagBits::eVertex;
        }
        else if (filePath.find(".frag") != std::string::npos)
        {
            return vk::ShaderStageFlagBits::eFragment;
        }
        else if (filePath.find(".geom") != std::string::npos)
        {
            return vk::ShaderStageFlagBits::eGeometry;
        }
        else if (filePath.find(".tesc") != std::string::npos)
        {
            return vk::ShaderStageFlagBits::eTessellationControl;
        }
        else if (filePath.find(".tese") != std::string::npos)
        {
            return vk::ShaderStageFlagBits::eTessellationEvaluation;
        }
        else if (filePath.find(".comp") != std::string::npos)
        {
            return vk::ShaderStageFlagBits::eCompute;
        }
    }

    void ShaderManager::LoadShaderModule(const char * path)
    {
        std::ifstream in(path, std::ios::binary | std::ios::ate);
        assert(in.is_open());

        size_t byteCount = in.tellg();
        std::vector<char> bytes(byteCount);

        in.seekg(0);
        in.read(bytes.data(), byteCount);
        in.close();

        vk::ShaderModuleCreateInfo info({}, byteCount,
            reinterpret_cast<const uint32_t*>(bytes.data()));
        vk::ShaderModule module = g_vkDevice.createShaderModule(info);
        vk::ShaderStageFlagBits stageFlag;

        std::string filePath(path);
        stageFlag = GetShaderStageFlag(filePath);

        mShaderModule[path] = Shader{ module, stageFlag };
        LOG_INFO("[LOG] Shader module loaded: {0}\n", path);
    }

    std::vector<vk::PipelineShaderStageCreateInfo> ShaderManager::CreateShaderStages(const std::vector<std::string>& pathList)
    {
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        stages.reserve(2);

        for (auto path : pathList)
        {
            auto shaderModule = mShaderModule[path];
            auto shaderStage = CreateShaderStage(shaderModule.module, shaderModule.stage);
            stages.push_back(shaderStage);
        }

        return stages;
    }

    vk::PipelineShaderStageCreateInfo ShaderManager::CreateShaderStage(vk::ShaderModule mod,
        vk::ShaderStageFlagBits stageFlag)
    {
        vk::PipelineShaderStageCreateInfo shaderStage({},
            stageFlag, mod, "main");
        return shaderStage;
    }

    void ShaderManager::DestroyShaderModules()
    {
        for (auto mod : mShaderModule)
        {
            g_vkDevice.destroyShaderModule(mod.second.module);
        }

        mShaderModule.clear();
        LOG_INFO("[LOG] Destroy shader modules\n");
    }

    
}

extern "C"
{
    LAVA_API void LoadShader(const char * path)
    {
        GShaderManager.LoadShaderModule(path);
    }
}
