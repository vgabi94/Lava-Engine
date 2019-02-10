#include "RenderpassManager.h"
#include <Engine\Swapchain.h>

namespace Engine
{
    RenderpassManager g_RenderpassManager;

    void RenderpassManager::Init()
    {
        mPass.reserve(RENDERPASS_CAPACITY);
        LOG_INFO("[LOG] RenderpassManager init\n");
    }

    void RenderpassManager::Destroy()
    {
        for (auto pass : mPass)
        {
            pass->Destroy();
        }
        LOG_INFO("[LOG] RenderpassManager destroy\n");
    }
}

