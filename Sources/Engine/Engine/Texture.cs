using System;
using System.Runtime.InteropServices;

namespace Lava.Engine
{
    public static class Texture
    {
        /// <summary>
        /// Load an image as a texture.
        /// </summary>
        /// <param name="path">The relative path to the image</param>
        [DllImport("LavaCore.dll")]
        public static extern uint Load2D(string path, bool genmips= false);

        /// <summary>
        /// Load an HDR image as a 2D texture.
        /// </summary>
        /// <param name="path">The relative path to the image</param>
        [DllImport("LavaCore.dll")]
        public static extern uint LoadHDR(string path, bool genmips = false);

        [DllImport("LavaCore.dll")]
        private static extern uint CreateFromColor(byte r, byte g, byte b, byte a);

        /// <summary>
        /// Creates a 1x1 texture from the given color.
        /// </summary>
        public static uint FromColor(Color color)
        {
            return CreateFromColor(color.R, color.G, color.B, color.A);
        }
    }
}