#pragma once

#include <Common\Constants.h>
#include <RenderPass\RenderPass.h>
#include <string>
#include <unordered_map>

#define GRenderpassManager Engine::g_RenderpassManager
#define RP Engine::RPConst

namespace Engine
{
	// Render pass constant names
	namespace RPConst
	{
		static constexpr const char* FRAME = "framePass";
		static constexpr const char* SKY = "skyPass";
	}

    class RenderpassManager
    {
        static constexpr uint32_t RENDERPASS_CAPACITY = 2;

    public:
        void Init();
		void PostShaderLoadInit();
		void PostSwapchainInit();
        void Destroy();

        template<typename T>
        RenderPass* AddPass(const std::string& name)
        {
			THROW_IF(mPassMap.find(name) != mPassMap.end(), "RenderPass already added: {0}", name.c_str());

            RenderPass* pass = T::Allocate();
            pass->Init();
            mPass.push_back(pass);
			mPassMap[name] = pass;
            return pass;
        }
   
		template<typename T = RenderPass>
        T* GetPassAt(uint32_t index) const { return reinterpret_cast<T*>(mPass[index]); }
		
		template<typename T = RenderPass>
		T* GetPass(const std::string& name)
		{
			THROW_IF(mPassMap.find(name) == mPassMap.end(), "RenderPass {0} is does not exist", name.c_str());
			return reinterpret_cast<T*>(mPassMap[name]);
		}

		vk::Fence GetFenceAt(uint32_t index) const { return mFence[index]; }

		void InitPasses();
		void SetupPasses();
		void RenderPasses(vk::Semaphore& waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore& signalSem);

    private:
		void CreateFences();
		void DestroyFences();

        std::vector<RenderPass*> mPass;
        std::unordered_map<std::string, RenderPass*> mPassMap;
		std::vector<vk::Fence> mFence;
    };

    extern RenderpassManager g_RenderpassManager;
}