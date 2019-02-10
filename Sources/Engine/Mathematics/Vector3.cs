using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3 : IEquatable<Vector3>
    {
        public float X, Y, Z;

        #region Public Static Properties
        /// <summary>
        /// Returns the vector (0,0,0).
        /// </summary>
        public static Vector3 Zero { get { return new Vector3(0.0f); } }
        /// <summary>
        /// Returns the vector (1,1,1).
        /// </summary>
        public static Vector3 One { get { return new Vector3(1.0f); } }
        /// <summary>
        /// Returns the vector (1,0,0).
        /// </summary>
        public static Vector3 UnitX { get { return new Vector3(1.0f, 0.0f, 0.0f); } }
        /// <summary>
        /// Returns the vector (0,1,0).
        /// </summary>
        public static Vector3 UnitY { get { return new Vector3(0.0f, 1.0f, 0.0f); } }
        /// <summary>
        /// Returns the vector (0,0,1).
        /// </summary>
        public static Vector3 UnitZ { get { return new Vector3(0.0f, 0.0f, 1.0f); } }
        #endregion Public Static Properties

        #region Operators
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator +(Vector3 u, Vector3 v)
        {
            return new Vector3(u.X + v.X, u.Y + v.Y, u.Z + v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator +(Vector3 v, float s)
        {
            return new Vector3(v.X + s, v.Y + s, v.Z + s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator +(float s, Vector3 v)
        {
            return new Vector3(v.X + s, v.Y + s, v.Z + s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator -(Vector3 u, Vector3 v)
        {
            return new Vector3(u.X - v.X, u.Y - v.Y, u.Z - v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator -(Vector3 v, float s)
        {
            return new Vector3(v.X - s, v.Y - s, v.Z - s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator -(float s, Vector3 v)
        {
            return new Vector3(s - v.X, s - v.Y, s - v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator -(Vector3 v)
        {
            return new Vector3(-v.X, -v.Y, -v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator *(Vector3 u, Vector3 v)
        {
            return new Vector3(u.X * v.X, u.Y * v.Y, u.Z * v.Z);
        }

        /// <summary>
        /// Rotates a vector by the given quaternion. The same as q * v.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator *(Vector3 v, Quaternion q)
        {
            return q * v;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator *(float s, Vector3 v)
        {
            return new Vector3(v.X * s, v.Y * s, v.Z * s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator *(Vector3 v, float s)
        {
            return new Vector3(v.X * s, v.Y * s, v.Z * s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator /(Vector3 u, Vector3 v)
        {
            return new Vector3(u.X / v.X, u.Y / v.Y, u.Z / v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator /(float s, Vector3 v)
        {
            return new Vector3(s / v.X, s / v.Y, s / v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 operator /(Vector3 v, float s)
        {
            return new Vector3(v.X / s, v.Y / s, v.Z / s);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator ==(Vector3 u, Vector3 v)
        {
            return (u.X == v.X && u.Y == v.Y && u.Z == v.Z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator !=(Vector3 u, Vector3 v)
        {
            return (u.X != v.X || u.Y != v.Y || u.Z != v.Z);
        }
        #endregion

        #region Constructors
        /// <summary>Creates a Vector3 structure whose elements have the specified values.</summary>
        /// <param name="x">The value to assign to the X field.</param>
        /// <param name="y">The value to assign to the Y field.</param>
        /// <param name="z">The value to assign to the Z field.</param>
        public Vector3(float x, float y, float z)
        {
            X = x; 
            Y = y; 
            Z = z;
        }

        /// <summary>Creates a Vector3 structure whose three elements have the same value.</summary>
        /// <param name="value">The value to assign to all three elements.</param>
        public Vector3(float value)
        {
            X = Y = Z = value;
        }

        /// <summary>Creates a Vector3 structure based on a Vector2 and z.</summary>
        /// <param name="v">The Vector2 to use for the X and Y components.</param>
        /// <param name="z">The value to assign to the Z field.</param>
        public Vector3(Vector2 v, float z)
        {
            X = v.X;
            Y = v.Y;
            Z = z;
        }
        #endregion

        #region Overrides
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public override bool Equals(object obj)
        {
            if (!(obj is Vector3)) return false;

            return Equals((Vector3)obj);
        }

        public bool Equals(Vector3 other)
        {
            return this == other;
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        public override int GetHashCode()
        {
            int hash = X.GetHashCode();
            hash = HashCodeHelper.CombineHashCodes(hash, Y.GetHashCode());
            hash = HashCodeHelper.CombineHashCodes(hash, Z.GetHashCode());
            return hash;
        }

        /// <summary>
        /// Returns a string representing this Vector3.
        /// </summary>
        /// <returns>The string representation.</returns>
        public override string ToString()
        {
            return string.Format("[{0}, {1}, {2}]", X, Y, Z);
        }

        /// <summary>
        /// Parses a JSON stream and produces a Vector3 struct.
        /// </summary>
        public static Vector3 Parse(string text)
        {
            string[] split = text.Trim(new char[] { '{', '}' }).Split(',');
            if (split.Length != 3) return Zero;

            return new Vector3(float.Parse(split[0]), float.Parse(split[1]), float.Parse(split[2]));
        }

        public float this[int a]
        {
            get 
            {
                if (a > 2 || a < 0) throw new ArgumentOutOfRangeException();
                return (a == 0) ? X : (a == 1) ? Y : Z; 
            }
            set
            {
                if (a == 0) X = value;
                else if (a == 1) Y = value;
                else if (a == 2) Z = value;
                else throw new ArgumentOutOfRangeException();
            }
        }
        #endregion

        #region Methods
        /// <summary>
        /// Returns the length of the vector.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float Length()
        {
            float ls = X * X + Y * Y + Z * Z;
            return (float)System.Math.Sqrt(ls);
        }

        /// <summary>
        /// Returns the squared length of the vector.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z;
        }

        /// <summary>
        /// Computes the dot product between two vectors.
        /// </summary>
        /// <returns>The dot product of the two vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Dot(Vector3 u, Vector3 v)
        {
            return u.X * v.X + u.Y * v.Y + u.Z * v.Z;
        }

        /// <summary>
        /// Computes the dot product between two vectors.
        /// </summary>
        /// <returns>The dot product of the two vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float Dot(Vector3 v)
        {
            return X * v.X + Y * v.Y + Z * v.Z;
        }

        /// <summary>
        /// Computes the cross product between two vectors.
        /// </summary>
        /// <returns>The cross product of the two vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Cross(Vector3 u, Vector3 v)
        {
            return new Vector3(u.Y * v.Z - u.Z * v.Y,
                u.Z * v.X - u.X * v.Z,
                u.X * v.Y - u.Y * v.X);
        }

        /// <summary>
        /// Computes the cross product of this vector and another.
        /// </summary>
        /// <param name="v">The other vector</param>
        /// <returns>The cross product of the two vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector3 Cross(Vector3 v)
        {
            return new Vector3(Y * v.Z - Z * v.Y,
                Z * v.X - X * v.Z,
                X * v.Y - Y * v.X);
        }

        /// <summary>
        /// Returns the absolute value of this vector (element-wise).
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Abs(Vector3 value)
        {
            return new Vector3(System.Math.Abs(value.X), System.Math.Abs(value.Y), System.Math.Abs(value.Z));
        }

        /// <summary>
        /// Returns a vector with the same direction as the given vector, but with a length of 1.
        /// </summary>
        /// <returns>The normalized vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Vector3 Normalize()
        {
            float ls = X * X + Y * Y + Z * Z;
            float length = (float)System.Math.Sqrt(ls);
            return new Vector3(X / length, Y / length, Z / length);
        }

        /// <summary>
        /// Returns the Euclidean distance between the two given points.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Distance(Vector3 u, Vector3 v)
        {
            float dx = u.X - v.X;
            float dy = u.Y - v.Y;
            float dz = u.Z - v.Z;

            float ls = dx * dx + dy * dy + dz * dz;

            return (float)System.Math.Sqrt(ls);
        }

        /// <summary>
        /// Returns the Euclidean distance squared between the two given points.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float DistanceSquared(Vector3 u, Vector3 v)
        {
            float dx = u.X - v.X;
            float dy = u.Y - v.Y;
            float dz = u.Z - v.Z;

            return dx * dx + dy * dy + dz * dz;
        }

        /// <summary>
        /// Returns the reflection of a vector off a surface that has the specified normal.
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Reflect(Vector3 vector, Vector3 normal)
        {
            float dot = vector.X * normal.X + vector.Y * normal.Y + vector.Z * normal.Z;
            float tempX = normal.X * dot * 2f;
            float tempY = normal.Y * dot * 2f;
            float tempZ = normal.Z * dot * 2f;
            return new Vector3(vector.X - tempX, vector.Y - tempY, vector.Z - tempZ);
        }

        /// <summary>
        /// Computes a new vector using linear interpolation between two given vectors.
        /// </summary>
        /// <param name="u">First vector (amount = 0).</param>
        /// <param name="v">Second vector (amount = 1).</param>
        /// <param name="amount">Amount of each vector to consider [0, 1].</param>
        /// <returns>The interpolated vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Lerp(Vector3 u, Vector3 v, float amount)
        {
            return new Vector3(
                    u.X + (v.X - u.X) * amount,
                    u.Y + (v.Y - u.Y) * amount,
                    u.Z + (v.Z - u.Z) * amount);
        }

        /// <summary>
        /// Calculates the angle (in radians) between two vectors.
        /// </summary>
        /// <param name="first">The first vector.</param>
        /// <param name="second">The second vector.</param>
        /// <returns>Angle (in radians) between the vectors.</returns>
        /// <remarks>Note that the returned angle is never bigger than the constant Pi.</remarks>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float GetAngleBetween(Vector3 first, Vector3 second)
        {
            return (float)System.Math.Acos((Dot(first, second)) / (first.Length() * second.Length()));
        }

        /// <summary>
        /// Calculates the angle (in radians) between two vectors.
        /// </summary>
        /// <param name="v">The second vector.</param>
        /// <returns>Angle (in radians) between the vectors.</returns>
        /// <remarks>Note that the returned angle is never bigger than the constant Pi.</remarks>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float GetAngleBetween(Vector3 v)
        {
            return (float)System.Math.Acos((Dot(this, v)) / (this.Length() * v.Length()));
        }

        /// <summary>
        /// Returns a quaternion that represents the rotation between this
        /// vector and a destination vector.
        /// </summary>
        /// <param name="destination">The vector we would like to rotate to.</param>
        /// <returns>A quaternion representing the axis of rotation between this vector 
        /// and the destination vector.</returns>
        public Quaternion RotationTo(Vector3 destination)
        {
            // Based on Stan Melax's algorithm in "Game Programming Gems"
            Vector3 t_source = Normalize();
            Vector3 t_dest = destination.Normalize();

            float d = t_source.Dot(t_dest);

            // if dot == 1 then the vectors are the same
            if (d >= 1.0f) return Quaternion.Identity;
            else if (d < (1e-6f - 1.0f))
            {
                Vector3 t_axis = UnitX.Cross(this);
                if (t_axis.LengthSquared() < (1e-12)) // pick another if colinear
                    t_axis = UnitY.Cross(this);
                t_axis.Normalize();
                return Quaternion.CreateFromAxisAngle(t_axis, (float)System.Math.PI);
            }
            else
            {
                float t_sqrt = (float)System.Math.Sqrt((1 + d) * 2.0f);
                float t_invs = 1.0f / t_sqrt;

                Vector3 t_cross = t_source.Cross(t_dest);
                return new Quaternion(t_cross.X * t_invs, t_cross.Y * t_invs, t_cross.Z * t_invs, t_sqrt * 0.5f).Normalize();
            }
        }

        /// <summary>
        /// Returns a quaternion that represents the rotation between this
        /// vector and a destination vector.
        /// </summary>
        /// <param name="destination">The vector we would like to rotate to.</param>
        /// <returns>A quaternion representing the axis of rotation between this vector 
        /// and the destination vector.</returns>
        public static Quaternion RotationBetween(Vector3 u, Vector3 v)
        {
            // Based on Stan Melax's algorithm in "Game Programming Gems"
            Vector3 t_source = u.Normalize();
            Vector3 t_dest = v.Normalize();

            float d = t_source.Dot(t_dest);

            // if dot == 1 then the vectors are the same
            if (d >= 1.0f) return Quaternion.Identity;
            else if (d < (1e-6f - 1.0f))
            {
                Vector3 t_axis = UnitX.Cross(u);
                if (t_axis.LengthSquared() < (1e-12)) // pick another if colinear
                    t_axis = UnitY.Cross(u);
                t_axis.Normalize();
                return Quaternion.CreateFromAxisAngle(t_axis, (float)System.Math.PI);
            }
            else
            {
                float t_sqrt = (float)System.Math.Sqrt((1 + d) * 2.0f);
                float t_invs = 1.0f / t_sqrt;

                Vector3 t_cross = t_source.Cross(t_dest);
                return new Quaternion(t_cross.X * t_invs, t_cross.Y * t_invs, t_cross.Z * t_invs, t_sqrt * 0.5f).Normalize();
            }
        }

        /// <summary>
        /// Clamps a vector between a min and max value.
        /// </summary>
        /// <param name="value">The source vector.</param>
        /// <param name="min">The minimum vector.</param>
        /// <param name="max">The maximum vector.</param>
        /// <returns>A vector that has been clamped between the minimum and maximum vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Clamp(Vector3 value, Vector3 min, Vector3 max)
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

            return new Vector3(x, y, z);
        }

        /// <summary>
        /// Take the maximum of two vectors (element-wise).
        /// </summary>
        /// <returns>A vector that contains the maximum of the elements of the source vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Max(Vector3 u, Vector3 v)
        {
            return new Vector3(System.Math.Max(u.X, v.X), System.Math.Max(u.Y, v.Y), System.Math.Max(u.Z, v.Z));
        }

        /// <summary>
        /// Take the minimum of two vectors (element-wise).
        /// </summary>
        /// <returns>A vector that contains the minimum of the elements of the source vectors.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Min(Vector3 u, Vector3 v)
        {
            return new Vector3(System.Math.Min(u.X, v.X), System.Math.Min(u.Y, v.Y), System.Math.Min(u.Z, v.Z));
        }

        /// <summary>
        /// Computes the square root of the vector (element-wise).
        /// </summary>
        /// <returns>The square root of the vector.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector3 Sqrt(Vector3 v)
        {
            return new Vector3((float)System.Math.Sqrt(v.X), (float)System.Math.Sqrt(v.Y), (float)System.Math.Sqrt(v.Z));
        }
        #endregion
    }
}