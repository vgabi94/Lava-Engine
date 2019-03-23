#pragma once

#include "Entity.h"
#include "PhysicsWorld.h"
#include <Common\LightInfo.h>
#include "buffers.h"

#define WORLD_DIRTY 0x3
#define WORLD_CLEAN 0x0

namespace otr
{
    template<typename T = void> class Octree;
}

namespace Engine
{
	struct SkySettings
	{
		Vector3 color;
		unsigned int hdrTex;
		unsigned int hdrEnv;
		bool useTex;
		float exposure;
		float gamma;

		void GetClearColor(float& r, float& g, float& b) const
		{
			r = color.x;
			g = color.y;
			b = color.z;
		}
	};

    // Represents the world of drawable entities
    class World
    {
        friend class WorldManager;
        static constexpr uint32_t INIT_CAPACITY = 8;

    public:
        void Init(bool hasPhysics);
        void Destroy();
        
        void AddEntity(Entity* ent);
        // TODO: RemoveEntity

        void BuildVisibles();
        void RecordWorldCommandBuffers(uint32_t imageIndex);
        void FreeWorldCommandBuffers();
        void CreateWorldCommandBuffers();

        PhysicsWorld* GetPhysicsWorld() const { return mPhysicsWorld; }

		void UploadLightSources() const;

        vk::CommandBuffer GetWorldCommandBuffer(uint32_t index) const 
        { return mCommandBuffer[index]; }

        uint8_t mDirty;
        Vector3 mCameraPos;
        Vector3 mViewBoundingMin;
        Vector3 mViewBoundingMax;
		SkySettings mSkySettings;
		Matrix4 mSkyViewProj;
		std::vector<LightInfo> mLightInfo;
    private:
        MEM_POOL_DECLARE(World);
        void DestroyEntities();

        std::vector<vk::CommandBuffer> mCommandBuffer;
        std::vector<Entity*> mEntityList;
        otr::Octree<Entity>* mVisibleEntities;
        vk::CommandPool mCommandPool;

        PhysicsWorld* mPhysicsWorld;

        void UpdatePhysicsWorld();
    };
}