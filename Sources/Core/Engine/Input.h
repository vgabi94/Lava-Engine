#pragma once

#include <Common\Constants.h>
#include <GLFW\glfw3.h>

#include <array>

namespace Engine
{
    constexpr int MAX_KEYS = 349;
    constexpr int MAX_BUTTONS = 8;

    enum State : uint8_t
    {
        NONE = 0, RELEASED, PRESSED, HOLD = 4
    };

    struct Input
    {
        std::array<uint8_t, MAX_KEYS> keyState;
        std::array<uint8_t, MAX_BUTTONS> mouseState;

        float scrollX, scrollY;
        float mousePositionX, mousePositionY;

        int mods;

        void Init();
        void Update();
        void RegisterCallbacks(GLFWwindow * window);
    };

    extern Input g_Input;
}

/* EXPORTED INTERFACE */
extern "C"
{
    LAVA_API bool GetKeyPressed(int key);
    LAVA_API bool GetKeyReleased(int key);
    LAVA_API bool GetKey(int key);

    LAVA_API bool GetMouseButtonPressed(int button);
    LAVA_API bool GetMouseButtonReleased(int button);
    LAVA_API bool GetMouseButton(int button);

    LAVA_API void GetCursorPosition(float * X, float * Y);
    LAVA_API void GetScrollValue(float * X, float * Y);

    LAVA_API int GetModifierFlags();
}