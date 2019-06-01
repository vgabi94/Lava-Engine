using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Lava.Engine;

namespace Lava.Physics
{
    public sealed class CollisionBody : Component
    {
        [DllImport("LavaCore.dll")]
        private static extern void SetTransformCb_Native(IntPtr cb, Mathematics.Vector3 pos, Mathematics.Quaternion rot);

        public IntPtr NativePtr { get; internal set; }

        public UpdateRigidBodyCallback UpdateCallback { get; set; }

        public Mathematics.Vector3 Position
        {
            get { return position; }
            set { position = value; SetTransformCb_Native(NativePtr, position, rotation); }
        }
        private Mathematics.Vector3 position;

        public Mathematics.Quaternion Rotation
        {
            get { return rotation; }
            set { rotation = value; SetTransformCb_Native(NativePtr, position, rotation); }
        }
        private Mathematics.Quaternion rotation;

        public void SetTransform(Mathematics.Vector3 position, Mathematics.Quaternion rotation)
        {
            this.position = position;
            this.rotation = rotation;
            SetTransformCb_Native(NativePtr, position, rotation);
        }

        public PhysicsWorld PhysicsWorld { get; internal set; }

        internal CollisionBody() : this(Mathematics.Vector3.Zero, Mathematics.Quaternion.Identity) { }

        internal CollisionBody(Mathematics.Vector3 pos, Mathematics.Quaternion rot)
        {
            position = pos;
            rotation = rot;
            shapes = new List<CollisionShape>();
        }

        internal void OnCollisionBodyUpdate(Mathematics.Vector3 pos, Mathematics.Quaternion rot)
        {
            position = pos;
            rotation = rot;
        }

        protected internal override void OnEntityAddOwner()
        {
        }

        protected internal override void OnEntityRemoveOwner()
        {
        }

        private List<CollisionShape> shapes;

        public void AddCollisionShape(CollisionShape shape)
        {
            shapes.Add(shape);
            shape.CreateProxyTrigger(this);
        }

        public void RemoveCollisionShape(CollisionShape shape)
        {
            shape.DestroyProxyTrigger(this);
            shapes.Remove(shape);
        }

        public T GetCollsionShape<T>() where T : CollisionShape
        {
            return shapes.Find(x => x is T) as T;
        }
    }
}
