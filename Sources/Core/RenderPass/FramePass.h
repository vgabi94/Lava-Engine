#pragma once

#include <Manager\RenderpassManager.h>
#include <MemoryPool.h>
#include <Common\Constants.h>
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

        vk::SubmitInfo GetSubmitInfo(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStages, vk::Semaphore& signalSem) override;

        MEM_POOL_DECLARE(FramePass);

    private:
        void _Destroy();

        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateCommandPoolAndBuffer();
        void RecordCommandBuffer(uint32_t imageIndex);

        void DestroyRenderPass();
        void DestroyFramebuffers();
        void DestroyCommandPool();
    };
}