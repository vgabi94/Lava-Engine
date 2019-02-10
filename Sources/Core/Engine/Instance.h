#pragma once

#include <vulkan\vulkan.hpp>

#include <GLFW\glfw3.h>
#include <Common\Constants.h>

#define g_vkInstance ((vk::Instance)Vulkan::g_Instance)
#define g_vkSurface (g_Instance.GetSurface())

namespace Vulkan
{
    typedef std::vector<const char*> StringList;

    struct QueueFamilyIndices
    {
        int graphics = -1;
        int presentation = -1;

        bool IsComplete()
        {
            return graphics > -1 && presentation > -1;
        }
    };


    class Instance
    {
        friend class Device;

    public:
        operator vk::Instance() const { return mInstance; }

        vk::SurfaceKHR GetSurface() const { return mSurface; }

        void Init();
        void Destroy();
        void CreateSurface(GLFWwindow* window);
        void DestroySurface();

    private:
        vk::Instance mInstance;
        vk::SurfaceKHR mSurface;

        void GetGLFWRequiredExtensions(StringList& extensions);

#ifdef _DEBUG
        StringList mValidationLayers;
        bool PopulateValidationLayers();
        void InitDebug();
#endif
    };

    extern Instance g_Instance;
}