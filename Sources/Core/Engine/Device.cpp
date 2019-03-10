#include "Device.h"

namespace Vulkan
{
    Device g_Device;

    void Device::Init()
    {
        PickPhysicalDevice();
        FindQueueFamilies();
        CreateLogicalDevice();
    }

    void Device::Destroy()
    {
        mDevice.destroy();
    }
    
    SwapChainSupportDetails Device::QuerySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
    {
        SwapChainSupportDetails details;

		details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
		details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
		details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

        return details;
    }

    void Device::PickPhysicalDevice()
    {
        vk::Instance instance = g_vkInstance;
        uint32_t deviceCount;

        std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
        deviceCount = devices.size();

        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device, mPhysicalDeviceLimits) && 
                CheckDeviceExtensionSupport(device, mDeviceExtensions))
            {
                SwapChainSupportDetails scsd = QuerySwapChainSupport(device, g_vkSurface);
                if (!scsd.formats.empty() && !scsd.presentModes.empty())
                {
                    mPhysicalDevice = device;
                    mSCSD = scsd;
                    vk::PhysicalDeviceProperties pdp = device.getProperties();
                    mApiMajor = (uint16_t)VK_VERSION_MAJOR(pdp.apiVersion);
                    mApiMinor = (uint16_t)VK_VERSION_MINOR(pdp.apiVersion);
                    mApiPatch = (uint16_t)VK_VERSION_PATCH(pdp.apiVersion);
					
					uint16_t x, y, z, a;
					uint32_t b;
					x = (uint16_t)VK_VERSION_MAJOR(pdp.apiVersion);
					y = (uint16_t)VK_VERSION_MINOR(pdp.apiVersion);
					z = (uint16_t)VK_VERSION_PATCH(pdp.apiVersion);
					a = (uint16_t)VK_VERSION_MAJOR(pdp.driverVersion);
					b = VK_VERSION_MINOR(pdp.driverVersion) | VK_VERSION_PATCH(pdp.driverVersion);
					LOG_INFO("Chosen <{}> {} with supported API {}.{}.{} and driver version {}.{}\n",
						vk::to_string(pdp.deviceType),
						pdp.deviceName,
						x, y, z,
						a, b);
                    
					break;
                }
            }
        }

        assert(mPhysicalDevice.operator!=(nullptr));
    }

    void Device::FindQueueFamilies()
    {
		std::vector<vk::QueueFamilyProperties> queueFamilies = mPhysicalDevice.getQueueFamilyProperties();
		assert(queueFamilies.size() > 0);

        for (size_t i = 0; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueCount > 0 &&
                HAS_STATE(queueFamilies[i].queueFlags, vk::QueueFlagBits::eGraphics))
            {
                mQueueFamilyIndex.Graphics = i;
            }

            if (queueFamilies[i].queueCount > 0 &&
                mPhysicalDevice.getSurfaceSupportKHR(i, g_Instance.mSurface))
            {
                mQueueFamilyIndex.Presentation = i;
            }

            if (mQueueFamilyIndex.IsComplete()) break;
        }

        assert(mQueueFamilyIndex.IsComplete());
    }
    
    void Device::CreateLogicalDevice()
    {
        std::vector<vk::DeviceQueueCreateInfo> queueInfo;
        queueInfo.reserve(2);

        if (GRAPHICS_FAMILY_INDEX == PRESENTATION_FAMILY_INDEX)
        {
            float queuePriorities[] = { 1.f, 1.f, 1.f, 1.f };

            uint32_t uniqueQueueFamilies = mQueueFamilyIndex.Graphics;
            uint32_t queueCount = 4;

            vk::DeviceQueueCreateInfo qinfo({}, uniqueQueueFamilies, queueCount, queuePriorities);
            queueInfo.push_back(qinfo);
        }
        else
        {
            float queuePriorities[] = { 1.f, 1.f, 1.f, 1.f };

            uint32_t uniqueQueueFamilies[] = { mQueueFamilyIndex.Graphics, mQueueFamilyIndex.Presentation };
            uint32_t queueCount[] = { 4, 1 };

            vk::DeviceQueueCreateInfo qinfo({}, uniqueQueueFamilies[0], queueCount[0], queuePriorities);
            queueInfo.push_back(qinfo);

            vk::DeviceQueueCreateInfo qinfo2({}, uniqueQueueFamilies[1], queueCount[1], &queuePriorities[0]);
            queueInfo.push_back(qinfo2);
        }

        vk::PhysicalDeviceFeatures pdf;
        pdf.samplerAnisotropy = VK_TRUE;
        pdf.geometryShader = VK_TRUE;
#ifdef _DEBUG
        vk::DeviceCreateInfo devInfo({},
            queueInfo.size(),
            queueInfo.data(),
            static_cast<uint32_t>(g_Instance.mValidationLayers.size()),
            g_Instance.mValidationLayers.data(),
            static_cast<uint32_t>(mDeviceExtensions.size()),
            mDeviceExtensions.data(),
            &pdf);
#else
        vk::DeviceCreateInfo devInfo({},
            queueInfo.size(),
            queueInfo.data(),
            0,
            nullptr,
            static_cast<uint32_t>(mDeviceExtensions.size()),
            mDeviceExtensions.data(),
            &pdf);
#endif
        mDevice = mPhysicalDevice.createDevice(devInfo);

        // Get graphics, presentation and transfer (buffer/texture) queue handles
        mQueue = {
            mDevice.getQueue(mQueueFamilyIndex.Graphics, 0),
            mDevice.getQueue(mQueueFamilyIndex.Presentation, 0),
            mDevice.getQueue(mQueueFamilyIndex.Graphics, 1),
            mDevice.getQueue(mQueueFamilyIndex.Graphics, 2),
            mDevice.getQueue(mQueueFamilyIndex.Graphics, 3),
        };
    }


    bool Device::IsDeviceSuitable(const vk::PhysicalDevice & pd, vk::PhysicalDeviceLimits & limits)
    {
        vk::PhysicalDeviceProperties pdp;
        vk::PhysicalDeviceFeatures pdf;
        pd.getProperties(&pdp);
        pd.getFeatures(&pdf);

#ifdef _DEBUG
        uint16_t x, y, z, a;
        uint32_t b;
        x = (uint16_t)VK_VERSION_MAJOR(pdp.apiVersion);
        y = (uint16_t)VK_VERSION_MINOR(pdp.apiVersion);
        z = (uint16_t)VK_VERSION_PATCH(pdp.apiVersion);
        a = (uint16_t)VK_VERSION_MAJOR(pdp.driverVersion);
        b = VK_VERSION_MINOR(pdp.driverVersion) | VK_VERSION_PATCH(pdp.driverVersion);

        LOG_INFO("Found <{}> {} with supported API {}.{}.{} and driver version {}.{}\n",
            vk::to_string(pdp.deviceType),
            pdp.deviceName,
            x, y, z,
            a, b);
#endif
        limits = pdp.limits;
        return pdp.deviceType == 
            vk::PhysicalDeviceType::eDiscreteGpu &&
            pdf.geometryShader &&
            pdf.samplerAnisotropy;
    }
    
    bool Device::CheckDeviceExtensionSupport(const vk::PhysicalDevice & pd, const StringList & extensions)
    {
        std::vector<vk::ExtensionProperties> availableExtensions = pd.enumerateDeviceExtensionProperties();
        int count = 0;
        for (auto exname : extensions)
        {
            for (auto available : availableExtensions)
            {
                if (strcmp(exname, available.extensionName) == 0)
                {
                    count++;
                    break;
                }
            }
        }

        return count == extensions.size();
    }
}
