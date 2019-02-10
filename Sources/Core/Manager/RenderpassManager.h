#pragma once

#include <vulkan\vulkan.hpp>

#define GRenderpassManager Engine::g_RenderpassManager

namespace Engine
{
    class RenderPass
    {
    public:
        virtual void Init() {}
        virtual void Destroy() {}

        virtual void Recreate() {};

        virtual void Setup() {}
        virtual void Render() = 0;

        virtual void Wait() {}

        vk::RenderPass GetVkObject() const { return mRenderPass; }
        vk::Framebuffer FramebufferAt(uint32_t index) const { return mFramebuffer[index]; }

    protected:
        vk::RenderPass mRenderPass;
        vk::CommandPool mCommandPool;
        std::vector<vk::Framebuffer> mFramebuffer;
        std::vector<vk::CommandBuffer> mCommandBuffer;
    };

    class RenderpassManager
    {
        static constexpr uint32_t RENDERPASS_CAPACITY = 2;

    public:
        void Init();
        void Destroy();

        template<typename T>
        RenderPass* AddPass()
        {
            RenderPass* pass = T::Allocate();
            pass->Init();
            mPass.push_back(pass);
            return pass;
        }
   
        RenderPass* GetPassAt(uint32_t index) const { return mPass[index]; }

    private:
        std::vector<RenderPass*> mPass;
    };

    extern RenderpassManager g_RenderpassManager;
}