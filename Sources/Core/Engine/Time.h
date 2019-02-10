#pragma once

#include <Common\Constants.h>

namespace Engine
{
    struct Time
    {
        float deltaTime = 0.f;
        float time = 0.f;
        float fixedTime = 0.f;
        float timeScale = 1.f;

        double lastTime = 0.0;

        void Update();
        void UpdateFixed();
        double TimeNow() const;

        void SetLastTime();

    private:
        bool firstUpdate = true;
    };

    extern Time g_Time;
}

/* EXPORTED INTERFACE */
extern "C"
{
    /// <summary>
    /// Returns the unscaled time point at the begining of this frame.
    /// </summary>
    LAVA_API float GetTime();

    /// <summary>
    /// Returns the unscaled fixed time point at the begining of this update.
    /// </summary>
    LAVA_API float GetFixedTime();

    /// <summary>
    /// Returns the unscaled time at this moment.
    /// </summary>
    LAVA_API float GetTimeNow();

    /// <summary>
    /// Returns the fixed update rate.
    /// <para>This is the update rate of the FixedUpdate callback.</para>
    /// </summary>
    LAVA_API float GetFixedDeltaTime();

    /// <summary>
    /// Returns the time that passed since the last frame.
    /// </summary>
    LAVA_API float GetDeltaTime();

    /// <summary>
    /// Use this to scale the flow of time. Set it to 0 to pause the simulation.
    /// </summary>
    /// <param name="scale"></param>
    LAVA_API void SetTimeScale(float scale);

    LAVA_API float GetTimeScale();
}