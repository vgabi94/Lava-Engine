#pragma once
#include <Engine\Material.h>

#define GMaterialManager Engine::g_MaterialManager

namespace Engine
{
    class MaterialManager
    {
        static constexpr uint32_t INIT_CAPACITY = 1024;

    public:
        void Init();
        void Destroy();

        Material* FromJSON(const char* jsonFile);
        Material* NewMaterial(const char* pipelineType);

    private:
        std::vector<Material*> mMaterials;
    };

    extern MaterialManager g_MaterialManager;
}