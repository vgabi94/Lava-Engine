using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4 : IEquatable<Vector4>
    {
        public float X, Y, Z, W;

        #region Public Static Properties
        /// <summary>
        /// Returns the vector (0,0,0,0).
        /// </summary>
        public static Vector4 Zero { get { return new Vector4(0.0f); } }
        /// <summary>
        /// Returns the vector (1,1,1,1).
        /// </summary>
        public static Vector4 One { get { return new Vector4(1.0f); } }
        /// <summary>
        /// Returns the vector (1,0,0,0).
        /// </summary>
        public static Vector4 UnitX { get { return new Vector4(1.0f, 0.0f, 0.0f, 0.0f); } }
        /// <summary>
        /// Returns the vector (0,1,0,0).
        /// </summary>
        public static Vector4 UnitY { get { return new Vector4(0.0f, 1.0f, 0.0f, 0.0f); } }
        /// <summary>
        /// Returns the vector (0,0,1,0).
        /// </summary>
        public static Vector4 UnitZ { get { return new Vector4(0.0f, 0.0f, 1.0f, 0.0f); } }
        /// <summary>
        /// Returns the vector (0,0,0,1).
        /// </summary>
        public static Vector4 UnitW { get { return new Vector4(0.0f, 0.0f, 0.0f, 1.0f); } }
        #endregion Public Static Properties

        #region Properties
        public Vector3 XYZ
        {
            get => new Vector3(X, Y, Z);
            set
            {
                X = value.X;
                Y = value.Y;
                Z = value.Z;
            }
        }
        #endregion

        #region Operators
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator +(Vector4 u, Vector4 v)
        {
            return new Vector4(u.X + v.X, u.Y + v.Y, u.Z + v.Z, u.W + v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator +(Vector4 v, float s)
        {
            return new Vector4(v.X + s, v.Y + s, v.Z + s, v.W + s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator +(float s, Vector4 v)
        {
            return new Vector4(v.X + s, v.Y + s, v.Z + s, v.W + s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator -(Vector4 u, Vector4 v)
        {
            return new Vector4(u.X - v.X, u.Y - v.Y, u.Z - v.Z, u.W - v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator -(Vector4 v, float s)
        {
            return new Vector4(v.X - s, v.Y - s, v.Z - s, v.W - s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator -(float s, Vector4 v)
        {
            return new Vector4(s - v.X, s - v.Y, s - v.Z, s - v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator -(Vector4 v)
        {
            return new Vector4(-v.X, -v.Y, -v.Z, -v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator *(Vector4 u, Vector4 v)
        {
            return new Vector4(u.X * v.X, u.Y * v.Y, u.Z * v.Z, u.W * v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator *(float s, Vector4 v)
        {
            return new Vector4(v.X * s, v.Y * s, v.Z * s, v.W * s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator *(Vector4 v, float s)
        {
            return new Vector4(v.X * s, v.Y * s, v.Z * s, v.W * s);
        }

        /// <summary>
        /// Rotates a vector by the given quaternion. The same as q * v.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator *(Vector4 v, Quaternion q)
        {
            return q * v;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator /(Vector4 u, Vector4 v)
        {
            return new Vector4(u.X / v.X, u.Y / v.Y, u.Z / v.Z, u.W / v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator /(float s, Vector4 v)
        {
            return new Vector4(s / v.X, s / v.Y, s / v.Z, s / v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 operator /(Vector4 v, float s)
        {
            return new Vector4(v.X / s, v.Y / s, v.Z / s, v.W / s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator ==(Vector4 u, Vector4 v)
        {
            return (u.X == v.X && u.Y == v.Y && u.Z == v.Z && u.W == v.W);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator !=(Vector4 u, Vector4 v)
        {
            return (u.X != v.X || u.Y != v.Y || u.Z != v.Z || u.W != v.W);
        }
        #endregion

        #region Constructors
        /// <summary>Creates a Vector4 structure whose elements have the specified values.</summary>
        /// <param name="x">The value to assign to the X field.</param>
        /// <param name="y">The value to assign to the Y field.</param>
        /// <param name="z">The value to assign to the Z field.</param>
        /// <param name="w">The value to assign to the W field.</param>
        public Vector4(float x, float y, float z, float w)
        {
            X = x; 
            Y = y; 
            Z = z; 
            W = w;
        }

        /// <summary>Creates a Vector4 structure based on a Vector3 and w.</summary>
        /// <param name="v">The Vector3 to use for the X, Y and Z components.</param>
        /// <param name="w">The value to assign to the W field.</param>
        public Vector4(Vector3 v, float w)
        {
            X = v.X; 
            Y = v.Y; 
            Z = v.Z; 
            W = w;
        }

        /// <summary>Creates a Vector4 structure whose four elements have the same value.</summary>
        /// <param name="value">The value to assign to all four elements.</param>
        public Vector4(float value)
        {
            X = Y = Z = W = value;
        }

        /// <summary>Creates a Vector4 structure based on a Vector2, z and w.</summary>
        /// <param name="v">The Vector2 to use for the X and Y components.</param>
        /// <param name="z">The value to assign to the Z field.</param>
        /// <param name="w">The value to assign to the W field.</param>
        public Vector4(Vector2 v, float z, float w)
        {
            X = v.X;
            Y = v.Y;
            Z = z;
            W = w;
        }
        #endregion

        #region Overrides
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public override bool Equals(object obj)
        {
            if (!(obj is Vector4)) return false;

            return Equals((Vector4)obj);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool Equals(Vector4 other)
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
            hash = HashCodeHelper.CombineHashCodes(hash, Z.GetHashCode());
            hash = HashCodeHelper.CombineHashCodes(hash, W.GetHashCode());
            return hash;
        }

        /// <summary>
        /// Returns a string representing this Vector4.
        /// </summary>
        /// <returns>The string representation.</returns>
        public override string ToString()
        {
            return string.Format("[{0}, {1}, {2}, {3}]", X, Y, Z, W);
        }

        /// <summary>
        /// Parses a JSON stream and produces a Vector4 struct.
        /// </summary>
        public static Vector4 Parse(string text)
        {
            string[] split = text.Trim(new char[] { '{', '}' }).Split(',');
            if (split.Length != 4) return Zero;

            return new Vector4(float.Parse(split[0]), float.Parse(split[1]), float.Parse(split[2]), float.Parse(split[3]));
        }

        public float this[int a]
        {
            get 
            {
                if (a > 3 || a < 0) throw new ArgumentOutOfRangeException();
                return (a == 0) ? X : (a == 1) ? Y : (a == 2) ? Z : W; 
            }
            set
            {
                if (a == 0) X = value;
                else if (a == 1) Y = value;
                else if (a == 2) Z = value;
                else if (a == 3) W = value;
                else throw new ArgumentOutOfRangeException();
            }
        }
        #endregion

        #region Methods
        /// <summary>
        /// Computes the length of this vector.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float Length()
        {
            float ls = X * X + Y * Y + Z * Z + W * W;
            return (float) System.Math.Sqrt(ls);
    }

        /// <summary>
        /// Computes the squared length of this vector.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z + W * W;
        }

        /// <summary>
        /// Computes the dot product between two vectors.
        /// </summary>
        /// <returns>The dot product of the two vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Dot(Vector4 u, Vector4 v)
        {
            return u.X * v.X + u.Y * v.Y + u.Z * v.Z + u.W * v.W;
        }

        /// <summary>
        /// Computes the dot product between two vectors.
        /// </summary>
        /// <returns>The dot product of the two vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float Dot(Vector4 v)
        {
            return Dot(this, v);
        }

        /// <summary>
        /// Normalizes the Vector4 structure to have a peak value of one.
        /// </summary>
        /// <returns>if (Length = 0) return Zero; else return Vector4(x,y,z,w)/Length</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector4 Normalize()
        {
            float ls = X * X + Y * Y + Z * Z + W * W;
            float invNorm = 1.0f / (float)System.Math.Sqrt(ls);

            return new Vector4(X * invNorm, Y * invNorm,
                Z * invNorm, W * invNorm);
        }

        /// <summary>
        /// Computes a new vector using linear interpolation between two given vectors.
        /// </summary>
        /// <param name="u">First vector (amount = 0).</param>
        /// <param name="v">Second vector (amount = 1).</param>
        /// <param name="amount">Amount of each vector to consider [0, 1].</param>
        /// <returns>The interpolated vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 Lerp(Vector4 u, Vector4 v, float amount)
        {
            return new Vector4(
                u.X + (v.X - u.X) * amount,
                u.Y + (v.Y - u.Y) * amount,
                u.Z + (v.Z - u.Z) * amount,
                u.W + (v.W - u.W) * amount);
        }

        /// <summary>
        /// Clamps a vector between a min and max value.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 Clamp(Vector4 value, Vector4 min, Vector4 max)
        {
            // This compare order is very important!!!
            // We must follow HLSL behavior in the case user specified min value is bigger than max value.

            float x = value.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            float w = value.W;
            w = (w > max.W) ? max.W : w;
            w = (w < min.W) ? min.W : w;

            return new Vector4(x, y, z, w);
        }

        /// <summary>
        /// Returns the Euclidean distance between the two given points.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Distance(Vector4 value1, Vector4 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            float dw = value1.W - value2.W;

            float ls = dx * dx + dy * dy + dz * dz + dw * dw;

            return (float)System.Math.Sqrt(ls);
        }

        /// <summary>
        /// Returns the Euclidean distance squared between the two given points.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float DistanceSquared(Vector4 value1, Vector4 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            float dw = value1.W - value2.W;

            return dx * dx + dy * dy + dz * dz + dw * dw;
        }

        /// <summary>
        /// Returns the absolute value of this vector (element-wise).
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector4 Abs(Vector4 value)
        {
            return new Vector4(System.Math.Abs(value.X), System.Math.Abs(value.Y),
                System.Math.Abs(value.Z), System.Math.Abs(value.W));
        }

        public float Get(int i) { return 1f; }
        #endregion
    }
}
