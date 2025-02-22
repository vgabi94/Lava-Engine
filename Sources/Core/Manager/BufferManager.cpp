#define VMA_IMPLEMENTATION
#include "BufferManager.h"
#include <Engine\Device.h>
#include <Engine\TaskScheduler.h>

namespace Engine
{
    BufferManager g_BufferManager;

    void BufferManager::Init()
    {
        mBuffer.reserve(BUFFER_INIT_CAPACITY);
        mBufferAllocation.reserve(BUFFER_INIT_CAPACITY);

        mStagBuffer.reserve(BUFFER_INIT_CAPACITY);
        mStagBuffer.reserve(BUFFER_INIT_CAPACITY);

        InitVmaAllocator();
        CreateCopyPool();
        mBufferFence = GDevice.CreateFence();
		mUIFence = GDevice.CreateFence();
        
        LOG_INFO("[LOG] BufferManager Init\n");
    }

    void BufferManager::Destroy()
    {
        DestroyBuffers();
        vmaDestroyAllocator(mAllocator);
        g_vkDevice.destroyCommandPool(mCopyPool);
        g_vkDevice.destroyFence(mBufferFence);
		g_vkDevice.destroyFence(mUIFence);
        
        LOG_INFO("[LOG] BufferManager Destroy\n");
    }

	void BufferManager::CreateCopyPool()
    {
        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eTransient,
            GRAPHICS_FAMILY_INDEX);
        mCopyPool = g_vkDevice.createCommandPool(poolInfo);
    }

    void BufferManager::InitVmaAllocator()
    {
        VmaAllocatorCreateInfo info = {};
        info.device = g_vkDevice;
        info.physicalDevice = g_vkPhysicalDevice;

        assert(vmaCreateAllocator(&info, &mAllocator) == VK_SUCCESS);
    }

    void BufferManager::ExecuteOperations()
    {
        if (!mCopyRequest.empty())
        {
            vk::CommandBufferAllocateInfo cmdBuffAllocateInfo(mCopyPool, vk::CommandBufferLevel::ePrimary, 1);
            mCopyCmdBuffer = g_vkDevice.allocateCommandBuffers(cmdBuffAllocateInfo)[0];
            vk::CommandBufferBeginInfo cmdBuffBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

            mCopyCmdBuffer.begin(cmdBuffBeginInfo);

            while (!mCopyRequest.empty())
            {
                CopyRequest& req = mCopyRequest.front();

                vk::DeviceSize size = mBufferAllocation[req.bufIndex]->GetSize();
                vk::BufferCopy bufCopy(0, 0, size);
                mCopyCmdBuffer.copyBuffer(mStagBuffer[req.stagIndex], mBuffer[req.bufIndex], 1, &bufCopy);

                mCopyRequest.pop();
            }

            mCopyCmdBuffer.end();

            vk::SubmitInfo subInfo(0, nullptr, nullptr, 1, &mCopyCmdBuffer, 0, nullptr);
            BUFFER_TRANSFER_QUEUE.submit(1, &subInfo, mBufferFence);

            LOG_INFO("[LOG] BufferManager start upload\n");

            GDevice.WaitForFence(mBufferFence);
            GDevice.ResetFence(mBufferFence);
            
            g_vkDevice.freeCommandBuffers(mCopyPool, 1, &mCopyCmdBuffer);
            DestroyStagingBuffers();

            LOG_INFO("[LOG] BufferManager end upload\n");
        }
    }

    vk::Buffer BufferManager::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsageflags,
        VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
        VmaAllocation& vmaAllocation, VmaAllocationInfo* vmaAllocationInfo)
    {
        vk::Buffer buffer;
        vk::BufferCreateInfo bufInfo({},
            size,
            bufferUsageflags,
            vk::SharingMode::eExclusive);

        VmaAllocationCreateInfo bufAllocCreateInfo = {};
        bufAllocCreateInfo.usage = vmaMemoryUsage;
        bufAllocCreateInfo.flags = vmaAllocationFlags;
        VkBuffer rawBuf;

        assert(
            vmaCreateBuffer(
                mAllocator,
                &(VkBufferCreateInfo)bufInfo,
                &bufAllocCreateInfo, &rawBuf,
                &vmaAllocation,
                vmaAllocationInfo) == VK_SUCCESS
        );

        buffer = rawBuf;
        return buffer;
    }

    void BufferManager::DestroyStagingBuffers()
    {
        for (size_t i = 0; i < mStagBufferAllocation.size(); i++)
        {
            vmaDestroyBuffer(mAllocator, mStagBuffer[i], mStagBufferAllocation[i]);
        }
        mStagBuffer.clear();
        mStagBufferAllocation.clear();
    }

    void BufferManager::DestroyBuffers()
    {
        for (size_t i = 0; i < mBuffer.size(); i++)
        {
            vmaDestroyBuffer(mAllocator, mBuffer[i], mBufferAllocation[i]);
        }
    }
}
