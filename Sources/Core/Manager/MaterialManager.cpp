#include "MaterialManager.h"
#include "PipelineManager.h"
#include <json.hpp>
#include <fstream>

namespace Engine
{
    MaterialManager g_MaterialManager;

    void MaterialManager::Init()
    {
        mMaterials.reserve(INIT_CAPACITY);
    }

    void MaterialManager::Destroy()
    {
        for (auto mat : mMaterials)
        {
            mat->mAllocator.deleteElement(mat);
        }
    }

    Material * MaterialManager::FromJSON(const char * jsonFile)
    {
        using json = nlohmann::json;

        std::ifstream fin(jsonFile);
        json j;
        fin >> j;

        // TODO
        return nullptr;
    }
    
    Material * MaterialManager::NewMaterial(const char * pipelineType)
    {
        // Check if pipeline is ok
        PipelineOfType(pipelineType);

        Material* mat = Material::mAllocator.newElement();
        mat->mPipeType = pipelineType;
        mat->InitializeUniforms();
        return mat;
    }
}

extern "C"
{
    LAVA_API Engine::Material* NewMaterial_Native(const char * pipelineType)
    {
        return GMaterialManager.NewMaterial(pipelineType);
    }

    LAVA_API void SetUniformUInt_Native(Engine::Material* mat, uint32_t binding, uint32_t value)
    {
        mat->UpdateUniform(binding, std::make_any<uint32_t>(value));
    }

    LAVA_API void SetUniformFloat_Native(Engine::Material* mat, uint32_t binding, float value)
    {
        mat->UpdateUniform(binding, std::make_any<float>(value));
    }
}