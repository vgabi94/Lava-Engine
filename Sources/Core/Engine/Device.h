#pragma once

#include "Instance.h"
#include <array>

#define GRAPHICS_FAMILY_INDEX Vulkan::g_Device.mQueueFamilyIndex.Graphics
#define PRESENTATION_FAMILY_INDEX Vulkan::g_Device.mQueueFamilyIndex.Presentation

#define GRAPHICS_QUEUE Vulkan::g_Device.GetQueue(0)
#define PRESENTATION_QUEUE Vulkan::g_Device.GetQueue(1)
#define BUFFER_TRANSFER_QUEUE Vulkan::g_Device.GetQueue(2)
#define TEXTURE_TRANSFER_QUEUE Vulkan::g_Device.GetQueue(3)
#define LAYOUT_TRANSITION_QUEUE Vulkan::g_Device.GetQueue(4)

#define g_vkDevice ((vk::Device)Vulkan::g_Device)
#define g_vkPhysicalDevice ((vk::PhysicalDevice)Vulkan::g_Device.GetPhysicalDevice())

#define GDevice Vulkan::g_Device

namespace Vulkan
{
    struct QueueFamilyIndex
    {
        uint32_t Graphics = (uint32_t)-1;
        uint32_t Presentation = (uint32_t)-1;

        bool IsComplete() { return Graphics != (uint32_t)-1 && Presentation != (uint32_t)-1; }
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };


    // ----------------------------- DEVICE -------------------------------- //
    class Device
    {
        friend class Swapchain;

    public:
        uint16_t mApiMajor, mApiMinor, mApiPatch;
        QueueFamilyIndex mQueueFamilyIndex;

        operator vk::Device() const { return mDevice; }

        vk::PhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
        vk::Queue GetQueue(uint32_t index) { return mQueue[index]; }
        
        vk::Semaphore CreateSemaphore()
        {
            vk::SemaphoreCreateInfo createInfo;
            return mDevice.createSemaphore(createInfo);
        }

        vk::Fence CreateFence()
        {
            vk::FenceCreateInfo createInfo;
            return mDevice.createFence(createInfo);
        }

        void WaitForFence(vk::Fence fence)
        {
            mDevice.waitForFences(1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        }

        void ResetFence(vk::Fence fence)
        {
            mDevice.resetFences(1, &fence);
        }

        void Init();
        void Destroy();

        const SwapChainSupportDetails& GetSwapChainSupportDetails() const { return mSCSD; }

    private:
        vk::PhysicalDevice mPhysicalDevice;
        vk::PhysicalDeviceLimits mPhysicalDeviceLimits;
        vk::Device mDevice;
        SwapChainSupportDetails mSCSD;
        StringList mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        
        std::array<vk::Queue, 5> mQueue;

        SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice, vk::SurfaceKHR);
        
        void PickPhysicalDevice();
        void FindQueueFamilies();
        void CreateLogicalDevice();

        bool IsDeviceSuitable(const vk::PhysicalDevice& pd, vk::PhysicalDeviceLimits& limits);
        bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& pd, const StringList& extensions);
    };

    extern Device g_Device;
}