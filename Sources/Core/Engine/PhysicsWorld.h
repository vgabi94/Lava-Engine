#pragma once

#include <rp3d/reactphysics3d.h>
#include <Common\Constants.h>
#include <Common\MathTypes.h>
#include <MemoryPool.h>
#include <forward_list>

namespace Engine
{
	struct CollisionInfoMarshal
	{
		Vector3 pointOfContact;
	};

	typedef void(*UpdateBodyCBack)(reactphysics3d::Vector3, reactphysics3d::Quaternion);
	typedef void(*CollisionCBack)(CollisionInfoMarshal&);

	/*struct CollisionBodyExt
	{
		reactphysics3d::CollisionBody* body;
		CollisionCBack collisionCallback;
	};*/

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
            UpdateBodyCBack callback);

		rp3d::CollisionBody* PhysicsWorld::CreateCollisionBody(const rp3d::Transform& transform,
			UpdateBodyCBack cbCallback);

        void SetGravity(reactphysics3d::Vector3 g) { mDynamics.setGravity(g); }

		void SetProxyCallback(rp3d::ProxyShape* ps, CollisionCBack cb)
		{
			mProxyCallback[ps] = cb;
		}

        UpdateCback PhysicsUpdateCallback;

        static MemoryPool<reactphysics3d::BoxShape> mBoxAllocator;
        static MemoryPool<reactphysics3d::SphereShape> mSphereAllocator;
        static MemoryPool<reactphysics3d::CapsuleShape> mCapsuleAllocator;
        //static MemoryPool<CollisionBodyExt> mCBAllocator;
    private:
        MEM_POOL_DECLARE_SIZE(PhysicsWorld, 8192);

        reactphysics3d::DynamicsWorld mDynamics;
        reactphysics3d::CollisionWorld mCollision;
        float mAccumulator;

        struct RbState
        {
            reactphysics3d::RigidBody* rb;
            UpdateBodyCBack updateRigidBody;
        };
        std::vector<RbState> mRbState;

		struct CbState
		{
			rp3d::CollisionBody* cb;
			UpdateBodyCBack updateCollisionBody;
		};
		std::vector<CbState> mCbState;

		std::unordered_map<rp3d::ProxyShape*, CollisionCBack> mProxyCallback;

        void Update();
    };
}