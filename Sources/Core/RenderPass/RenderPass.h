#pragma once

#include <vulkan\vulkan.hpp>

namespace Engine
{
	// Abstract base class for render passes
	class RenderPass
	{
	public:
		virtual void Init();
		virtual void PostShaderLoadInit() { };
		virtual void Destroy();

		virtual void Recreate() { };

		virtual void Setup() { }
		virtual vk::SubmitInfo GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStages, vk::Semaphore& signalSem) = 0;

		vk::RenderPass GetVkObject() const { return mRenderPass; }
		vk::Framebuffer FramebufferAt(uint32_t index) const { return mFramebuffer[index]; }
		vk::Semaphore GetSemaphore() const { return mSemaphore; }
		vk::PipelineStageFlags GetPipelineStage() const { return mPipelineStageFlags; }

	protected:
		vk::RenderPass mRenderPass;
		vk::CommandPool mCommandPool;
		std::vector<vk::Framebuffer> mFramebuffer;
		std::vector<vk::CommandBuffer> mCommandBuffer;
		vk::Semaphore mSemaphore;
		vk::PipelineStageFlags mPipelineStageFlags;
		std::vector<vk::PipelineStageFlags> mWaitStages;
	};
}
