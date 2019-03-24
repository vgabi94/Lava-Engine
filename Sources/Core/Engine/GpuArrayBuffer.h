#pragma once
#include <Manager\BufferManager.h>
#include <vector>

namespace Engine
{
	template<typename T>
	class GpuArrayBuffer
	{
		static constexpr uint32_t INIT_CAPACITY = 2;
	public:
		GpuArrayBuffer(vk::BufferUsageFlags flags = vk::BufferUsageFlagBits::eUniformBuffer);

		void Init() { /* buffer is initialized in Commit() method. */ }
		void Destroy() { vmaDestroyBuffer(GVmaAllocator, mBuffer, mVmaAllocation); }

		vk::Buffer GetBuffer() const
		{
			THROW_IF(mDirty || mSoftDirty, "Can't return buffer without commited data!");
			return mBuffer;
		}

		void Add(T elem) { mVectorBuffer.push_back(elem); mDirty = true; }
		void RemoveAt(uint32_t index) { mVectorBuffer.erase(mVectorBuffer.begin() + index); mDirty = true; }
		void Clear() { mVectorBuffer.clear(); mDirty = true; }

		size_t Size() const { return mVectorBuffer.size(); }
		void Resize(size_t newSize) { mVectorBuffer.resize(newSize); }
		void Reserve(size_t newSize) { mVectorBuffer.reserve(newSize); }

		T& operator[](uint32_t i) { mSoftDirty = true; return mVectorBuffer[i]; }

		// Send data to the GPU.
		void Commit();

	private:
		void CreateMappedGPUBuffer();
		void UpdateBuffer();

		vk::Buffer mBuffer;
		vk::BufferUsageFlags mFlags;
		VmaAllocation mVmaAllocation;
		VmaAllocationInfo mVmaAllocInfo;
		std::vector<T> mVectorBuffer;
		bool mDirty, mSoftDirty;
	};
	
	template<typename T>
	inline GpuArrayBuffer<T>::GpuArrayBuffer(vk::BufferUsageFlags flags)
		: mDirty(true), mSoftDirty(true), mFlags(flags), mBuffer(nullptr)
	{
		mVectorBuffer.reserve(INIT_CAPACITY);
	}
	
	template<typename T>
	void GpuArrayBuffer<T>::CreateMappedGPUBuffer()
	{
		if (mBuffer)
			vmaDestroyBuffer(GVmaAllocator, mBuffer, mVmaAllocation);
		vk::DeviceSize size = mVectorBuffer.size() * sizeof(T);

		mBuffer = g_BufferManager.CreateBuffer(size, mFlags, VMA_MEMORY_USAGE_CPU_ONLY,
			VMA_ALLOCATION_CREATE_MAPPED_BIT,
			mVmaAllocation, &mVmaAllocInfo);
	}

	template<typename T>
	void GpuArrayBuffer<T>::UpdateBuffer()
	{
		assert(!mVectorBuffer.empty());
		memcpy(mVmaAllocInfo.pMappedData, mVectorBuffer.data(), mVectorBuffer.size() * sizeof(T));
	}

	template<typename T>
	void GpuArrayBuffer<T>::Commit()
	{
		if (mDirty)
		{
			CreateMappedGPUBuffer();
			UpdateBuffer();
			mDirty = false;
			mSoftDirty = false;
		}
		else if (mSoftDirty)
		{
			UpdateBuffer();
			mSoftDirty = false;
		}
	}
}
