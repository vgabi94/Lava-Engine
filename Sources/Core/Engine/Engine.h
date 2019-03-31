#pragma once
#include "Input.h"

#ifdef LAVA_EDITOR
#ifdef ERROR
#undef ERROR
#endif
#define GLFW_EXPOSE_NATIVE_WIN32
#include "Include\GLFW\glfw3native.h"
#endif // LAVA_EDITOR

#define g_Window (Engine::g_Engine.GetWindow())
#define WINDOW_WIDTH (Engine::g_Engine.GetWidth())
#define WINDOW_HEIGHT (Engine::g_Engine.GetHeight())
#define GWINDOW_WIDTH (g_Engine.GetWidth())
#define GWINDOW_HEIGHT (g_Engine.GetHeight())
#define GEngine Engine::g_Engine

namespace Engine
{
    struct WindowParams
    {
        int Resizable;
        int Focused;
        int Visible;
        int Maximized;
        int Fullscreen;
        int Width;
        int Height;
        const char* Name;
#if LAVA_EDITOR
        HWND Parent;
#endif
    };

    struct EngineSettings
    {
        std::string ShaderModulePath;
        std::string PipelineJSONPath;
        std::string MaterialJSONPath;
        std::string ShaderSourcePath;
        std::string TexturesPath;
    };

    extern EngineSettings g_EngineSettings;

    class Engine
    {
    public:
        void Init(WindowParams params);
        void PostShaderLoadInit();
        void Destroy();

        void Update();
        void SetFullscreen(bool flag);

        GLFWwindow* GetWindow() const { return mWindow; }
        uint32_t GetWidth() const { return mWidth; }
        uint32_t GetHeight() const { return mHeight; }

        void SetWidth(uint32_t width) { mWidth = width; }
        void SetHeight(uint32_t height) { mHeight = height; }

        UpdateCback UpdateCallback = nullptr;
        FramebufferResizeCback FramebufferResize = nullptr;

    private:
        GLFWwindow* mWindow;
        GLFWmonitor* mMonitor;

        uint32_t mWidth, mHeight;
        uint32_t mOldWidth, mOldHeight;

        void CreateWindow(WindowParams params);
        void InitGraphics();
        void DestroyGraphics();
    };

    extern Engine g_Engine;

    struct EngineSettingsMarshal
    {
        const char* ShaderModulePath;
        const char* MaterialDirPath;
        const char* PipelineJSONPath;
        const char* ShaderSourcePath;
		const char* TexturesPath;
    };
}
