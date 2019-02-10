#include "Time.h"
#include <GLFW\glfw3.h>

namespace Engine
{
    Time g_Time;

    void Time::SetLastTime()
    {
        lastTime = glfwGetTime();
    }

    void Time::Update()
    {
        if (firstUpdate)
        {
            double t = glfwGetTime();
            deltaTime = 0.0f;
            time = static_cast<float>(t);
            lastTime = t;
            firstUpdate = false;
        }
        else
        {
            double t = glfwGetTime();
            deltaTime = static_cast<float>(t - lastTime);
            time = static_cast<float>(t);
            lastTime = t;
        }
    }
    
    void Time::UpdateFixed()
    {
        fixedTime += GetFixedDeltaTime();
    }

    double Time::TimeNow() const
    {
        return static_cast<float>(glfwGetTime());
    }
}

/* EXPORTED INTERFACE */
LAVA_API float GetTime()
{
    return Engine::g_Time.time;
}

LAVA_API float GetFixedTime()
{
    return Engine::g_Time.fixedTime;
}

LAVA_API float GetTimeNow()
{
    return Engine::g_Time.TimeNow();
}

LAVA_API float GetFixedDeltaTime()
{
    return TIME_STEP;
}

LAVA_API float GetDeltaTime()
{
    return Engine::g_Time.deltaTime;
}

LAVA_API void SetTimeScale(float scale)
{
    scale = scale > 1.f ? 1.f : scale;
    scale = scale < 0.f ? 0.f : scale;
    Engine::g_Time.timeScale = scale;
}

LAVA_API float GetTimeScale()
{
    return Engine::g_Time.timeScale;
}