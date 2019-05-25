#include "PhysicsWorld.h"
#include "Time.h"
#include <Common\MathTypes.h>
using namespace reactphysics3d;

namespace Engine
{
    MEM_POOL_DEFINE_SIZE(PhysicsWorld, 8192);

    MemoryPool<reactphysics3d::BoxShape> PhysicsWorld::mBoxAllocator;
    MemoryPool<reactphysics3d::SphereShape> PhysicsWorld::mSphereAllocator;
    MemoryPool<reactphysics3d::CapsuleShape> PhysicsWorld::mCapsuleAllocator;

    void PhysicsWorld::Init()
    {
        LOG_INFO("[LOG] Create physics world {0:#x}\n", (uint64_t)this);
        //mState.reserve(NUM_BODIES);
    }

    void PhysicsWorld::Destroy()
    {
        LOG_INFO("[LOG] Destroy physics world {0:#x}\n", (uint64_t)this);
        mAllocator.deleteElement(this);
    }

    rp3d::RigidBody * PhysicsWorld::CreateRigidBody(const rp3d::Transform & transform,
        UpdateBodyCBack rbCallback)
    {
        auto rb = mDynamics.createRigidBody(transform);
		rb->setAngularDamping(0.05f);
		rb->setLinearDamping(0.0f);

        mRbState.push_front(RbState{ rb, rbCallback });
        return rb;
    }

	rp3d::CollisionBody* PhysicsWorld::CreateCollisionBody(const rp3d::Transform& transform,
		UpdateBodyCBack cbCallback)
	{
		auto cb = mCollision.createCollisionBody(transform);
		mCbState.push_front(CbState{ CollisionBody{ cb, nullptr }, cbCallback });
		return cb;
	}

    void PhysicsWorld::Update()
    {   
        assert(PhysicsUpdateCallback);

        // Add the time difference in the accumulator 
        mAccumulator += g_Time.deltaTime;
        bool sim = false;

        // While there is enough accumulated time to take 
        // one or several physics steps 
        while (mAccumulator >= TIME_STEP)
        {
            PhysicsUpdateCallback();

            // Update the Dynamics world with a constant time step 
            mDynamics.update(TIME_STEP);

            // Decrease the accumulated time 
            mAccumulator -= TIME_STEP;
            sim = true;
        }

        if (sim)
        {
            // Advance the state with the remaining accumulator
            // so that we don't lag behind
            // I use this method instead of interpolation because the
            // interp method doesn't work for some reason
            //mDynamics.update(mAccumulator);

            // Set this to 0 because we simulated all of the deltaTime
            //mAccumulator = 0;

            for (auto state : mRbState)
            {
                auto trans = state.rb->getTransform();
                state.updateRigidBody(trans.getPosition(), trans.getOrientation());
            }
        }
    }
}

