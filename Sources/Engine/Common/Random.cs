using System;

namespace Lava.Engine
{
    public static class Random
    {
        public static int Seed
        {
            get => seed;
            set
            {
                seed = value;
                rnd = new System.Random(seed);
            }
        }

        private static int seed;

        private static System.Random rnd;

        static Random()
        {
            seed = (int)DateTimeOffset.Now.ToUnixTimeSeconds();
            rnd = new System.Random(seed);
        }

        /// <summary>
        /// Returns a random integer between min (inclusive) and max (exclusive).
        /// </summary>
        public static int Range(int min, int max)
        {
            return rnd.Next(min, max);
        }

        /// <summary>
        /// Returns a random float between min (inclusive) and max (inclusive).
        /// </summary>
        public static float Range(float min, float max)
        {
            return min + (float)rnd.NextDouble() * max;
        }

        /// <summary>
        /// Returns a random float between 0 and 1.
        /// </summary>
        public static float Range01()
        {
            return (float)rnd.NextDouble();
        }

        /// <summary>
        /// Returns a random integer between 0 (inclusive) and MaxInt (exclusive).
        /// </summary>
        /// <returns></returns>
        public static int Range()
        {
            return rnd.Next();
        }

        /// <summary>
        /// Returns a random point inside the unit sphere centered in origin.
        /// </summary>
        public static Mathematics.Vector3 UnitSphere
        {
            get => new Mathematics.Vector3(-1f + Range01() * 2f, -1f + Range01() * 2f, -1f + Range01() * 2f);
        }

        /// <summary>
        /// Returns a random point with values between 0 and 1.
        /// </summary>
        public static Mathematics.Vector3 Point
        {
            get => new Mathematics.Vector3(Range01(), Range01(), Range01());
        }

        /// <summary>
        /// Returns a random point inside the unit circle centered in origin.
        /// </summary>
        public static Mathematics.Vector2 UnitCircle
        {
            get => new Mathematics.Vector2(Range01(), Range01());
        }

        /// <summary>
        /// Returns a random color.
        /// </summary>
        public static Color Color
        {
            get => new Color((byte)Range(0, 256), (byte)Range(0, 256), (byte)Range(0, 256), 255);
        }
    }
}
