#include "RenderpassManager.h"
#include <Engine\Swapchain.h>
#include <Engine\Device.h>
#include <RenderPass\SkyPass.h>
#include <RenderPass\PrenvPass.h>
#include <RenderPass\BrdfPass.h>
#include <RenderPass\UIRenderPass.h>

namespace Engine
{
	RenderpassManager g_RenderpassManager;

    void RenderpassManager::Init()
    {
        mPass.reserve(RENDERPASS_CAPACITY);
        mPassTask.reserve(RENDERPASS_CAPACITY);
		mPassMap.reserve(RENDERPASS_CAPACITY);
		mPostShader = false;
        LOG_INFO("[LOG] RenderpassManager init\n");
    }

	void RenderpassManager::PostShaderLoadInit()
	{
		for (auto pass : mPass)
		{
			pass->PostShaderLoadInit();
		}
		for (auto pass : mPassTask)
		{
			pass->PostShaderLoadInit();
		}
		mPostShader = true;
	}

	void RenderpassManager::PostSwapchainInit()
	{
		CreateFences();
		LOG_INFO("[LOG] RenderpassManager PostSwapchain init\n");
	}

    void RenderpassManager::Destroy()
    {
        for (auto pass : mPass)
        {
            pass->Destroy();
        }
		for (auto pass : mPassTask)
		{
			pass->Destroy();
		}
		DestroyFences();
        LOG_INFO("[LOG] RenderpassManager destroy\n");
    }
	
	void RenderpassManager::InitPasses()
	{
		AddPass<Engine::FramePass>(RPConst::FRAME);
		AddPass<Engine::UIRenderPass>(RPConst::UI);
		AddPass<Engine::SkyPass>(RPConst::SKY);

		uint32_t unused;
		AddPassTask<Engine::PrenvPass>(unused, RPConst::PRENV);
		AddPassTask<Engine::BrdfPass>(unused, RPConst::BRDF);
		LOG_INFO("[LOG] Render passes init\n");
	}

	void RenderpassManager::SetupPasses()
	{
		for (auto pass : mPass)
		{
			pass->Setup();
		}
	}
	
	void RenderpassManager::RenderPasses(vk::Semaphore & waitSem, vk::PipelineStageFlags waitStage, vk::Semaphore & signalSem)
	{
		THROW_IF(mPass.empty(), "Render passes not initialized!");
		std::vector<vk::SubmitInfo> submitInfos;
		submitInfos.reserve(mPass.size());
		
		if (mPass.size() == 1)
		{
			submitInfos.push_back(mPass[0]->GetSubmitInfo(waitSem, waitStage, signalSem));
		}
		else
		{
			submitInfos.push_back(mPass[0]->GetSubmitInfo(waitSem, waitStage, mPass[0]->GetSemaphore()));

			for (size_t i = 1; i < mPass.size() - 1; i++)
			{
				submitInfos.push_back(
					mPass[i]->GetSubmitInfo(
						mPass[i - 1]->GetSemaphore(),
						mPass[i - 1]->GetPipelineStage(),
						mPass[i]->GetSemaphore()
					)
				);
			}

			const uint32_t last = mPass.size() - 1;
			submitInfos.push_back(
				mPass[last]->GetSubmitInfo(
					mPass[last - 1]->GetSemaphore(),
					mPass[last - 1]->GetPipelineStage(),
					signalSem
				)
			);
		}

		GRAPHICS_QUEUE.submit(submitInfos, mFence[GSwapchain.GetCurrentFrameIndex()]);
	}

	void RenderpassManager::CreateFences()
	{
		uint32_t size = GSwapchain.GetImageCount();
		mFence.resize(size);
		vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

		for (size_t i = 0; i < size; i++)
		{
			mFence[i] = g_vkDevice.createFence(fenceInfo);
		}
	}
	
	void RenderpassManager::DestroyFences()
	{
		for (auto& f : mFence)
		{
			g_vkDevice.destroyFence(f);
		}
	}
}

