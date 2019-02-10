#pragma once

#include <vulkan\vulkan.hpp>
#include <unordered_map>
#include <string>
#include <Engine\Pipeline.h>

#define GPipelineManager Engine::g_PipelineManager
#define PipelineOfType(type) g_PipelineManager.GetPipeline(type)

namespace Engine
{
    class PipelineManager
    {
    public:
        void Init();
        void Destroy();

        void InflatePipelineFromJSON(const char * json);
        void LoadFromJSON(const char * jsonFile);
        Pipeline& GetPipeline(const std::string& type)
        {
            THROW_IF(mPipeline.find(type) == mPipeline.end(), "Invalid pipeline type!");
            return mPipeline.at(type);
        }

    private:
        std::unordered_map<std::string, Pipeline> mPipeline;

        void DestroyPipelines();
    };

    extern PipelineManager g_PipelineManager;
}