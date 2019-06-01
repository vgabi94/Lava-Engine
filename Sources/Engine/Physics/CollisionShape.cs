using System;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Physics
{
    public struct CollisionInfoMarshal
    {
        public Vector3 pointOfContact;
    }

    public struct CollisionInfo
    {
        public Vector3 pointOfContact;
        public Lava.Engine.Component owner;
    }

    public abstract class CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern void SetShapeTransform_Native(IntPtr proxy, Vector3 pos, Quaternion rot);

        public IntPtr NativePtr { get; internal set; }

        public RigidBody RigidBody { get; protected set; }

        public CollisionBody CollisionBody { get; protected set; }

        public Vector3 Position { get; set; }
        public Quaternion Rotation { get; set; }
        public float Mass { get; set; }

        public bool IsTrigger { get; private set; }

        public CollisionShape(bool trigger, float mass)
        {
            Position = Vector3.Zero;
            Rotation = Quaternion.Identity;
            Mass = mass;
            if (trigger)
            {
                onCollision = new CollisionCallback(OnCollision);
                IsTrigger = trigger;
            }
        }

        public void SetLocalToBodyTransform()
        {
            SetShapeTransform_Native(NativePtr, Position, Rotation);
        }

        public abstract void CreateProxy(RigidBody rb);
        public abstract void CreateProxyTrigger(CollisionBody cb);
        public abstract void DestroyProxy(RigidBody rb);
        public abstract void DestroyProxyTrigger(CollisionBody rb);

        protected abstract void RegisterCollisionCallback();

        public delegate void CollisionHandler(CollisionInfo collisionInfo);
        public event CollisionHandler CollisionEvent;

        protected delegate void CollisionCallback(ref CollisionInfoMarshal collInfo);
        protected CollisionCallback onCollision; 

        protected void OnCollision(ref CollisionInfoMarshal collInfo)
        {
            CollisionInfo info = new CollisionInfo();
            info.pointOfContact = collInfo.pointOfContact;
            info.owner = CollisionBody;
            CollisionEvent?.Invoke(info);
        }
    }

    public class BoxShape : CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateBoxShape_Native(IntPtr rb, Vector3 halfExtents,
            Vector3 pos, Quaternion rot, float mass);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateBoxTrigger_Native(IntPtr cb, Vector3 halfExtents,
            Vector3 pos, Quaternion rot);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr DestroyBoxShape_Native(IntPtr rb, IntPtr proxy);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr DestroyBoxTrigger_Native(IntPtr cb, IntPtr proxy);

        [DllImport("LavaCore.dll")]
        private static extern void SetBoxTriggerCallback_Native(IntPtr pworld, IntPtr proxy, CollisionCallback cback);

        public override void CreateProxy(RigidBody rb)
        {
            NativePtr = CreateBoxShape_Native(rb.NativePtr, HalfExtent, Position, Rotation, Mass);
            RigidBody = rb;
        }

        public override void DestroyProxy(RigidBody rb)
        {
            DestroyBoxShape_Native(rb.NativePtr, NativePtr);
        }

        public override void CreateProxyTrigger(CollisionBody cb)
        {
            NativePtr = CreateBoxTrigger_Native(cb.NativePtr, HalfExtent, Position, Rotation);
            CollisionBody = cb;
            RegisterCollisionCallback();
        }

        public override void DestroyProxyTrigger(CollisionBody cb)
        {
            DestroyBoxTrigger_Native(cb.NativePtr, NativePtr);
        }

        protected override void RegisterCollisionCallback()
        {
            SetBoxTriggerCallback_Native(CollisionBody.PhysicsWorld.NativePtr, NativePtr, onCollision);
        }

        public Vector3 HalfExtent { get; private set; }

        public BoxShape(float radius, bool trigger = false, float mass = 1f) : base(trigger, mass)
        {
            HalfExtent = new Vector3(radius);
        }

        public BoxShape(float x, float y, float z, bool trigger = false, float mass = 1f) : base(trigger, mass)
        {
            HalfExtent = new Vector3(x, y, z);
        }

        public BoxShape(Vector3 halfExtent, bool trigger = false, float mass = 1f) : base(trigger, mass)
        {
            HalfExtent = halfExtent;
        }

        public BoxShape() : this(1f, false, 1f) { }
    }

    public class SphereShape : CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateSphereShape_Native(IntPtr rb, float radius,
            Vector3 pos, Quaternion rot, float mass);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr DestroySphereShape_Native(IntPtr rb, IntPtr proxy);

        public float Radius { get; private set; }

        public SphereShape(float radius, bool trigger = false, float mass = 1f) : base(trigger, mass)
        {
            Radius = radius;
        }

        public SphereShape() : this(1f, false, 1f) { }

        public override void CreateProxy(RigidBody rb)
        {
            NativePtr = CreateSphereShape_Native(rb.NativePtr, Radius, Position, Rotation, Mass);
        }

        public override void DestroyProxy(RigidBody rb)
        {
            DestroySphereShape_Native(rb.NativePtr, NativePtr);
        }

        public override void CreateProxyTrigger(CollisionBody cb)
        {
            throw new NotImplementedException();
        }

        public override void DestroyProxyTrigger(CollisionBody rb)
        {
            throw new NotImplementedException();
        }

        protected override void RegisterCollisionCallback()
        {
            throw new NotImplementedException();
        }
    }

    public class CapsuleShape : CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateCapsuleShape_Native(IntPtr rb, float radius, float height,
            Vector3 pos, Quaternion rot, float mass);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr DestroyCapsuleShape_Native(IntPtr rb, IntPtr proxy);

        public float Radius { get; private set; }
        public float Height { get; private set; }

        public CapsuleShape(float radius, float height, bool trigger = false, float mass = 1f) : base(trigger, mass)
        {
            Radius = radius;
            Height = height;
        }

        public CapsuleShape() : this(1f, 1f, false, 1f) { }

        public override void CreateProxy(RigidBody rb)
        {
            NativePtr = CreateCapsuleShape_Native(rb.NativePtr, Radius, Height, Position, Rotation, Mass);
        }

        public override void DestroyProxy(RigidBody rb)
        {
            DestroyCapsuleShape_Native(rb.NativePtr, NativePtr);
        }

        public override void CreateProxyTrigger(CollisionBody cb)
        {
            throw new NotImplementedException();
        }

        public override void DestroyProxyTrigger(CollisionBody rb)
        {
            throw new NotImplementedException();
        }

        protected override void RegisterCollisionCallback()
        {
            throw new NotImplementedException();
        }
    }
}
