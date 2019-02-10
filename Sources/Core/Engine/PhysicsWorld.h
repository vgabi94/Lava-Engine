#pragma once

#include <rp3d/reactphysics3d.h>
#include <Common\Constants.h>
#include <MemoryPool.h>
#include <forward_list>

namespace Engine
{
    typedef void(*UpdateRigidBodyCBack)(reactphysics3d::Vector3, reactphysics3d::Quaternion);

    class PhysicsWorld
    {
        friend class World;
        static constexpr size_t NUM_BODIES = 4;
    public:
        void Init();
        void Destroy();

        PhysicsWorld() : mDynamics(reactphysics3d::Vector3(0.0f, -9.81f, 0.0f)),
            PhysicsUpdateCallback(nullptr),
            mAccumulator(0) { }

        reactphysics3d::RigidBody* CreateRigidBody(const reactphysics3d::Transform& transform,
            UpdateRigidBodyCBack callback);

        void SetGravity(reactphysics3d::Vector3 g) { mDynamics.setGravity(g); }

        UpdateCback PhysicsUpdateCallback;

        static MemoryPool<reactphysics3d::BoxShape> mBoxAllocator;
        static MemoryPool<reactphysics3d::SphereShape> mSphereAllocator;
        static MemoryPool<reactphysics3d::CapsuleShape> mCapsuleAllocator;
    private:
        MEM_POOL_DECLARE_SIZE(PhysicsWorld, 8192);

        reactphysics3d::DynamicsWorld mDynamics;
        float mAccumulator;

        struct State
        {
            reactphysics3d::RigidBody* rb;
            UpdateRigidBodyCBack updateBody;
        };
        std::forward_list<State> mState;

        void Update();
    };
}