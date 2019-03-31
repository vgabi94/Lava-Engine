#include "MaterialManager.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include <Engine\Engine.h>
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

#define SET_PBR_UNIFORM(uniform, path) path = j[uniform];\
	if (path.find(".") == std::string::npos) texInd = g_TextureManager.GetColorTexture(path);\
	else {std::string newPath = textureDir + std::string("/") + path; texInd = g_TextureManager.LoadTex2D(newPath.c_str());}\
	mat->UpdateUniform(uniform, texInd)

    Material * MaterialManager::FromJSON(const char * jsonFile)
    {
		constexpr const char* ALBEDO = "albedoMap";
		constexpr const char* NORMAL = "normalMap";
		constexpr const char* AO = "aoMap";
		constexpr const char* METALLIC = "metallicMap";
		constexpr const char* ROUGHNESS = "roughnessMap";

        using json = nlohmann::json;

        std::ifstream fin(jsonFile);
        json j;
        fin >> j;
		const auto& textureDir = g_EngineSettings.TexturesPath;
        
		std::string type = j["type"];
		Material* mat = NewMaterial(type.c_str());

		if (type == "pbr")
		{
			std::string path;
			uint32_t texInd;
			
			SET_PBR_UNIFORM(ALBEDO, path);
			SET_PBR_UNIFORM(NORMAL, path);
			SET_PBR_UNIFORM(AO, path);
			SET_PBR_UNIFORM(METALLIC, path);
			SET_PBR_UNIFORM(ROUGHNESS, path);
		}
		
		return mat;
    }

#undef SET_PBR_UNIFORM
    
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

	LAVA_API Engine::Material* MaterialFromJSON_Native(const char * path)
	{
		return GMaterialManager.FromJSON(path);
	}

    LAVA_API void SetUniformUInt_Native(Engine::Material* mat, uint32_t binding, uint32_t value)
    {
        mat->UpdateUniform(binding, std::make_any<uint32_t>(value));
    }

    LAVA_API void SetUniformFloat_Native(Engine::Material* mat, uint32_t binding, float value)
    {
        mat->UpdateUniform(binding, std::make_any<float>(value));
    }

	LAVA_API void SetUniformNameUInt_Native(Engine::Material* mat, const char* name, uint32_t value)
	{
		mat->UpdateUniform(name, std::make_any<uint32_t>(value));
	}

	LAVA_API void SetUniformNameFloat_Native(Engine::Material* mat, const char* name, float value)
	{
		mat->UpdateUniform(name, std::make_any<float>(value));
	}
}