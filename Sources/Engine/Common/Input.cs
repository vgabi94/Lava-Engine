using System.Runtime.InteropServices;

namespace Lava.Engine
{
    public static class Input
    {
        [DllImport("LavaCore.dll")]
        public static extern bool GetKey(KeyCode key);

        [DllImport("LavaCore.dll")]
        public static extern bool GetKeyPressed(KeyCode key);

        [DllImport("LavaCore.dll")]
        public static extern bool GetKeyReleased(KeyCode key);

        [DllImport("LavaCore.dll")]
        public static extern bool GetMouseButton(MouseButton button);

        [DllImport("LavaCore.dll")]
        public static extern bool GetMouseButtonPressed(MouseButton button);

        [DllImport("LavaCore.dll")]
        public static extern bool GetMouseButtonReleased(MouseButton button);

        [DllImport("LavaCore.dll")]
        public static extern void GetCursorPosition(out float X, out float Y);

        [DllImport("LavaCore.dll")]
        public static extern void GetScrollValue(out float X, out float Y);

        [DllImport("LavaCore.dll")]
        public static extern ModifierFlag GetModifierFlags(MouseButton button);
    }
}
