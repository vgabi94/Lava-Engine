#include "Instance.h"

namespace Vulkan
{
    Instance g_Instance;

#ifdef _DEBUG
    vk::DebugReportCallbackEXT callback;

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData) {

        auto flagBits = static_cast<vk::DebugReportFlagBitsEXT>(flags);
        auto objTypeBits = static_cast<vk::DebugReportObjectTypeEXT>(objType);

        if (flagBits == vk::DebugReportFlagBitsEXT::eError)
            LOG_ERROR("<{}> {}\n", vk::to_string(objTypeBits), std::string(msg))
        else if (flagBits == vk::DebugReportFlagBitsEXT::eDebug ||
            flagBits == vk::DebugReportFlagBitsEXT::eInformation)
            LOG_INFO("<{}> {}\n", vk::to_string(objTypeBits), std::string(msg))
        else if (flagBits == vk::DebugReportFlagBitsEXT::ePerformanceWarning)
            LOG_ERROR("<{}> {}\n", vk::to_string(objTypeBits), std::string(msg))

            return VK_FALSE;
    }

    vk::Result CreateDebugReportCallbackEXT(const vk::Instance& instance, const vk::DebugReportCallbackCreateInfoEXT* createInfo,
        const vk::AllocationCallbacks* allocator, vk::DebugReportCallbackEXT* callback)
    {
        auto func = (PFN_vkCreateDebugReportCallbackEXT)instance.getProcAddr("vkCreateDebugReportCallbackEXT");

        if (func != nullptr)
        {
            return static_cast<vk::Result>(func((VkInstance)instance,
                reinterpret_cast<const VkDebugReportCallbackCreateInfoEXT*>(createInfo),
                reinterpret_cast<const VkAllocationCallbacks*>(allocator),
                reinterpret_cast<VkDebugReportCallbackEXT*>(callback)));
        }

        return vk::Result::eErrorExtensionNotPresent;
    }

    void DestroyDebugReportCallbackEXT(const vk::Instance& instance, vk::DebugReportCallbackEXT& callback,
        const vk::AllocationCallbacks* allocator)
    {
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)instance.getProcAddr("vkDestroyDebugReportCallbackEXT");

        if (func != nullptr)
        {
            func((VkInstance)instance, (VkDebugReportCallbackEXT)callback,
                reinterpret_cast<const VkAllocationCallbacks*>(allocator));
        }
    }
#endif

    void Instance::Init()
    {
        vk::ApplicationInfo appInfo(
            "Vulkan application",
            VK_MAKE_VERSION(1, 0, 0),
            "LavaEngine",
            VK_MAKE_VERSION(1, 0, 0),
            VK_MAKE_VERSION(1, 1, 70)
        );

        StringList extensions;
        GetGLFWRequiredExtensions(extensions);

#ifdef _DEBUG
        assert(PopulateValidationLayers());

        vk::InstanceCreateInfo instanceInfo({},
            &appInfo,
            static_cast<uint32_t>(mValidationLayers.size()),
            mValidationLayers.data(),
            static_cast<uint32_t>(extensions.size()),
            extensions.data());
#else
        vk::InstanceCreateInfo instanceInfo({},
            &appInfo,
            0,
            nullptr,
            static_cast<uint32_t>(extensions.size()),
            extensions.data());
#endif

        mInstance = vk::createInstance(instanceInfo);

#ifdef _DEBUG
        InitDebug();
#endif
    }

    void Instance::Destroy()
    {
#ifdef _DEBUG
        DestroyDebugReportCallbackEXT(mInstance, callback, nullptr);
#endif
        mInstance.destroy();
    }
    
    void Instance::GetGLFWRequiredExtensions(StringList & extensions)
    {
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        assert(glfwExtensions != nullptr);

#ifdef _DEBUG
        extensions.resize(glfwExtensionCount + 1);
#else
        extensions.resize(glfwExtensionCount);
#endif

        for (uint32_t i = 0; i < glfwExtensionCount; i++)
            extensions[i] = glfwExtensions[i];

#ifdef _DEBUG
        extensions[glfwExtensionCount] = (VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
    }

    void Instance::CreateSurface(GLFWwindow* window)
    {
        VkSurfaceKHR surf;
        VkResult res = glfwCreateWindowSurface(
            mInstance,
            window,
            nullptr,
            &surf);
        assert(res == VK_SUCCESS);
        mSurface = vk::SurfaceKHR(surf);
    }

    void Instance::DestroySurface()
    {
        mInstance.destroySurfaceKHR(mSurface);
    }

#ifdef _DEBUG
    bool Instance::PopulateValidationLayers()
    {
		std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

		StringList foundValidationLayers;
		foundValidationLayers.reserve(mValidationLayers.size());

        mValidationLayers = 
        {
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_monitor"
        };

        for (const char* layerName : mValidationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProp : availableLayers)
            {
                if (strcmp(layerName, layerProp.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

			if (!layerFound)
			{
				LOG_INFO("Could not enable {0} layer!\n", layerName);
			}
			else
			{
				foundValidationLayers.push_back(layerName);
			}
        }

		mValidationLayers = foundValidationLayers;

        return true;
    }

    void Instance::InitDebug()
    {
        vk::DebugReportCallbackCreateInfoEXT debugInfo(
            vk::DebugReportFlagBitsEXT::eError | 
            vk::DebugReportFlagBitsEXT::eWarning |
            /*vk::DebugReportFlagBitsEXT::eInformation | */
            vk::DebugReportFlagBitsEXT::ePerformanceWarning,
            DebugCallback
        );

        vk::Result res = CreateDebugReportCallbackEXT(mInstance, &debugInfo, nullptr, &callback);
        assert(res == vk::Result::eSuccess);
    }
#endif
}
