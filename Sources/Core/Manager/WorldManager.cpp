#include "WorldManager.h"

namespace Engine
{
    WorldManager g_WorldManager;
    
    void WorldManager::Init()
    {
        mWorld.reserve(INIT_CAPACITY);
        mCurrentWorld = nullptr;
        LOG_INFO("[LOG] WorldManager Init\n");
    }

    void WorldManager::Destroy()
    {
        for (auto world : mWorld)
        {
            world->Destroy();
        }

        LOG_INFO("[LOG] WorldManager Destroy\n");
    }
    
    World* WorldManager::NewWorld(bool makeCurrent, bool hasPhysics)
    {
        World* world = World::Allocate();
        if (makeCurrent)
            mCurrentWorld = world;

        mWorld.push_back(world);

        world->Init(hasPhysics);

        return world;
    }

    void WorldManager::UpdatePhysicsWorld()
    {
        if (mCurrentWorld)
        {
            mCurrentWorld->UpdatePhysicsWorld();
        }
    }
}

/* EXPORTED INTERFACE */
extern "C"
{
    LAVA_API void* CreateWorld_Native(bool makeCurrent, bool hasPhysics)
    {
        return Engine::g_WorldManager.NewWorld(makeCurrent, hasPhysics);
    }

    LAVA_API void* GetCurrentWorld_Native()
    {
        return Engine::g_WorldManager.GetCurrentWorld();
    }

    LAVA_API void SetCurrentWorld_Native(Engine::World* world)
    {
        Engine::g_WorldManager.SetCurrentWorld(world);
    }
}