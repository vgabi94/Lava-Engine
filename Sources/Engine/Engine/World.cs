﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Engine
{
    public class World
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateWorld_Native(bool makeCurrent, bool hasPhysics);

        [DllImport("LavaCore.dll")]
        private static extern void AddEntity_Native(IntPtr world, IntPtr ent);

        [DllImport("LavaCore.dll")]
        private static extern void AddLightInfo_Native(IntPtr world, LightInfo li);

        [DllImport("LavaCore.dll")]
        private static extern void SetCameraPos_Native(IntPtr world, Vector3 camPos);

        [DllImport("LavaCore.dll")]
        private static extern void AddIBLProbeInfo_Native(IntPtr world, IBLProbeInfo iblInfo);

        [DllImport("LavaCore.dll")]
        private static extern void SetSkySettings_Native(IntPtr world, SkySettings skySettings);

        [DllImport("LavaCore.dll")]
        private static extern void SetSkyViewProj_Native(IntPtr world, Matrix4 viewProj);

        [DllImport("LavaCore.dll")]
        private static extern void SetViewBounds_Native(IntPtr world, Vector3 bmin, Vector3 bmax);

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

            Light lightComponent = ent.GetComponent<Light>();
            if (lightComponent != null)
            {
                AddLightInfo_Native(NativePtr, lightComponent.ToLightInfo());
            }

            IBLProbe ibl = ent.GetComponent<IBLProbe>();
            if (ibl != null)
            {
                AddIBLProbeInfo_Native(NativePtr, ibl.info);
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
                Vector3 bmin = Camera.Main.Position - new Vector3(Camera.Main.FarPlane + 5f);
                Vector3 bmax = Camera.Main.Position + new Vector3(Camera.Main.FarPlane + 5f);
                SetViewBounds_Native(NativePtr, bmin, bmax);
                SetSkyViewProj_Native(NativePtr, Camera.Main.SkyViewProj);
                SetSkySettings_Native(NativePtr, skySettings);
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

        public SkySettings skySettings;
    }
}