/* EXPORTED INTERFACE */
extern "C"
{
	LAVA_API void SetPhysicsCallback_Native(Engine::PhysicsWorld* pworld,
		Engine::UpdateCback callback)
	{
		pworld->PhysicsUpdateCallback = callback;
	}

	LAVA_API void SetGravity_Native(Engine::PhysicsWorld* pworld, rp3d::Vector3 gravity)
	{
		pworld->SetGravity(gravity);
	}

	// -------- CollisionBody -------- //
	LAVA_API rp3d::CollisionBody* CreateCollisionBody_Native(Engine::PhysicsWorld* pworld,
		rp3d::Vector3 pos,
		rp3d::Quaternion quat,
		Engine::UpdateBodyCBack callback)
	{
		rp3d::Transform trans(pos, quat);
		return pworld->CreateCollisionBody(trans, callback);
	}

	// -------- RigidBody -------- //
	LAVA_API rp3d::RigidBody* CreateRigidBody_Native(Engine::PhysicsWorld* pworld,
		rp3d::Vector3 pos,
		rp3d::Quaternion quat,
		Engine::UpdateBodyCBack callback)
	{
		rp3d::Transform trans(pos, quat);
		return pworld->CreateRigidBody(trans, callback);
	}

	LAVA_API void SetType_Native(rp3d::RigidBody* rb, rp3d::BodyType type)
	{
		rb->setType(type);
	}

	LAVA_API void SetPhysicsMaterial_Native(rp3d::RigidBody* rb, rp3d::Material material)
	{
		rb->setMaterial(material);
	}

	LAVA_API void SetMass_Native(rp3d::RigidBody* rb, float mass)
	{
		rb->setMass(mass);
	}

	LAVA_API float GetMass_Native(rp3d::RigidBody* rb)
	{
		return rb->getMass();
	}

	LAVA_API void EnableGravity_Native(rp3d::RigidBody* rb, bool flag)
	{
		rb->enableGravity(flag);
	}

	LAVA_API void SetTransform_Native(rp3d::RigidBody* rb, rp3d::Vector3 pos, rp3d::Quaternion rot)
	{
		rp3d::Transform trans(pos, rot);
		rb->setTransform(trans);
	}

	LAVA_API void ApplyForce_Native(rp3d::RigidBody* rb, rp3d::Vector3 force, rp3d::Vector3 point)
	{
		rb->applyForce(force, point);
	}

	LAVA_API void ApplyForceToCenterOfMass_Native(rp3d::RigidBody* rb, rp3d::Vector3 force)
	{
		rb->applyForceToCenterOfMass(force);
	}

	LAVA_API void ApplyTorque_Native(rp3d::RigidBody* rb, rp3d::Vector3 torque)
	{
		rb->applyTorque(torque);
	}

	LAVA_API void SetLinearDamping_Native(rp3d::RigidBody* rb, float damping)
	{
		rb->setLinearDamping(damping);
	}

	LAVA_API void SetAngularDamping_Native(rp3d::RigidBody* rb, float damping)
	{
		rb->setAngularDamping(damping);
	}

	// -------- Collision Shapes -------- //
	LAVA_API void SetShapeTransform_Native(rp3d::ProxyShape* proxy, rp3d::Vector3 pos, rp3d::Quaternion rot)
	{
		rp3d::Transform trans(pos, rot);
		proxy->setLocalToBodyTransform(trans);
	}

	// -------- Box Shape -------- //
	LAVA_API rp3d::ProxyShape* CreateBoxShape_Native(rp3d::RigidBody* rb, rp3d::Vector3 halfExtents,
		rp3d::Vector3 pos, rp3d::Quaternion rot, float mass)
	{
		rp3d::Transform trans(pos, rot);
		rp3d::BoxShape* shape = Engine::PhysicsWorld::mBoxAllocator.newElement(halfExtents);
		auto proxy = rb->addCollisionShape(shape, trans, mass);
		proxy->setUserData(shape);
		return proxy;
	}

	LAVA_API rp3d::ProxyShape* CreateBoxTrigger_Native(rp3d::CollisionBody* cb, rp3d::Vector3 halfExtents,
		rp3d::Vector3 pos, rp3d::Quaternion rot)
	{
		rp3d::Transform trans(pos, rot);
		rp3d::BoxShape* shape = Engine::PhysicsWorld::mBoxAllocator.newElement(halfExtents);
		auto proxy = cb->addCollisionShape(shape, trans);
		proxy->setUserData(shape);
		return proxy;
	}

	LAVA_API void SetBoxTriggerCallback_Native(rp3d::ProxyShape* proxy, Engine::CollisionCBack cback)
	{
		// TODO
	}

    LAVA_API void DestroyBoxShape_Native(rp3d::RigidBody* rb, rp3d::ProxyShape* proxy)
    {
        Engine::PhysicsWorld::mBoxAllocator.deleteElement(
            static_cast<rp3d::BoxShape*>(proxy->getUserData())
        );
        rb->removeCollisionShape(proxy);
    }

	LAVA_API void DestroyBoxTrigger_Native(rp3d::CollisionBody* cb, rp3d::ProxyShape* proxy)
	{
		Engine::PhysicsWorld::mBoxAllocator.deleteElement(
			static_cast<rp3d::BoxShape*>(proxy->getUserData())
		);
		cb->removeCollisionShape(proxy);
	}

    // -------- Sphere Shape -------- //
    LAVA_API rp3d::ProxyShape* CreateSphereShape_Native(rp3d::RigidBody* rb, float radius,
        rp3d::Vector3 pos, rp3d::Quaternion rot, float mass)
    {
        rp3d::Transform trans(pos, rot);
        rp3d::SphereShape* shape = Engine::PhysicsWorld::mSphereAllocator.newElement(radius);
        auto proxy = rb->addCollisionShape(shape, trans, mass);
        proxy->setUserData(shape);
        return proxy;
    }

    LAVA_API void DestroySphereShape_Native(rp3d::RigidBody* rb, rp3d::ProxyShape* proxy)
    {
        Engine::PhysicsWorld::mSphereAllocator.deleteElement(
            static_cast<rp3d::SphereShape*>(proxy->getUserData())
        );
        rb->removeCollisionShape(proxy);
    }

    // -------- Capsule Shape -------- //
    LAVA_API rp3d::ProxyShape* CreateCapsuleShape_Native(rp3d::RigidBody* rb, float radius, float height,
        rp3d::Vector3 pos, rp3d::Quaternion rot, float mass)
    {
        rp3d::Transform trans(pos, rot);
        rp3d::CapsuleShape* shape = Engine::PhysicsWorld::mCapsuleAllocator.newElement(radius, height);
        auto proxy = rb->addCollisionShape(shape, trans, mass);
        proxy->setUserData(shape);
        return proxy;
    }

    LAVA_API void DestroyCapsuleShape_Native(rp3d::RigidBody* rb, rp3d::ProxyShape* proxy)
    {
        Engine::PhysicsWorld::mCapsuleAllocator.deleteElement(
            static_cast<rp3d::CapsuleShape*>(proxy->getUserData())
        );
        rb->removeCollisionShape(proxy);
    }
}