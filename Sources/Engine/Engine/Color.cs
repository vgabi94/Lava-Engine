using System;
using System.Runtime.CompilerServices;
using Lava.Mathematics;

namespace Lava.Engine
{
    public struct Color
    {
        #region Public Fields
        public byte R;
        public byte G;
        public byte B;
        public byte A;
        #endregion

        #region Static Constructors
        public static Color RED => new Color(255, 0, 0, 255);
        public static Color GREEN => new Color(0, 255, 0, 255);
        public static Color BLUE => new Color(0, 0, 255, 255);
        public static Color YELLOW => new Color(255, 255, 0, 255);
        public static Color MAGENTA => new Color(255, 0, 255, 255);
        public static Color CYAN => new Color(0, 255, 255, 255);
        public static Color ORANGE => new Color(255, 165, 0, 255);
        public static Color PINK => new Color(255, 192, 203, 255);
        public static Color PURPLE => new Color(128, 0, 128, 255);
        public static Color GRAY => new Color(128, 128, 128, 255);
        public static Color BROWN => new Color(165, 42, 42, 255);
        public static Color WHITE => new Color(255, 255, 255, 255);
        public static Color BLACK => new Color(0, 0, 0, 255);
        #endregion

        #region Constructors
        /// <summary>
        /// Create a color from rgba values [0-255].
        /// </summary>
        public Color(byte r, byte g, byte b, byte a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        /// <summary>
        /// Create a color with alpha set to 255.
        /// </summary>
        public Color(byte r, byte g, byte b)
        {
            R = r;
            G = g;
            B = b;
            A = 255;
        }

        /// <summary>
        /// Create a color from a hex value (ex: #ffff00 for yellow). Alpha set to 255.
        /// </summary>
        public static Color FromHex(string hex)
        {
            hex = hex.Trim('#');
            if (hex.Length != 6)
                throw new Exception("Hex value length must be 6!");

            Color color;
            color.R = ConvertHexChars(hex[0], hex[1]);
            color.G = ConvertHexChars(hex[2], hex[3]);
            color.B = ConvertHexChars(hex[4], hex[5]);
            color.A = 255;
            return color;
        }

        private static byte ConvertHexChars(char first, char second)
        {
            first = char.ToLowerInvariant(first);
            second = char.ToLowerInvariant(second);
            byte f = 0, s = 0;

            if (first < '0' || first > '9')
                if (first < 'a' && first > 'f')
                    throw new Exception("Hex value has wrong format!");

            if (second < '0' || second > '9')
                if (second < 'a' && second > 'f')
                    throw new Exception("Hex value has wrong format!");

            if (first >= 'a' || first < 'f')
                f = (byte)(first - 'a' + 10);
            else
                f = (byte)(first - '0');

            if (second >= 'a' || second < 'f')
                s = (byte)(second - 'a' + 10);
            else
                s = (byte)(second - '0');

            return (byte)(s + f * 16);
        }
        #endregion

        #region Properties
        public float Alpha
        {
            get => A / (float)255;
            set
            {
                float a = Mathf.Clamp01(value);
                A = (byte)(a * 255);
            }
        }
        public float Red
        {
            get => R / (float)255;
            set
            {
                float r = Mathf.Clamp01(value);
                R = (byte)(r * 255);
            }
        }
        public float Green
        {
            get => G / (float)255;
            set
            {
                float g = Mathf.Clamp01(value);
                G = (byte)(g * 255);
            }
        }
        public float Blue
        {
            get => B / (float)255;
            set
            {
                float b = Mathf.Clamp01(value);
                B = (byte)(b * 255);
            }
        }
        #endregion

        #region Methods
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector4 ToVector()
        {
            return new Vector4(Red, Green, Blue, Alpha);
        }

        /// <summary>
        /// Returns a string reprezentation of the color.
        /// </summary>
        public override string ToString()
        {
            return string.Format("<{0}, {1}, {2}, {3}>", R, G, B, A);
        }
        #endregion
    }
}