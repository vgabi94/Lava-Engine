#pragma once

#include <vulkan\vulkan.hpp>

#define GRenderpassManager Engine::g_RenderpassManager
#define PassIndex Engine::RenderPassIndex

namespace Engine
{
	namespace RenderPassIndex
	{
		static constexpr uint32_t FRAME = 0;
	}

    class RenderPass
    {
    public:
		virtual void Init();
		virtual void Destroy();

        virtual void Recreate() {};

        virtual void Setup() {}
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

    class RenderpassManager
    {
        static constexpr uint32_t RENDERPASS_CAPACITY = 2;

    public:
        void Init();
		void PostSwapchainInit();
        void Destroy();

        template<typename T>
        RenderPass* AddPass()
        {
            RenderPass* pass = T::Allocate();
            pass->Init();
            mPass.push_back(pass);
            return pass;
        }
   
		template<typename T = RenderPass>
        T* GetPassAt(uint32_t index) const { return reinterpret_cast<T*>(mPass[index]); }

		vk::Fence GetFenceAt(uint32_t index) const { return mFence[index]; }

		void InitPasses();
		void SetupPasses();
		void RenderPasses(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore& signalSem);

    private:
		void CreateFences();
		void DestroyFences();

        std::vector<RenderPass*> mPass;
		std::vector<vk::Fence> mFence;
    };

    extern RenderpassManager g_RenderpassManager;
}