#pragma once

#include "StaticMesh.h"
#include "Material.h"
#include <Common\MathTypes.h>

namespace Engine
{
    struct Entity
    {
        StaticMesh* mMesh;
        Material* mMaterial;
        Matrix4 mMVP;
        Matrix4 mModel;
        Vector3 mPosition;
        class World* mWorld;

        virtual void Init() { }
        void Destroy();

        virtual void Draw(vk::CommandBuffer cmdBuff);

        void OnAddToWorld();
        void OnRemoveFromWorld();

        MEM_POOL_DECLARE(Entity);
    };
}