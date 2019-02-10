using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Lava.Engine
{
    public class World
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateWorld_Native(bool makeCurrent, bool hasPhysics);

        [DllImport("LavaCore.dll")]
        private static extern void AddEntity_Native(IntPtr world, IntPtr ent);

        [DllImport("LavaCore.dll")]
        private static extern void SetCameraPos_Native(IntPtr world, Mathematics.Vector3 camPos);

        [DllImport("LavaCore.dll")]
        private static extern void SetViewBounds_Native(IntPtr world, Mathematics.Vector3 bmin, Mathematics.Vector3 bmax);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr GetPhysicsWorld_Native(IntPtr world);

        public IntPtr NativePtr { get; private set; }

        public Physics.PhysicsWorld PhysicsWorld { get; private set; }

        public void AddEntity(Entity ent)
        {
            if (ent is VisualEntity)
            {
                VisualEntity vent = ent as VisualEntity;
                vent.UpdatePosition();
                AddEntity_Native(NativePtr, vent.NativePtr);
            }
            
            entities.Add(ent);
            ent.World = this;
        }

        internal void Update()
        {
            foreach (var ent in entities)
            {
                ent.Update();
            }

            if (Camera.Main != null)
            {
                SetCameraPos_Native(NativePtr, Camera.Main.Position);

                //Mathf.Vector3 bmin = Camera.Main.Position - Camera.Main.Right * Camera.Main.FarPlane / 2f
                //    - Camera.Main.Up * Camera.Main.FarPlane / 2f;
                //Mathf.Vector3 bmax = Camera.Main.Position + Camera.Main.Right * Camera.Main.FarPlane / 2f
                //    + Camera.Main.Up * Camera.Main.FarPlane / 2f + Camera.Main.Forward * Camera.Main.FarPlane;
                Mathematics.Vector3 bmin = Camera.Main.Position - new Mathematics.Vector3(Camera.Main.FarPlane + 5f);
                Mathematics.Vector3 bmax = Camera.Main.Position + new Mathematics.Vector3(Camera.Main.FarPlane + 5f);
                SetViewBounds_Native(NativePtr, bmin, bmax);
            }
        }

        internal World(bool makeCurrent = false, bool hasPhysics = true)
        {
            entities = new HashSet<Entity>();
            NativePtr = CreateWorld_Native(makeCurrent, hasPhysics);

            if (hasPhysics)
            {
                PhysicsWorld = new Physics.PhysicsWorld();
                PhysicsWorld.NativePtr = GetPhysicsWorld_Native(NativePtr);
            }
            else
            {
                PhysicsWorld = null;
            }
        }

        private HashSet<Entity> entities;
    }
}
