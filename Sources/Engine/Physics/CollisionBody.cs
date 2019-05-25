using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Lava.Engine;

namespace Lava.Physics
{
    public sealed class CollisionBody : Component
    {
        [DllImport("LavaCore.dll")]
        private static extern void SetType_Native(IntPtr rb, BodyType type);

        [DllImport("LavaCore.dll")]
        private static extern void SetTransform_Native(IntPtr rb, Mathematics.Vector3 pos, Mathematics.Quaternion rot);

        public IntPtr NativePtr { get; internal set; }

        public UpdateRigidBodyCallback UpdateCallback { get; set; }

        public Mathematics.Vector3 Position
        {
            get { return position; }
            set { position = value; SetTransform_Native(NativePtr, position, rotation); }
        }
        private Mathematics.Vector3 position;

        public Mathematics.Quaternion Rotation
        {
            get { return rotation; }
            set { rotation = value; SetTransform_Native(NativePtr, position, rotation); }
        }
        private Mathematics.Quaternion rotation;

        internal CollisionBody() : this(Mathematics.Vector3.Zero, Mathematics.Quaternion.Identity) { }

        internal CollisionBody(Mathematics.Vector3 pos, Mathematics.Quaternion rot)
        {
            position = pos;
            rotation = rot;
            visualTransform = null;
            shapes = new List<CollisionShape>();
        }

        private Transform visualTransform;

        protected internal override void OnEntityAddOwner()
        {
            visualTransform = Owner.GetComponent<Transform>();
            if (visualTransform == null)
                throw new Exception("Transform of VisualEntity not found!");
        }

        protected internal override void OnEntityRemoveOwner()
        {
            visualTransform = null;
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
