#include "World.h"
#include "Device.h"
#include <Manager\ShaderManager.h>
#include "Swapchain.h"
#include "Engine.h"
#include <Manager\BufferManager.h>
#include <Manager\WorldManager.h>
#include <Manager\ResourceManager.h>
#define OCTREE_IMPL
#include <Octree.h>

namespace Engine
{
    MEM_POOL_DEFINE(World);

    void World::Init(bool hasPhysics)
    {
        LOG_INFO("[LOG] Create world {0:#x}\n", (uint64_t)this);
        mEntityList.reserve(INIT_CAPACITY);
		mIBLProbes.reserve(1);
        mDirty = WORLD_CLEAN;
        mPhysicsWorld = nullptr;
        
        mVisibleEntities = nullptr;
        BuildVisibles();

        if (hasPhysics)
        {
            mPhysicsWorld = PhysicsWorld::Allocate();
            mPhysicsWorld->Init();
        }

        vk::CommandPoolCreateInfo commandPoolCreateInfo(
            vk::CommandPoolCreateFlagBits::eTransient |
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            GRAPHICS_FAMILY_INDEX);
        mCommandPool = g_vkDevice.createCommandPool(commandPoolCreateInfo);

        CreateWorldCommandBuffers();
    }

    void World::Destroy()
    {
        LOG_INFO("[LOG] Destroy world {0:#x}\n", (uint64_t)this);
        g_vkDevice.destroyCommandPool(mCommandPool);
        DestroyEntities();
        mPhysicsWorld->Destroy();
        mAllocator.deleteElement(this);
    }

    void World::FreeWorldCommandBuffers()
    {
        g_vkDevice.freeCommandBuffers(mCommandPool, mCommandBuffer);
    }

    void World::CreateWorldCommandBuffers()
    {
        // World should be split and cmdbuffer registered by multiple threads
        // I should add at least one secondary buffer for objects created at runtime
        // One world for static objects, and one for animated objects but not created at runtime
        vk::CommandBufferAllocateInfo cmdBufferAllocInfo(
            mCommandPool,
            vk::CommandBufferLevel::eSecondary,
            GSwapchain.GetImageCount());

        mCommandBuffer = g_vkDevice.allocateCommandBuffers(cmdBufferAllocInfo);
    }

	void World::UploadLightSources() const
	{
		auto& lightsBuffer = g_ResourceManager.GetLightsBuffer();
		
		if (lightsBuffer.Size() != mLightInfo.size())
		{
			lightsBuffer.Clear();
			lightsBuffer.Reserve(mLightInfo.size());
			
			for (auto& li : mLightInfo)
			{
				LightSource ls;
				ls.direction = Vector4(li.direction, 0.0f);
				ls.position = Vector4(li.position, 1.0f);
				ls.color = li.color;
				ls.type.x = static_cast<int>(li.type);
				lightsBuffer.Add(ls);
			}
		}
		else
		{
			for (size_t i = 0; i < lightsBuffer.Size(); i++)
			{
				LightSource& ls = lightsBuffer[i];
				const LightInfo& li = mLightInfo[i];
				ls.direction = Vector4(li.direction, 0.0f);
				ls.position = Vector4(li.position, 1.0f);
				ls.color = li.color;
				ls.type.x = static_cast<int>(li.type);
			}
		}

		lightsBuffer.Commit();
	}

	void World::UploadFrameConsts() const
	{
		auto& frameConsts = g_ResourceManager.GetFrameConstsBuffer();
		frameConsts.Get().ambientLight = mSkySettings.ambient;
		frameConsts.Get().numLights = mLightInfo.size();
		frameConsts.Get().exposure = mSkySettings.exposure;
		frameConsts.Get().gamma = mSkySettings.gamma;
		frameConsts.Commit();
	}

	void World::AddIBLProbeInfo(const IBLProbeInfo & info)
	{
		IBLProbe probe;
		probe.mPosition = info.position;
		probe.mResIndex = g_ResourceManager.AddIBLProbeInfo(info);
		mIBLProbes.push_back(probe);
	}
    
