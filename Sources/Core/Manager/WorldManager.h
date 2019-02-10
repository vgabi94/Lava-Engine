#pragma once

#include <Engine\World.h>

#define GWorldManager Engine::g_WorldManager
#define CurrentWorld g_WorldManager.GetCurrentWorld()
#define g_CurrentWorld Engine::g_WorldManager.GetCurrentWorld()
#define g_CurrentPhysicsWorld Engine::g_WorldManager.GetCurrentWorld()->GetPhysicsWorld()

namespace Engine
{
    class WorldManager
    {
        static constexpr uint32_t INIT_CAPACITY = 2;

    public:
        void Init();
        void Destroy();

        World* NewWorld(bool makeCurrent, bool hasPhysics);

        void UpdatePhysicsWorld();

        World* GetCurrentWorld() { return mCurrentWorld; }
        void SetCurrentWorld(World* world) { mCurrentWorld = world; }

    private:
        std::vector<World*> mWorld;
        World* mCurrentWorld;
    };

    extern WorldManager g_WorldManager;
}