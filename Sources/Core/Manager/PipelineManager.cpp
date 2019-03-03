#include "PipelineManager.h"
#include <Engine\Swapchain.h>
//#include <cctype>

namespace Engine
{
    PipelineManager g_PipelineManager;

    void PipelineManager::Init()
    {
        mPipeline.reserve(2);
    }

    void PipelineManager::Destroy()
    {
        DestroyPipelines();
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
    LAVA_API void LoadPipeline(const char * path)
    {
        GPipelineManager.LoadFromJSON(path);
    }

    LAVA_API void LoadBasePipeline(const char * path)
    {
        Engine::Pipeline::BuildBase(path);
    }
}