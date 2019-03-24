#pragma once
#include <Manager\BufferManager.h>

namespace Engine
{
	template<typename T>
	class GpuBuffer
	{
	public:
		GpuBuffer(vk::BufferUsageFlags flags = vk::BufferUsageFlagBits::eUniformBuffer);

		void Init() { CreateMappedGPUBuffer(); }
		void Destroy() { vmaDestroyBuffer(GVmaAllocator, mBuffer, mVmaAllocation); }

		vk::Buffer GetBuffer() const
		{
			THROW_IF(mDirty, "Can't return buffer without commited data!");
			return mBuffer;
		}

		T& Get() { mDirty = true; return mData; }

		// Send data to the GPU.
		void Commit();

	private:
		void CreateMappedGPUBuffer();
		void UpdateBuffer();

		vk::Buffer mBuffer;
		vk::BufferUsageFlags mFlags;
		VmaAllocation mVmaAllocation;
		VmaAllocationInfo mVmaAllocInfo;
		T mData;

		bool mDirty;
	};


	template<typename T>
	inline GpuBuffer<T>::GpuBuffer(vk::BufferUsageFlags flags)
		: mDirty(true), mFlags(flags), mBuffer(nullptr)
	{
	}

	template<typename T>
	inline void GpuBuffer<T>::Commit()
	{
		if (mDirty)
		{
			UpdateBuffer();
			mDirty = false;
		}
	}

	template<typename T>
	inline void GpuBuffer<T>::CreateMappedGPUBuffer()
	{
		if (mBuffer)
			vmaDestroyBuffer(GVmaAllocator, mBuffer, mVmaAllocation);
		vk::DeviceSize size = sizeof(T);

		mBuffer = g_BufferManager.CreateBuffer(size, mFlags, VMA_MEMORY_USAGE_CPU_ONLY,
			VMA_ALLOCATION_CREATE_MAPPED_BIT,
			mVmaAllocation, &mVmaAllocInfo);
	}

	template<typename T>
	inline void GpuBuffer<T>::UpdateBuffer()
	{
		memcpy(mVmaAllocInfo.pMappedData, &mData, sizeof(T));
	}
}