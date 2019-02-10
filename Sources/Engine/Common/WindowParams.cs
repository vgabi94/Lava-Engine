using System;
using System.Runtime.InteropServices;

namespace Lava.Engine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct WindowParams
    {
        public bool Resizable;
        public bool Focused;
        public bool Visible;
        public bool Maximized;
        public bool Fullscreen;
        public int Width;
        public int Height;
        public string Name;
        public IntPtr Parent;

        public WindowParams(bool resizable, bool focused, bool visible, bool maximized, bool fullscreen, int width, int height, string name)
        {
            Resizable = resizable;
            Focused = focused;
            Visible = visible;
            Maximized = maximized;
            Fullscreen = fullscreen;
            Width = width;
            Height = height;
            Name = name;
            Parent = IntPtr.Zero;
        }

        public WindowParams(int width, int height)
        {
            Resizable = false;
            Focused = true;
            Visible = true;
            Maximized = false;
            Fullscreen = false;
            Width = width;
            Height = height;
            Name = "App";
            Parent = IntPtr.Zero;
        }

        public WindowParams(int width, int height, bool fullscreen)
        {
            Resizable = false;
            Focused = true;
            Visible = true;
            Maximized = false;
            Fullscreen = fullscreen;
            Width = width;
            Height = height;
            Name = "App";
            Parent = IntPtr.Zero;
        }

        public WindowParams(bool fullscreen)
        {
            Resizable = false;
            Focused = true;
            Visible = true;
            Maximized = false;
            Fullscreen = fullscreen;
            Width = 1336;
            Height = 768;
            Name = "App";
            Parent = IntPtr.Zero;
        }

        public WindowParams(bool fullscreen, string name)
        {
            Resizable = false;
            Focused = true;
            Visible = true;
            Maximized = false;
            Fullscreen = fullscreen;
            Width = 1336;
            Height = 768;
            Name = name;
            Parent = IntPtr.Zero;
        }

#if LAVA_EDITOR
        public WindowParams(int width, int height, IntPtr parent)
        {
            Resizable = true;
            Focused = true;
            Visible = false;
            Maximized = false;
            Fullscreen = false;
            Width = width;
            Height = height;
            Name = "Tool";
            Parent = parent;
        }
#endif
    }
}
