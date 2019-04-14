using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Lava.Physics
{
    public delegate void UpdateRigidBodyCallback(Mathematics.Vector3 pos, Mathematics.Quaternion rot);

    public class PhysicsWorld
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateRigidBody_Native(IntPtr pworld, Mathematics.Vector3 pos,
            Mathematics.Quaternion rot, UpdateRigidBodyCallback cback);

        [DllImport("LavaCore.dll")]
        private static extern void SetGravity_Native(IntPtr pworld, Mathematics.Vector3 gravuty);

        public IntPtr NativePtr { get; internal set; }

        public Mathematics.Vector3 Gravity
        {
            set => SetGravity_Native(NativePtr, value);
        }

        public RigidBody CreateRigidBody(Mathematics.Vector3 position, Mathematics.Quaternion rotation)
        {
            RigidBody rb = new RigidBody(position, rotation);
            rb.UpdateCallback = new UpdateRigidBodyCallback(rb.OnRigidBodyUpdate);
            rb.NativePtr = CreateRigidBody_Native(NativePtr, position, rotation, rb.UpdateCallback);
            return rb;
        }

        public RigidBody CreateRigidBody(Mathematics.Vector3 position)
        {
            RigidBody rb = new RigidBody(position, Mathematics.Quaternion.Identity);
            rb.UpdateCallback = new UpdateRigidBodyCallback(rb.OnRigidBodyUpdate);
            rb.NativePtr = CreateRigidBody_Native(NativePtr, position, Mathematics.Quaternion.Identity, rb.UpdateCallback);
            return rb;
        }

        public RigidBody CreateRigidBody()
        {
            RigidBody rb = new RigidBody();
            rb.UpdateCallback = new UpdateRigidBodyCallback(rb.OnRigidBodyUpdate);
            rb.NativePtr = CreateRigidBody_Native(NativePtr, Mathematics.Vector3.Zero, Mathematics.Quaternion.Identity, rb.UpdateCallback);
            return rb;
        }
    }
}