    void World::AddEntity(Entity * ent)
    {
        mEntityList.push_back(ent);
        ent->OnAddToWorld();
        ent->mWorld = this;
        otr::Vector3 position(ent->mPosition.x, ent->mPosition.y, ent->mPosition.z);
        mVisibleEntities->Insert(otr::OctreeData<Entity>::New(position, ent));
        mDirty = WORLD_DIRTY;
    }
    
    void World::BuildVisibles()
    {
        if (mVisibleEntities)
        {
            mVisibleEntities->Delete();
        }
        otr::Vector3 origin(0.f);
        otr::Vector3 extent(1e6f);
        mVisibleEntities = otr::Octree<Entity>::New(origin, extent);
        assert(mVisibleEntities != nullptr);
    }

    void World::RecordWorldCommandBuffers(uint32_t imageIndex)
    {
        //if ((mDirty & (1 << imageIndex)) == 0) return;

        vk::CommandBufferInheritanceInfo inheritanceInfo(
            GSwapchain.GetFramePass()->GetVkObject(),
            0,
            GSwapchain.GetFramePass()->FramebufferAt(imageIndex)
        );

        // Record the buffer
        vk::CommandBufferBeginInfo beginInfo(
            vk::CommandBufferUsageFlagBits::eSimultaneousUse |
            vk::CommandBufferUsageFlagBits::eRenderPassContinue,
            &inheritanceInfo);
        mCommandBuffer[imageIndex].begin(beginInfo);

        vk::Viewport viewport(0.f, 0.f, (float)GWINDOW_WIDTH, (float)GWINDOW_HEIGHT, 0.f, 1.f);
        mCommandBuffer[imageIndex].setViewport(0, { viewport });

        vk::Rect2D scissor({}, { GWINDOW_WIDTH, GWINDOW_HEIGHT });
        mCommandBuffer[imageIndex].setScissor(0, { scissor });

        /*for (auto ent : mEntityList)
        {
            ent->Draw(mCommandBuffer[imageIndex]);
        }*/

        std::vector<otr::OctreeData<Entity>*> entities;
        otr::Vector3 bmin(mViewBoundingMin.x, mViewBoundingMin.y, mViewBoundingMin.z);
        otr::Vector3 bmax(mViewBoundingMax.x, mViewBoundingMax.y, mViewBoundingMax.z);
        mVisibleEntities->GetPointsInsideBox(bmin, bmax, entities);

        for (auto ent : entities)
        {
            ent->mData->Draw(mCommandBuffer[imageIndex]);
        }

        mCommandBuffer[imageIndex].end();

        //mDirty &= ~(1 << imageIndex);
    }
    
    void World::DestroyEntities()
    {
        for (auto ent : mEntityList)
        {
            ent->Destroy();
        }
    }

    void World::UpdatePhysicsWorld()
    {
        if (mPhysicsWorld)
        {
            mPhysicsWorld->Update();
        }
    }
}

/* EXPORTED INTERFACE */
extern "C"
{
    LAVA_API void AddEntity_Native(Engine::World* world, Engine::Entity* ent)
    {
        world->AddEntity(ent);
    }

	LAVA_API void AddLightInfo_Native(Engine::World* world, Engine::LightInfo li)
	{
		world->mLightInfo.push_back(li);
	}

	LAVA_API void AddIBLProbeInfo_Native(Engine::World* world, Engine::IBLProbeInfo info)
	{
		world->AddIBLProbeInfo(info);
	}

    LAVA_API void SetCameraPos_Native(Engine::World* world, Engine::Vector3 camPos)
    {
        world->mCameraPos = camPos;
    }

	LAVA_API void SetSkySettings_Native(Engine::World* world, Engine::SkySettings skySettings)
	{
		world->mSkySettings = skySettings;
	}

	LAVA_API void SetSkyViewProj_Native(Engine::World* world, Engine::Matrix4 viewProj)
	{
		world->mSkyViewProj = viewProj;
	}

    LAVA_API void SetViewBounds_Native(Engine::World* world, Engine::Vector3 bmin, Engine::Vector3 bmax)
    {
        world->mViewBoundingMin = bmin;
        world->mViewBoundingMax = bmax;
    }

    LAVA_API void* GetPhysicsWorld_Native(Engine::World* world)
    {
        return world->GetPhysicsWorld();
    }
}
