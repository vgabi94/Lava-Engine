using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Lava.Engine;

namespace Lava.Physics
{
    [StructLayout(LayoutKind.Sequential)]
    public class PhysicsMaterial
    {
        // Friction coefficient (positive value)
        public float frictionCoefficient;

        // Rolling resistance factor (positive value)
        public float rollingResistance;

        // Bounciness during collisions (between 0 and 1) where 1 is for a very bouncy body
        public float bounciness;

        public PhysicsMaterial(float frictionCoefficient, float rollingResistance, float bounciness)
        {
            this.frictionCoefficient = frictionCoefficient;
            this.rollingResistance = rollingResistance;
            this.bounciness = bounciness;
        }

        public PhysicsMaterial() : this(0.5f, 0.1f, 0.05f) { }
    }

    public enum BodyType
    {
        Static, Kinematic, Dynamic
    }

    public sealed class RigidBody : Component
    {
        [DllImport("LavaCore.dll")]
        private static extern void SetType_Native(IntPtr rb, BodyType type);

        [DllImport("LavaCore.dll")]
        private static extern void SetPhysicsMaterial_Native(IntPtr rb, PhysicsMaterial mat);

        [DllImport("LavaCore.dll")]
        private static extern void SetMass_Native(IntPtr rb, float mass);

        [DllImport("LavaCore.dll")]
        private static extern float GetMass_Native(IntPtr rb);

        [DllImport("LavaCore.dll")]
        private static extern void EnableGravity_Native(IntPtr rb, bool flag);

        [DllImport("LavaCore.dll")]
        private static extern void SetTransform_Native(IntPtr rb, Mathematics.Vector3 pos, Mathematics.Quaternion rot);

        [DllImport("LavaCore.dll")]
        private static extern void ApplyForce_Native(IntPtr rb, Mathematics.Vector3 force, Mathematics.Vector3 pos);

        [DllImport("LavaCore.dll")]
        private static extern void ApplyForceToCenterOfMass_Native(IntPtr rb, Mathematics.Vector3 force);

        [DllImport("LavaCore.dll")]
        private static extern void ApplyTorque_Native(IntPtr rb, Mathematics.Vector3 torque);

        [DllImport("LavaCore.dll")]
        private static extern void SetLinearDamping_Native(IntPtr rb, float damping);

        [DllImport("LavaCore.dll")]
        private static extern void SetAngularDamping_Native(IntPtr rb, float damping);

        public IntPtr NativePtr { get; internal set; }

        public float AngularDamping
        {
            get => angularDamping;
            set
            {
                angularDamping = value;
                SetAngularDamping_Native(NativePtr, value);
            }
        }
        private float angularDamping;

        public float LinearDamping
        {
            get => linearDamping;
            set
            {
                linearDamping = value;
                SetLinearDamping_Native(NativePtr, value);
            }
        }
        private float linearDamping;

        public float Mass
        {
            get => GetMass_Native(NativePtr);
            set => SetMass_Native(NativePtr, value);
        }

        public bool EnabledGravity
        {
            get => enabledGravity;
            set { enabledGravity = value; EnableGravity_Native(NativePtr, enabledGravity); }
        }
        private bool enabledGravity;

        public PhysicsMaterial Material
        {
            get => material;
            set { material = value; SetPhysicsMaterial_Native(NativePtr, material); }
        }
        private PhysicsMaterial material;

        public BodyType Type
        {
            get { return bodyType; }
            set { bodyType = value; SetType_Native(NativePtr, bodyType); }
        }
        private BodyType bodyType;

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

        internal RigidBody() : this(Mathematics.Vector3.Zero, Mathematics.Quaternion.Identity) { }

        internal RigidBody(Mathematics.Vector3 pos, Mathematics.Quaternion rot)
        {
            position = pos;
            rotation = rot;
            visualTransform = null;
            bodyType = BodyType.Dynamic;
            enabledGravity = true;
            material = new PhysicsMaterial();
            angularDamping = 0.05f;
            linearDamping = 0f;
            shapes = new List<CollisionShape>();
        }

        internal void OnRigidBodyUpdate(Mathematics.Vector3 pos, Mathematics.Quaternion rot)
        {
            if (visualTransform == null)
                throw new Exception("RigidBody not added to VisualEntity!");
            position = pos;
            rotation = rot;
            visualTransform.Position = pos;
            visualTransform.Rotation = rot;
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
            shape.CreateProxy(this);
        }

        public void RemoveCollisionShape(CollisionShape shape)
        {
            shape.DestroyProxy(this);
            shapes.Remove(shape);
        }

        public T GetCollsionShape<T>() where T : CollisionShape
        {
            return shapes.Find(x => x is T) as T;
        }

        public void ApplyForce(Mathematics.Vector3 force, Mathematics.Vector3 point)
        {
            ApplyForce_Native(NativePtr, force, point);
        }

        public void ApplyForceToCenterOfMass(Mathematics.Vector3 force)
        {
            ApplyForceToCenterOfMass_Native(NativePtr, force);
        }

        public void ApplyTorque(Mathematics.Vector3 torque)
        {
            ApplyTorque_Native(NativePtr, torque);
        }
    }
}
