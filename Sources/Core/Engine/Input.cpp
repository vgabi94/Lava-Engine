#include "Input.h"

namespace Engine
{
    Input g_Input;

    void Input::Init()
    {
        keyState.fill(0);
        mouseState.fill(0);
    }
    
    void Input::Update()
    {
        mods = 0;

        for (auto& key : keyState)
        {
            REMOVE_STATE(key, ((uint8_t)State::PRESSED | (uint8_t)State::RELEASED));
        }

        for (auto& button : mouseState)
        {
            REMOVE_STATE(button, ((uint8_t)State::PRESSED | (uint8_t)State::RELEASED));
        }
    }

    /// <summary>
    /// Internal - handles keyboard related events
    /// </summary>
    static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            ADD_STATE(g_Input.keyState[key], ((uint8_t)State::PRESSED | (uint8_t)State::HOLD));
        }
        else if (action == GLFW_RELEASE)
        {
            REMOVE_STATE(g_Input.keyState[key], (uint8_t)State::HOLD);
            ADD_STATE(g_Input.keyState[key], (uint8_t)State::RELEASED);
        }

        g_Input.mods = mods;
    }


    /// <summary>
    /// Internal - handles mouse related events
    /// </summary>
    static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            ADD_STATE(g_Input.mouseState[button], ((uint8_t)State::PRESSED | (uint8_t)State::HOLD));
        }
        else if (action == GLFW_RELEASE)
        {
            REMOVE_STATE(g_Input.mouseState[button], (uint8_t)State::HOLD);
            ADD_STATE(g_Input.mouseState[button], (uint8_t)State::RELEASED);
        }

        g_Input.mods = mods;
    }

    /// <summary>
    /// Internal - handles scroll wheel movement
    /// </summary>
    static void ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
    {
        g_Input.scrollX = (float)xoffset;
        g_Input.scrollY = (float)yoffset;
    }

    /// <summary>
    /// Internal - monitors mouse position on screen
    /// </summary>
    static void CursorPosCallback(GLFWwindow * window, double xpos, double ypos)
    {
        g_Input.mousePositionX = (float)xpos;
        g_Input.mousePositionY = (float)ypos;
    }
    
    void Input::RegisterCallbacks(GLFWwindow * window)
    {
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetScrollCallback(window, ScrollCallback);
        glfwSetCursorPosCallback(window, CursorPosCallback);
    }
}

/* EXPORTED INTERFACE */
LAVA_API bool GetKeyPressed(int key)
{
    return HAS_STATE(Engine::g_Input.keyState[key], (uint8_t)Engine::State::PRESSED);
}

LAVA_API bool GetKeyReleased(int key)
{
    return HAS_STATE(Engine::g_Input.keyState[key], (uint8_t)Engine::State::RELEASED);
}

LAVA_API bool GetKey(int key)
{
    return HAS_STATE(Engine::g_Input.keyState[key], (uint8_t)Engine::State::HOLD);
}

LAVA_API bool GetMouseButtonPressed(int button)
{

    return HAS_STATE(Engine::g_Input.mouseState[(int)button], (uint8_t)Engine::State::PRESSED);
}

LAVA_API bool GetMouseButtonReleased(int button)
{
    return HAS_STATE(Engine::g_Input.mouseState[(int)button], (uint8_t)Engine::State::RELEASED);
}

LAVA_API bool GetMouseButton(int button)
{
    return HAS_STATE(Engine::g_Input.mouseState[(int)button], (uint8_t)Engine::State::HOLD);
}

LAVA_API void GetCursorPosition(float * X, float * Y)
{
    *X = Engine::g_Input.mousePositionX;
    *Y = Engine::g_Input.mousePositionY;
}

LAVA_API void GetScrollValue(float * X, float * Y)
{
    *X = Engine::g_Input.scrollX;
    *Y = Engine::g_Input.scrollY;
}

LAVA_API int GetModifierFlags()
{
    return Engine::g_Input.mods;
}
