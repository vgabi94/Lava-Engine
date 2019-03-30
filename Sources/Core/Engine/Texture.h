#pragma once
#include <vulkan\vulkan.hpp>
#include <vector>
#include <vk_mem_alloc.h>

namespace Engine
{
	struct Texture
	{
		friend class TextureManager;

		uint32_t mWidth, mHeight;
		uint32_t mDepth;
		uint32_t mChannels;
		vk::Image mImage;
		vk::Sampler mSampler;
		vk::ImageView mImageView;

		void Destroy();

		void* GetMappedBit() { if (mVmaAllocationInfo) return mVmaAllocationInfo->pMappedData; else return nullptr; }

	private:
		VmaAllocation mImageAllocation;
		VmaAllocationInfo* mVmaAllocationInfo;
	};
}