using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Lava.Engine
{
    public delegate void Callback();
    public delegate void FramebufferCallback(int w, int h);

    public static class Application
    {
        /// <summary>
        /// Initialize the application using param as window options.
        /// </summary>
        /// <param name="param">Specify the window creation options.</param>
        [DllImport("LavaCore.dll")]
        private static extern void Initialize(WindowParams param);

        /// <summary>
        /// Runs the application loop.
        /// </summary>
        [DllImport("LavaCore.dll")]
        public static extern void Run();

        /// <summary>
        /// Quit the application. Resources are released by the native code.
        /// </summary>
        [DllImport("LavaCore.dll")]
        public static extern void Quit();

        /// <summary>
        /// Register main game loop update callback function.
        /// </summary>
        [DllImport("LavaCore.dll")]
        private static extern void RegisterUpdateCallback(Callback update);

        /// <summary>
        /// Register the framebuffer resize callback.
        /// </summary>
        [DllImport("LavaCore.dll")]
        private static extern void RegisterFramebufferResizeCallback(FramebufferCallback cback);

        /// <summary>
        /// Set the fullscreen state of the window.
        /// </summary>
        /// <param name="flag">Set to true to go fullscreen</param>
        [DllImport("LavaCore.dll")]
        public static extern void SetFullScreen(bool flag);

        /// <summary>
        /// Loads a shader binary code into the shader manager
        /// </summary>
        /// <param name="path"></param>
        [DllImport("LavaCore.dll")]
        private static extern void LoadShader(string path);

        /// <summary>
        /// Loads a pipeline from a json file
        /// </summary>
        /// <param name="path"></param>
        [DllImport("LavaCore.dll")]
        private static extern void LoadPipeline(string path);

        /// <summary>
        /// Loads the base pipeline from a json file
        /// </summary>
        /// <param name="path"></param>
        [DllImport("LavaCore.dll")]
        private static extern void LoadBasePipeline(string path);

        /// <summary>
        /// Set the constant globals of this application
        /// </summary>
        /// <param name="globals"></param>
        [DllImport("LavaCore.dll")]
        public static extern void SetGlobals(Settings.Globals globals);

        /// <summary>
        /// Call this after all the shader modules are loaded
        /// </summary>
        [DllImport("LavaCore.dll")]
        public static extern void PostShaderLoadInitialize();

        // ------------------- CALLBACKS --------------------- //
        private static Callback updateCallback;
        private static FramebufferCallback framebufferCallback;

        private static void RegisterEvents()
        {
            updateCallback = new Callback(EventManager.OnUpdate);
            framebufferCallback = new FramebufferCallback(EventManager.OnFramebufferResize);
            RegisterUpdateCallback(updateCallback);
            RegisterFramebufferResizeCallback(framebufferCallback);
        }

#if LAVA_EDITOR
        [DllImport("LavaCore.dll")]
        public static extern IntPtr GetHwnd();
#endif

        public static void Init(WindowParams? wp = null)
        {
            WorldManager.Init();
            RegisterEvents();

            WindowParams p;
            if (wp == null)
            {
                p = new WindowParams(true);
            }
            else
            {
                p = wp.Value;
            }

            ShaderBuilder.BuildShaders();

            Initialize(p);

            LoadShaderModules();
            LoadPipelines();

            PostShaderLoadInitialize();
        }

        private static void LoadShaderModules()
        {
            foreach (var file in Directory.EnumerateFiles(Settings.ShaderDirPath))
            {
                LoadShader(file);
            }
        }

        private static void LoadPipelines()
        {
            string baseJson = Settings.PipelineDirPath + "\\base.json";
            LoadBasePipeline(baseJson);
            foreach (var file in Directory.EnumerateFiles(Settings.PipelineDirPath))
            {
                if (file == baseJson) continue;
                LoadPipeline(file);
            }
        }
    }
}
