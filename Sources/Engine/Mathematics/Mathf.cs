using System.Runtime.CompilerServices;

namespace Lava.Mathematics
{
    public static class Mathf
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Degrees(float radians)
        {
            return radians * (180f * (float)System.Math.PI);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Radians(float degrees)
        {
            return (float)System.Math.PI * degrees / 180f;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static int Clamp(int min, int max, int value)
        {
            value = value < min ? min : value;
            value = value > max ? max : value;
            return value;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Clamp(float min, float max, float value)
        {
            value = value < min ? min : value;
            value = value > max ? max : value;
            return value;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Clamp01(float value)
        {
            value = value < 0f ? 0f : value;
            value = value > 1f ? 1f : value;
            return value;
        }
    }
}
