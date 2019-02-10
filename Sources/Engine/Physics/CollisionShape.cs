using System;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Physics
{
    public abstract class CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern void SetShapeTransform_Native(IntPtr proxy, Vector3 pos, Quaternion rot);

        public IntPtr NativePtr { get; internal set; }

        public Vector3 Position { get; set; }
        public Quaternion Rotation { get; set; }
        public float Mass { get; set; }

        public CollisionShape(float mass)
        {
            Position = Vector3.Zero;
            Rotation = Quaternion.Identity;
            Mass = mass;
        }

        public void SetLocalToBodyTransform()
        {
            SetShapeTransform_Native(NativePtr, Position, Rotation);
        }

        public abstract void CreateProxy(RigidBody rb);
        public abstract void DestroyProxy(RigidBody rb);
    }

    public class BoxShape : CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateBoxShape_Native(IntPtr rb, Vector3 halfExtents,
            Vector3 pos, Quaternion rot, float mass);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr DestroyBoxShape_Native(IntPtr rb, IntPtr proxy);

        public override void CreateProxy(RigidBody rb)
        {
            NativePtr = CreateBoxShape_Native(rb.NativePtr, HalfExtent, Position, Rotation, Mass);
        }

        public override void DestroyProxy(RigidBody rb)
        {
            DestroyBoxShape_Native(rb.NativePtr, NativePtr);
        }

        public Vector3 HalfExtent { get; private set; }

        public BoxShape(float radius, float mass = 1f) : base(mass)
        {
            HalfExtent = new Vector3(radius);
        }

        public BoxShape(float x, float y, float z, float mass = 1f) : base(mass)
        {
            HalfExtent = new Vector3(x, y, z);
        }

        public BoxShape(Vector3 halfExtent, float mass = 1f) : base(mass)
        {
            HalfExtent = halfExtent;
        }

        public BoxShape() : this(1f, 1f) { }
    }

    public class SphereShape : CollisionShape
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateSphereShape_Native(IntPtr rb, float radius,
            Vector3 pos, Quaternion rot, float mass);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr DestroySphereShape_Native(IntPtr rb, IntPtr proxy);

        public float Radius { get; private set; }

        public SphereShape(float radius, float mass = 1f) : base(mass)
        {
            Radius = radius;
        }

        public SphereShape() : this(1f, 1f) { }

        public override void CreateProxy(RigidBody rb)
        {
            NativePtr = CreateSphereShape_Native(rb.NativePtr, Radius, Position, Rotation, Mass);
        }

        public override void DestroyProxy(RigidBody rb)
        {
            DestroySphereShape_Native(rb.NativePtr, NativePtr);
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

        public CapsuleShape(float radius, float height, float mass = 1f) : base(mass)
        {
            Radius = radius;
            Height = height;
        }

        public CapsuleShape() : this(1f, 1f, 1f) { }

        public override void CreateProxy(RigidBody rb)
        {
            NativePtr = CreateCapsuleShape_Native(rb.NativePtr, Radius, Height, Position, Rotation, Mass);
        }

        public override void DestroyProxy(RigidBody rb)
        {
            DestroyCapsuleShape_Native(rb.NativePtr, NativePtr);
        }
    }
}
