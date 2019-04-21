#include "Engine.h"
#include "Time.h"
#include "Device.h"
#include "Swapchain.h"
#include "TaskScheduler.h"
#include <Manager\WorldManager.h>
#include <Manager\ShaderManager.h>
#include <Manager\PipelineManager.h>
#include <Manager\BufferManager.h>
#include <Manager\TextureManager.h>
#include <Manager\RenderpassManager.h>
#include <Manager\ResourceManager.h>
#include <Manager\MaterialManager.h>
#include <Manager\PrimitiveManager.h>
#include <Manager\AudioManager.h>
#include <Manager\UIManager.h>

#define EngineGlobals Engine::g_EngineSettings

namespace Engine
{
    Engine g_Engine;
    EngineSettings g_EngineSettings;

    void Engine::Init(WindowParams params)
    {
        assert(glfwInit());
        CreateWindow(params);
        //g_TaskScheduler.Init();
		g_PrimitiveManager.Init();
		g_AudioManager.Init();
        InitGraphics();
    }

    void Engine::PostShaderLoadInit()
    {
        // The pipelines have to be created after the shader modules are loaded
        // This function is called from .NET runtime
		g_RenderpassManager.PostShaderLoadInit();
        g_WorldManager.Init();
    }
    
    void Engine::Destroy()
    {
        // RenderPassManager is needed here to wait for command buffers to complete
        g_RenderpassManager.Destroy();
        g_WorldManager.Destroy();
        DestroyGraphics();
		g_PrimitiveManager.Destroy();
		g_AudioManager.Destroy();
        //g_TaskScheduler.Destroy();
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }
    
    void Engine::Update()
    {
        UpdateCallback();

#ifdef SINGLE_THREAD
        g_TaskScheduler.Execute();
#endif
		g_UIManager.MirrorInput();
		g_UIManager.Update();
		g_UIManager.SetupDrawBuffers();

        g_BufferManager.ExecuteOperations();
        g_TextureManager.ExecuteOperations();

        g_WorldManager.UpdatePhysicsWorld();

		g_ResourceManager.ExecuteIBLPasses();
        
        GSwapchain.Update();

		g_UIManager.FreeDrawBuffers();
    }
    
    void Engine::CreateWindow(WindowParams params)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, params.Resizable);
        glfwWindowHint(GLFW_FOCUSED, params.Focused);
        glfwWindowHint(GLFW_VISIBLE, params.Visible);
        glfwWindowHint(GLFW_MAXIMIZED, params.Maximized);

#if LAVA_EDITOR
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        params.Fullscreen = false;
        assert(params.Parent != nullptr);
#endif

        std::string appname(params.Name);
        appname += ": Vulkan";

        if (params.Fullscreen)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            mWindow = glfwCreateWindow(mode->width, mode->height, appname.c_str(), monitor, nullptr);
            mWidth = mode->width;
            mHeight = mode->height;
            mMonitor = monitor;
        }
        else
        {
            mWindow = glfwCreateWindow(params.Width, params.Height, appname.c_str(), nullptr, nullptr);
            mWidth = params.Width;
            mHeight = params.Height;
        }

        mOldHeight = params.Height;
        mOldWidth = params.Width;

        assert(mWindow != nullptr);

#if LAVA_EDITOR
        HWND native = glfwGetWin32Window(mWindow);
        SetParent(native, params.Parent);
        LONG newStyle = GetWindowLong(native, GWL_STYLE) & ~WS_POPUP | WS_CHILDWINDOW;
        SetWindowLong(native, GWL_STYLE, newStyle);
        glfwShowWindow(mWindow);
#endif
    }

    void Engine::InitGraphics()
    {
        using namespace Vulkan;
        g_Instance.Init();
        g_Instance.CreateSurface(mWindow);
        g_Device.Init();
        g_BufferManager.Init();
        g_TextureManager.Init();
		g_ResourceManager.Init();
        g_RenderpassManager.Init();
        g_Swapchain.Init();
		g_RenderpassManager.PostSwapchainInit();
        g_ShaderManager.Init();
        g_PipelineManager.Init();
		g_MaterialManager.Init();
		g_UIManager.Init();
    }

    void Engine::DestroyGraphics()
    {
        using namespace Vulkan;
		g_UIManager.Destroy();
		g_MaterialManager.Destroy();
        g_ResourceManager.Destroy();
        g_TextureManager.Destroy();
        g_PipelineManager.Destroy();
        g_BufferManager.Destroy();
        g_ShaderManager.Destroy();
        g_Swapchain.Destroy();
        g_Device.Destroy();
        g_Instance.Destroy();
    }

    void Engine::SetFullscreen(bool flag)
    {
        if (flag)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(mWindow, monitor, 0, 0, mode->width,
                mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(mWindow, nullptr, 40, 40, mOldWidth, mOldHeight, 0);
        }
    }

    void OnFramebufferResize(GLFWwindow * window, int width, int height)
    {
        //assert(g_VulkanFramebufferResize != nullptr);

        //g_VulkanFramebufferResize(width, height);

        if (g_Engine.FramebufferResize != nullptr)
        {
            g_Engine.FramebufferResize(width, height);
        }
        g_Engine.SetWidth(width);
        g_Engine.SetHeight(height);
    }
}

/* EXPORTED INTERFACE */
extern "C"
{
    LAVA_API void RegisterUpdateCallback(Engine::UpdateCback fun)
    {
        GEngine.UpdateCallback = fun;
    }

    LAVA_API void RegisterFramebufferResizeCallback(Engine::FramebufferResizeCback fun)
    {
        GEngine.FramebufferResize = fun;
    }

    LAVA_API void Initialize(Engine::WindowParams params)
    {
        GEngine.Init(params);

        Engine::g_Input.Init();
        Engine::g_Input.RegisterCallbacks(g_Window);
    }

    LAVA_API void PostShaderLoadInitialize()
    {
        assert(g_Window != nullptr);

        GEngine.PostShaderLoadInit();
        glfwSetFramebufferSizeCallback(g_Window, Engine::OnFramebufferResize);
    }

    LAVA_API void Run()
    {
        assert(g_Window != nullptr);
        assert(GEngine.UpdateCallback != nullptr);

        while (!glfwWindowShouldClose(g_Window))
        {
            Engine::g_Time.Update();
            // Reset input state BEFORE we poll the next events
            Engine::g_Input.Update();
            glfwPollEvents();

            GEngine.Update();
        }

        GEngine.Destroy();
    }

    LAVA_API void Quit()
    {
        assert(g_Window != nullptr);
        glfwSetWindowShouldClose(g_Window, true);
    }

    LAVA_API void SetFullScreen(bool flag)
    {
        GEngine.SetFullscreen(flag);
    }

    LAVA_API void SetGlobals(Engine::EngineSettingsMarshal globals)
    {
        EngineGlobals.MaterialJSONPath = globals.MaterialDirPath;
        EngineGlobals.PipelineJSONPath = globals.PipelineJSONPath;
        EngineGlobals.ShaderModulePath = globals.ShaderModulePath;
        EngineGlobals.ShaderSourcePath = globals.ShaderSourcePath;
        EngineGlobals.TexturesPath = globals.TexturesPath;
    }

#ifdef LAVA_EDITOR
    LAVA_API HWND GetHwnd()
    {
        return glfwGetWin32Window(g_Window);
    }
#endif // LAVA_EDITOR
}