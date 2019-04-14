using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Engine
{
    public static class WorldManager
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr GetCurrentWorld_Native();

        [DllImport("LavaCore.dll")]
        private static extern void SetCurrentWorld_Native(IntPtr world);

        [DllImport("LavaCore.dll")]
        private static extern void SetPhysicsCallback_Native(IntPtr pworld, Callback cback);

        //[DllImport("LavaCore.dll")]
        //private static extern void SetViewProjection(Matrix4 vp);

        // ------------- CALLBACKS --------------- //
        private static Callback physicsUpdateCallback;

        public static World CreateWorld(bool makeCurrent = true, bool hasPhysics = true)
        {
            World world = new World(makeCurrent, hasPhysics);
            currentWorld = world;
            if (hasPhysics)
            {
                physicsUpdateCallback = new Callback(EventManager.OnPhysicsUpdate);
                SetPhysicsCallback_Native(world.PhysicsWorld.NativePtr, physicsUpdateCallback);
            }
            worlds.Add(world);
            return world;
        }

        public static World CurrentWorld
        {
            get { return currentWorld; }
            set
            {
                SetCurrentWorld_Native(value.NativePtr);
                if (value.PhysicsWorld != null)
                {
                    physicsUpdateCallback = new Callback(EventManager.OnPhysicsUpdate);
                    SetPhysicsCallback_Native(value.PhysicsWorld.NativePtr, physicsUpdateCallback);
                }
                currentWorld = value;
            }
        }
        private static World currentWorld;

        public static void UpdateCurrentWorld()
        {
            IntPtr native = GetCurrentWorld_Native();
            foreach (var item in worlds)
            {
                if (item.NativePtr == native)
                {
                    currentWorld = item;
                    break;
                }
            }
        }

        private static List<World> worlds = new List<World>();

        private static void Update()
        {
            currentWorld?.Update();
        }

        public static void Init()
        {
            EventManager.UpdateEvent += Update;
        }
    }
}
