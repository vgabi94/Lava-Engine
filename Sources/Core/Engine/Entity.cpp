#include "Entity.h"
#include <Manager\BufferManager.h>
#include <Manager\PipelineManager.h>
#include <Engine\World.h>
#include <Common\PushConstantsStructs.h>

namespace Engine
{
    MEM_POOL_DEFINE(Entity);

    void Entity::Destroy()
    {
        mMesh->Destroy();
        //mMaterial->Destroy();
        LOG_INFO("[LOG] Delete entity {0:#x}\n", (uint64_t)this);
        mAllocator.deleteElement(this);
    }

    void Entity::Draw(vk::CommandBuffer cmdBuff)
    {
        const Pipeline& pipe = PipelineOfType(mMaterial->mPipeType);
        vk::Pipeline pipeline = pipe.mPipeline;
        cmdBuff.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

        DefaultPS pc;
        pc.MVP = mMVP;
        pc.model = mModel;
        pc.eyePos = mWorld->mCameraPos;

        cmdBuff.pushConstants(pipe.mPipelineLayout, vk::ShaderStageFlagBits::eVertex
            | vk::ShaderStageFlagBits::eFragment,
            0, sizeof(DefaultPS), &pc);

        mMaterial->Bind(cmdBuff);
		pipe.BindGlobalDescSets(cmdBuff);

        vk::Buffer vbo = BufferAt(mMesh->mVBO);
        vk::DeviceSize vertOffset = 0;
        vk::DeviceSize indOffset = mMesh->mIBO;

        cmdBuff.bindVertexBuffers(0, 1, &vbo, &vertOffset);
        cmdBuff.bindIndexBuffer(vbo, indOffset, vk::IndexType::eUint32);

        cmdBuff.drawIndexed(mMesh->mCount, 1, 0, 0, 0);
    }
    
    void Entity::OnAddToWorld()
    {
        //mMaterial->UpdateStaticData();
    }
    
    void Entity::OnRemoveFromWorld()
    {
        Destroy();
    }
}

/* EXPORTED INTERFACE */
extern "C"
{
    LAVA_API void* CreateEntity_Native()
    {
        auto ent = Engine::Entity::Allocate();
        LOG_INFO("[LOG] Create entity {0:#x}\n", (uint64_t)ent);
        return ent;
    }

    LAVA_API void AddMaterial_Native(Engine::Entity* ent, Engine::Material* mat)
    {
        ent->mMaterial = mat;
    }

    LAVA_API void AddMesh_Native(Engine::Entity* ent, Engine::StaticMesh* mesh)
    {
        ent->mMesh = mesh;
    }

    LAVA_API void SetMVP_Native(Engine::Entity* ent, Engine::Matrix4 mvp)
    {
        ent->mMVP = mvp;
    }

    LAVA_API void SetModel_Native(Engine::Entity* ent, Engine::Matrix4 model)
    {
        ent->mModel = model;
    }

    LAVA_API void SetPosition_Native(Engine::Entity* ent, Engine::Vector3 pos)
    {
        ent->mPosition = pos;
    }
}