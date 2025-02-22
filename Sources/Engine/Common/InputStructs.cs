﻿namespace Lava.Engine
{
    public enum KeyCode
    {
        Unknown = -1,
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus,
        Period,
        Slash,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        Semicolon = 59,
        Equal = 61,
        A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T,
        U, V, W, X, Y, Z,
        LeftBracket,
        Backslash,
        RightBracket,
        GraveAccent = 96,
        World1 = 161,
        World2 = 162,
        Escape = 256,
        Enter, Tab, Backspace, Insert, Delete, Right,
        Left, Down, Up, PageUp, PageDown, Home, End,
        CapsLock = 280, ScrollLock, NumLock, PrintScreen, Pause,
        F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13,
        F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,
        KeyPad0 = 320, KeyPad1, KeyPad2, KeyPad3, KeyPad4, KeyPad5, KeyPad6, KeyPad7,
        KeyPad8, KeyPad9,
        Decimal, Divide, Multiply, Subtract, Add, KeypadEnter, KeypadEqual,
        LeftShit = 340, LeftControl, LeftAlt, LeftSuper,
        RightShift, RightControl, RightAlt, RightSuper,
        Menu
    }

    public enum ModifierFlag
    {
        Shift = 0x0001,
        Control = 0x0002,
        Alt = 0x0003,
        Super = 0x0008
    }

    public enum MouseButton
    {
        Num1 = 0, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8,
        Left = Num1,
        Right = Num2,
        Middle = Num3
    }
}