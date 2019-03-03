#include "RenderPass.h"
#include <Engine\Device.h>

namespace Engine
{
	void RenderPass::Init()
	{
		mSemaphore = GDevice.CreateSemaphore();
	}

	void RenderPass::Destroy()
	{
		g_vkDevice.destroySemaphore(mSemaphore);
	}
}
