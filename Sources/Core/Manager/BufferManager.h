#pragma once
#include <Common\Constants.h>
#include <Common\VertexDataTypes.h>
#include <vk_mem_alloc.h>
#include <queue>
#include <utility>

#define GBufferManager Engine::g_BufferManager
#define CreateVertexIndexBuffer(v, i) g_BufferManager.Allocate(v, i, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer)
#define CreateVertexBuffer(v) g_BufferManager.Allocate(v, vk::BufferUsageFlagBits::eVertexBuffer)
#define BufferAt(i) g_BufferManager.GetBuffer(i)
#define GVmaAllocator g_BufferManager.mAllocator

namespace Engine
{
    typedef std::pair<uint32_t, uint32_t> IndexOffset;

    class BufferManager
    {
        static constexpr uint32_t BUFFER_INIT_CAPACITY = 16;

    public:
        typedef void(*UploadCompleteCback)(void*);

        void Init();
        void Destroy();

        void ExecuteOperations();

        vk::Buffer GetBuffer(uint32_t index) const
        {
            assert(index < mBuffer.size());
            return mBuffer[index];
        }

        // Creates an empty buffer. NOT registered by the manager.
        // User must deallocate the buffer manually when no longer used!
        vk::Buffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsageflags,
            VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationFlags,
            VmaAllocation& vmaAllocation, VmaAllocationInfo* vmaAllocationInfo);

        // ---- Allocation function for indexed vertex buffer ---- //
        template<typename T, typename U>
        IndexOffset Allocate(const std::vector<T>& data1, const std::vector<U>& data2,
            vk::BufferUsageFlags flags)
        {
            vk::Buffer dataBuffer, stagingBuffer;
            IndexOffset indexOffset;

            // 1. Allocate the staging buffers
            indexOffset.second = sizeof(T) * data1.size();
            vk::DeviceSize size = indexOffset.second + sizeof(U) * data2.size();
			assert(size > 0);

            VmaAllocation stagAllocation;
            VmaAllocationInfo stagAllocInfo;
            stagingBuffer = CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
                VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT,
                stagAllocation, &stagAllocInfo);

            // 2. Copy the contents from vertices and indices to the staging buffers
            memcpy(stagAllocInfo.pMappedData, data1.data(), indexOffset.second);
            // Assume byte level
            memcpy((char*)stagAllocInfo.pMappedData + indexOffset.second, data2.data(),
                size  - indexOffset.second);

            // 3. Create the local device buffer
            VmaAllocation bufAllocation;
            dataBuffer = CreateBuffer(size, vk::BufferUsageFlagBits::eTransferDst | flags,
                VMA_MEMORY_USAGE_GPU_ONLY, 0, bufAllocation, nullptr);

            // 4. Update global arrays and create copy request
            uint32_t bufIndex = (uint32_t)mBuffer.size();
            mBuffer.push_back(dataBuffer);
            mBufferAllocation.push_back(bufAllocation);

            uint32_t stagIndex = (uint32_t)mStagBuffer.size();
            mStagBuffer.push_back(stagingBuffer);
            mStagBufferAllocation.push_back(stagAllocation);

            CopyRequest req{ bufIndex, stagIndex };
            mCopyRequest.push(req);

            indexOffset.first = bufIndex;

            return indexOffset;
        }

		// ---- Allocation function for vertex buffer ---- //
		template<typename T>
		uint32_t Allocate(const std::vector<T>& data, vk::BufferUsageFlags flags)
		{
			vk::Buffer dataBuffer, stagingBuffer;

			// 1. Allocate the staging buffers
			vk::DeviceSize size = sizeof(T) * data.size();
			assert(size > 0);

			VmaAllocation stagAllocation;
			VmaAllocationInfo stagAllocInfo;
			stagingBuffer = CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
				VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT,
				stagAllocation, &stagAllocInfo);

			// 2. Copy the contents from data to the staging buffers
			memcpy(stagAllocInfo.pMappedData, data.data(), size);

			// 3. Create the local device buffer
			VmaAllocation bufAllocation;
			dataBuffer = CreateBuffer(size, vk::BufferUsageFlagBits::eTransferDst | flags,
				VMA_MEMORY_USAGE_GPU_ONLY, 0, bufAllocation, nullptr);

			// 4. Update global arrays and create copy request
			uint32_t bufIndex = (uint32_t)mBuffer.size();
			mBuffer.push_back(dataBuffer);
			mBufferAllocation.push_back(bufAllocation);

			uint32_t stagIndex = (uint32_t)mStagBuffer.size();
			mStagBuffer.push_back(stagingBuffer);
			mStagBufferAllocation.push_back(stagAllocation);

			CopyRequest req{ bufIndex, stagIndex };
			mCopyRequest.push(req);

			return bufIndex;
		}

        VmaAllocator mAllocator;
        vk::Fence mBufferFence;
    private:
        struct CopyRequest
        {
            uint32_t bufIndex, stagIndex;
        };

        typedef std::vector<vk::Buffer> BufferVector;
        typedef std::vector<VmaAllocation> AllocationVector;

        vk::CommandPool mCopyPool;
        vk::CommandBuffer mCopyCmdBuffer;

        BufferVector mBuffer;
        AllocationVector mBufferAllocation;
        BufferVector mStagBuffer;
        AllocationVector mStagBufferAllocation;

        std::queue<CopyRequest> mCopyRequest;

        void CreateCopyPool();
        void InitVmaAllocator();
        
        void DestroyStagingBuffers();
        void DestroyBuffers();
    };

    extern BufferManager g_BufferManager;
}