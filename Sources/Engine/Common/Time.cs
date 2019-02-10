using System.Runtime.InteropServices;

namespace Lava.Engine
{
    public static class Time
    {
        [DllImport("LavaCore.dll")]
        public static extern float GetTime();

        [DllImport("LavaCore.dll")]
        public static extern float GetFixedTime();

        [DllImport("LavaCore.dll")]
        public static extern float GetTimeNow();

        [DllImport("LavaCore.dll")]
        public static extern float GetFixedDeltaTime();

        [DllImport("LavaCore.dll")]
        public static extern float GetDeltaTime();

        [DllImport("LavaCore.dll")]
        public static extern void SetTimeScale(float scale);

        [DllImport("LavaCore.dll")]
        public static extern float GetTimeScale();
    }
}
