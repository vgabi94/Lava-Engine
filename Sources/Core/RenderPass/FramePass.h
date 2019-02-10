#pragma once

#include <Manager\RenderpassManager.h>
#include <MemoryPool.h>
#include <Common\Constants.h>
#include <vk_mem_alloc.h>
#include <array>

namespace Engine
{
    class FramePass : public RenderPass
    {
    public:
        void Init() override;
        void Destroy() override;

        void Recreate() override;

        void Setup() override;
        void Render() override;

        void Wait() override;

        void CreateSubmitInfo(vk::Semaphore& imageSem, vk::Semaphore& renderSem);

        MEM_POOL_DECLARE(FramePass);

    private:
        void _Destroy();

        void CreateRenderPass();
        void CreateDepthResources();
        void CreateFences();
        void CreateFramebuffers();
        void CreateCommandPoolAndBuffer();
        void RecordCommandBuffer(uint32_t imageIndex);

        void DestroyRenderPass();
        void DestroyFences();
        void DestroyFramebuffers();
        void DestroyCommandPool();
        void DestroyDepthResources();

        std::vector<vk::Fence> mFence;

        vk::SubmitInfo mSubmitInfo;
        std::array<vk::PipelineStageFlags, 1> mWaitStages;

        vk::Image mDepthImage;
        vk::Format mDepthFormat;
        vk::ImageView mDepthImageView;
        VmaAllocation mDepthAlloc;
    };
}