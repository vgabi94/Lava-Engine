using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2 : IEquatable<Vector2>
    {
        public float X, Y;

        #region Public Static Properties
        /// <summary>
        /// Returns the vector (0,0).
        /// </summary>
        public static Vector2 Zero { get { return new Vector2(0f, 0f); } }
        /// <summary>
        /// Returns the vector (1,1).
        /// </summary>
        public static Vector2 One { get { return new Vector2(1f, 1f); } }
        /// <summary>
        /// Returns the vector (1,0).
        /// </summary>
        public static Vector2 UnitX { get { return new Vector2(1f, 0f); } }
        /// <summary>
        /// Returns the vector (0,1).
        /// </summary>
        public static Vector2 UnitY { get { return new Vector2(0f, 1f); } }
        #endregion Public Static Properties

        #region Operators
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator +(Vector2 u, Vector2 v)
        {
            return new Vector2(u.X + v.X, u.Y + v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator +(Vector2 v, float s)
        {
            return new Vector2(v.X + s, v.Y + s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator +(float s, Vector2 v)
        {
            return new Vector2(v.X + s, v.Y + s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator -(Vector2 u, Vector2 v)
        {
            return new Vector2(u.X - v.X, u.Y - v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator -(Vector2 v, float s)
        {
            return new Vector2(v.X - s, v.Y - s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator -(float s, Vector2 v)
        {
            return new Vector2(s - v.X, s - v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator -(Vector2 v)
        {
            return new Vector2(-v.X, -v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator *(Vector2 u, Vector2 v)
        {
            return new Vector2(u.X * v.X, u.Y * v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator *(float s, Vector2 v)
        {
            return new Vector2(v.X * s, v.Y * s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator *(Vector2 v, float s)
        {
            return new Vector2(v.X * s, v.Y * s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator /(Vector2 u, Vector2 v)
        {
            return new Vector2(u.X / v.X, u.Y / v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator /(float s, Vector2 v)
        {
            return new Vector2(s / v.X, s / v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 operator /(Vector2 v, float s)
        {
            return new Vector2(v.X / s, v.Y / s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator ==(Vector2 u, Vector2 v)
        {
            return (u.X == v.X && u.Y == v.Y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator !=(Vector2 u, Vector2 v)
        {
            return !(u == v);
        }
        #endregion

        #region Constructors
        /// <summary>Creates a Vector2 structure whose elements have the specified values.</summary>
        /// <param name="x">The value to assign to the X field.</param>
        /// <param name="y">The value to assign to the Y field.</param>
        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        /// <summary>Creates a Vector2 structure whose two elements have the same value.</summary>
        /// <param name="value">The value to assign to all two elements.</param>
        public Vector2(float value) : this(value, value) { }
        #endregion

        #region Overrides
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public override bool Equals(object obj)
        {
            if (!(obj is Vector2)) return false;

            return Equals((Vector2)obj);
        }

        public bool Equals(Vector2 other)
        {
            return this == other;
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            int hash = X.GetHashCode();
            hash = HashCodeHelper.CombineHashCodes(hash, Y.GetHashCode());
            return hash;
        }

        /// <summary>
        /// Returns a string representing this Vector2.
        /// </summary>
        /// <returns>The string representation.</returns>
        public override string ToString()
        {
            return string.Format("[{0}, {1}]", X, Y);
        }

        /// <summary>
        /// Parses a JSON stream and produces a Vector2 struct.
        /// </summary>
        public static Vector2 Parse(string text)
        {
            string[] split = text.Trim(new char[] { '{', '}' }).Split(',');
            if (split.Length != 2) return Vector2.Zero;

            return new Vector2(float.Parse(split[0]), float.Parse(split[1]));
        }

        public float this[int a]
        {
            get 
            {
                if (a > 1 || a < 0) throw new ArgumentOutOfRangeException();
                return (a == 0) ? X : Y; 
            }
            set
            {
                if (a == 0) X = value;
                else if (a == 1) Y = value;
                else throw new ArgumentOutOfRangeException();
            }
        }
        #endregion

        #region Methods
        /// <summary>
        /// Returns the perpendicular vector in counter clockwise order.
        /// </summary>
        public Vector2 OrthogonalCounterClockwise()
        {
            return new Vector2(Y, -X);
        }

        /// <summary>
        /// Returns the perpendicular vector in clockwise order.
        /// </summary>
        public Vector2 OrthogonalClockwise()
        {
            return new Vector2(-Y, X);
        }

        /// <summary>
        /// Computes the length or magnitude of this vector.
        /// </summary>
        /// <returns>The length of the vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float Length()
        {
            float ls = X * X + Y * Y;
            return (float)System.Math.Sqrt(ls);
        }

        /// <summary>
        /// Computes the squared length or magnitude of this vector.
        /// </summary>
        /// <returns>The squared length of the vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float LengthSquared()
        {
            return X * X + Y * Y;
        }

        /// <summary>
        /// Computes the dot product between two vectors.
        /// </summary>
        /// <param name="u">First vector.</param>
        /// <param name="v">Second vector.</param>
        /// <returns>The dot product between u and v.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Dot(Vector2 u, Vector2 v)
        {
            return u.X * v.X + u.Y * v.Y;
        }

        /// <summary>
        /// Computes the dot product between this vector and another.
        /// </summary>
        /// <param name="v">The other vector.</param>
        /// <returns>The dot product between this and v.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float Dot(Vector2 v)
        {
            return X * v.X + Y * v.Y;
        }

        /// <summary>
        /// Returns a vector with the same direction as the given vector, but with a length of 1.
        /// </summary>
        /// <returns>The normalized vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector2 Normalize()
        {
            float ls = X * X + Y * Y;
            float invNorm = 1.0f / (float)System.Math.Sqrt(ls);

            return new Vector2(X * invNorm, Y * invNorm);
        }

        /// <summary>
        /// Computes a new vector using linear interpolation between two given vectors.
        /// </summary>
        /// <param name="u">First vector (amount = 0).</param>
        /// <param name="v">Second vector (amount = 1).</param>
        /// <param name="amount">Amount of each vector to consider [0, 1].</param>
        /// <returns>The interpolated vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 Lerp(Vector2 u, Vector2 v, float amount)
        {
            return new Vector2(
                u.X + (v.X - u.X) * amount,
                u.Y + (v.Y - u.Y) * amount);
        }

        /// <summary>
        /// Computes the absolute value of this vector (element-wise).
        /// </summary>
        /// <param name="value">The source vector.</param>
        /// <returns>A new absolute-value vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 Abs(Vector2 value)
        {
            return new Vector2(System.Math.Abs(value.X), System.Math.Abs(value.Y));
        }

        /// <summary>
        /// Clamps a vector between a min and max value.
        /// </summary>
        /// <param name="value">The source vector.</param>
        /// <param name="min">The minimum vector.</param>
        /// <param name="max">The maximum vector.</param>
        /// <returns>A vector that has been clamped between the minimum and maximum vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 Clamp(Vector2 value, Vector2 min, Vector2 max)
        {
            // This compare order is very important!!!
            // We must follow HLSL behavior in the case user specified min value is bigger than max value.

            float x = value.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            return new Vector2(x, y);
        }

        /// <summary>
        /// Computes the square root of the vector (element-wise).
        /// </summary>
        /// <returns>The square root of the vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 Sqrt(Vector2 v)
        {
            return new Vector2((float)System.Math.Sqrt(v.X), (float)System.Math.Sqrt(v.Y));
        }
        #endregion
    }
}
