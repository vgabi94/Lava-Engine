#include "PipelineManager.h"
#include <Engine\Swapchain.h>
//#include <cctype>

namespace Engine
{
    PipelineManager g_PipelineManager;

    /*PipelineType PipelineTypeFromString(std::string ptype)
    {
        std::transform(ptype.begin(), ptype.end(), ptype.begin(), std::tolower);
        if (ptype == "default")
            return PipelineType::Default;
        if (ptype == "albedo")
            return PipelineType::Albedo;
        if (ptype == "phong")
            return PipelineType::Phong;
        return PipelineType::Unknown;
    }*/

    void PipelineManager::Init()
    {
        mPipeline.reserve(2);
    }

    void PipelineManager::Destroy()
    {
        DestroyPipelines();
    }

    void PipelineManager::InflatePipelineFromJSON(const char * jsonFile)
    {
        /*using json = nlohmann::json;

        std::ifstream fin(jsonFile);
        json j;
        fin >> j;

        if (j.find("type") == j.end())
        {
            LOG_ERROR("Material doesn't have a 'type': {0}", jsonFile);
            return;
        }

        std::string ptype = j["type"];
        PipelineType pipelineType = PipelineTypeFromString(ptype);

        if (pipelineType == PipelineType::Unknown)
        {
            LOG_ERROR("Unknown material type: {0}", jsonFile);
            return;
        }

        if (j.find("shaders") == j.end())
        {
            LOG_ERROR("Material doesn't have 'shaders' specified: {0}", jsonFile);
            return;
        }

        std::vector<std::string> shaderPath = j["shaders"];
        auto shaderStages = g_ShaderManager.CreateShaderStages(shaderPath);

        mPipeline[pipelineType] = Pipeline::Make(pipelineType, shaderStages,
            GSwapchain.GetFramePass()->GetVkObject());

        LOG_INFO("[LOG] Create pipeline from material: {0}\n", jsonFile);*/
    }

    static std::string GetPipelineName(const std::string& filename)
    {
        size_t sz = filename.find('.');
        size_t lastSlash = filename.rfind("\\");
        size_t count = sz - lastSlash - 1;
        return filename.substr(lastSlash + 1, count);
    }

    void PipelineManager::LoadFromJSON(const char * jsonFile)
    {
        std::string pipelineType = GetPipelineName(jsonFile);
        mPipeline[pipelineType] = Pipeline::FromJSON(jsonFile);

        LOG_INFO("[LOG] Create pipeline from json: {0}\n", jsonFile);
    }

    void PipelineManager::DestroyPipelines()
    {
        for (auto pipe : mPipeline)
        {
            pipe.second.Destroy();
        }

        LOG_INFO("[LOG] Pipelines destroyed\n");
    }
}

extern "C"
{
    // TODO DELETE
    LAVA_API void LoadPipelineFromMaterial(const char * path)
    {
        GPipelineManager.InflatePipelineFromJSON(path);
    }

    LAVA_API void LoadPipeline(const char * path)
    {
        GPipelineManager.LoadFromJSON(path);
    }

    LAVA_API void LoadBasePipeline(const char * path)
    {
        Engine::Pipeline::BuildBase(path);
    }
}