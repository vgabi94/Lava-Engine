using System;
using System.Runtime.InteropServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Quaternion : IEquatable<Quaternion>
    {
        // Some implementations were taken from
        // https://referencesource.microsoft.com/#System.Numerics/System/Numerics/Quaternion.cs

        #region Public Fields
        public float X, Y, Z, W;
        #endregion

        #region Static Constructors
        public static Quaternion Zero
        {
            get { return new Quaternion(0, 0, 0, 0); }
        }

        public static Quaternion Identity
        {
            get { return new Quaternion(0, 0, 0, 1); }
        }
        #endregion

        #region Constructors
        public Quaternion(float x, float y, float z, float w)
        {
            X = x; Y = y; Z = z; W = w;
        }

        public Quaternion(Vector4 vec)
        {
            X = vec.X; Y = vec.Y; Z = vec.Z; W = vec.W;
        }

        public Quaternion(Vector3 vec, float scalarPart)
        {
            X = vec.X; Y = vec.Y; Z = vec.Z;
            W = scalarPart;
        }
        #endregion

        #region Operators
        public static Quaternion operator +(Quaternion q1, Quaternion q2)
        {
            return new Quaternion(q1.X + q2.X, q1.Y + q2.Y, q1.Z + q2.Z, q1.W + q2.W);
        }

        public static Quaternion operator -(Quaternion q1, Quaternion q2)
        {
            return new Quaternion(q1.X - q2.X, q1.Y - q2.Y, q1.Z - q2.Z, q1.W - q2.W);
        }

        public static Quaternion operator -(Quaternion q)
        {
            return new Quaternion(-q.X, -q.Y, -q.Z, -q.W);
        }

        public static Quaternion operator *(Quaternion q, float s)
        {
            return new Quaternion(s * q.X, s * q.Y, s * q.Z, s * q.W);
        }

        public static Quaternion operator *(float s, Quaternion q)
        {
            return new Quaternion(s * q.X, s * q.Y, s * q.Z, s * q.W);
        }

        public static Vector3 operator *(Quaternion q, Vector3 v)
        {   // From nVidia SDK
            Vector3 t_uv, t_uuv;
            Vector3 t_qvec = new Vector3(q.X, q.Y, q.Z);
            t_uv = Vector3.Cross(t_qvec, v);
            t_uuv = Vector3.Cross(t_qvec, t_uv);
            t_uv *= 2.0f * q.W;
            t_uuv *= 2.0f;
            return v + t_uv + t_uuv;
        }

        public static Vector4 operator *(Quaternion rotation, Vector4 value)
        {
            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            return new Vector4(
                value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2) + value.Z * (xz2 + wy2),
                value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (yz2 - wx2),
                value.X * (xz2 - wy2) + value.Y * (yz2 + wx2) + value.Z * (1.0f - xx2 - yy2),
                value.W);
        }

        public static Quaternion operator *(Quaternion q1, Quaternion q2)
        {
            return new Quaternion(
               q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y + q1.W * q2.X,
              -q1.X * q2.Z + q1.Y * q2.W + q1.Z * q2.X + q1.W * q2.Y,
               q1.X * q2.Y - q1.Y * q2.X + q1.Z * q2.W + q1.W * q2.Z,
              -q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z + q1.W * q2.W);
        }

        public static Quaternion operator /(Quaternion q, float scalar)
        {
            float invScalar = 1.0f / scalar;
            return new Quaternion(q.X * invScalar, q.Y * invScalar, q.Z * invScalar, q.W * invScalar);
        }

        public static Quaternion operator /(Quaternion q1, Quaternion q2)
        {
            return q1 * q2.Inverse();
        }

        public static bool operator ==(Quaternion q1, Quaternion q2)
        {
            return (q1.W == q2.W && q1.X == q2.X && q1.Y == q2.Y && q1.Z == q2.Z);
        }

        public static bool operator !=(Quaternion q1, Quaternion q2)
        {
            return !(q1.W == q2.W && q1.X == q2.X && q1.Y == q2.Y && q1.Z == q2.Z);
        }
        #endregion

        #region Overrides
        /// <summary>
        /// Returns a string representing this Quaternion.
        /// </summary>
        /// <returns>The string representation.</returns>
        public override string ToString()
        {
            return string.Format("<{0}, {1}, {2}, {3}>", X, Y, Z, W);
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Quaternion)) return false;

            return this.Equals((Quaternion)obj);
        }

        public bool Equals(Quaternion other)
        {
            return this == other;
        }

        /// <summary>
        /// Returns the hash code for this instance.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
        #endregion

        #region Properties
        public Matrix4 Matrix4
        {
            get
            {
                return new Matrix4(
                    new Vector4(1.0f - 2.0f * (Y * Y + Z * Z), 2.0f * (X * Y - W * Z), 2.0f * (X * Z + W * Y), 0.0f),
                    new Vector4(2.0f * (X * Y + W * Z), 1.0f - 2.0f * (X * X + Z * Z), 2.0f * (Y * Z - W * X), 0.0f),
                    new Vector4(2.0f * (X * Z - W * Y), 2.0f * (Y * Z + W * X), 1.0f - 2.0f * (X * X + Y * Y), 0.0f),
                    new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            }
        }
        #endregion

        #region Methods
        /// <summary>
        /// Returns the dot product of the current Quaternion with another.
        /// </summary>
        public float Dot(Quaternion q)
        {
            return (X * q.X) + (Y * q.Y) + (Z * q.Z) + (W * q.W);
        }

        /// <summary>
        /// Calculates the complex conjugate of this Quaternion.
        /// </summary>
        public Quaternion Conjugate()
        {
            Quaternion ans;

            ans.X = -X;
            ans.Y = -Y;
            ans.Z = -Z;
            ans.W = W;

            return ans;
        }

        /// <summary>
        /// Calculates the length squared of the Quaternion. This operation is cheaper than Length().
        /// </summary>
        /// <returns>The length squared of the Quaternion.</returns>
        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z + W * W;
        }

        /// <summary>
        /// Calculates the length of the Quaternion.
        /// </summary>
        /// <returns>The computed length of the Quaternion.</returns>
        public float Length()
        {
            float ls = X * X + Y * Y + Z * Z + W * W;

            return (float)Math.Sqrt((double)ls);
        }

        /// <summary>
        /// Calculates the inverse of this Quaternion.
        /// </summary>
        public Quaternion Inverse()
        {
            Quaternion ans;

            float ls = X * X + Y * Y + Z * Z + W * W;
            float invNorm = 1.0f / ls;

            ans.X = -X * invNorm;
            ans.Y = -Y * invNorm;
            ans.Z = -Z * invNorm;
            ans.W = W * invNorm;

            return ans;
        }

        /// <summary>
        /// Normalizes this Quaternion.
        /// </summary>
        public Quaternion Normalize()
        {
            Quaternion ans;

            float ls = X * X + Y * Y + Z * Z + W * W;

            float invNorm = 1.0f / (float)Math.Sqrt((double)ls);

            ans.X = X * invNorm;
            ans.Y = Y * invNorm;
            ans.Z = Z * invNorm;
            ans.W = W * invNorm;

            return ans;
        }

        /// <summary>
        ///  Linearly interpolates between two quaternions.
        /// </summary>
        /// <param name="quaternion1">The first source Quaternion.</param>
        /// <param name="quaternion2">The second source Quaternion.</param>
        /// <param name="amount">The relative weight of the second source Quaternion in the interpolation.</param>
        /// <returns>The interpolated Quaternion.</returns>
        public static Quaternion Lerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
        {
            float t = amount;
            float t1 = 1.0f - t;

            Quaternion r = new Quaternion();

            float dot = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y +
                        quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;

            if (dot >= 0.0f)
            {
                r.X = t1 * quaternion1.X + t * quaternion2.X;
                r.Y = t1 * quaternion1.Y + t * quaternion2.Y;
                r.Z = t1 * quaternion1.Z + t * quaternion2.Z;
                r.W = t1 * quaternion1.W + t * quaternion2.W;
            }
            else
            {
                r.X = t1 * quaternion1.X - t * quaternion2.X;
                r.Y = t1 * quaternion1.Y - t * quaternion2.Y;
                r.Z = t1 * quaternion1.Z - t * quaternion2.Z;
                r.W = t1 * quaternion1.W - t * quaternion2.W;
            }

            // Normalize it.
            float ls = r.X * r.X + r.Y * r.Y + r.Z * r.Z + r.W * r.W;
            float invNorm = 1.0f / (float)Math.Sqrt((double)ls);

            r.X *= invNorm;
            r.Y *= invNorm;
            r.Z *= invNorm;
            r.W *= invNorm;

            return r;
        }

        /// <summary>
        /// Interpolates between two quaternions, using spherical linear interpolation.
        /// </summary>
        /// <param name="quaternion1">The first source Quaternion.</param>
        /// <param name="quaternion2">The second source Quaternion.</param>
        /// <param name="amount">The relative weight of the second source Quaternion in the interpolation.</param>
        /// <returns>The interpolated Quaternion.</returns>
        public static Quaternion Slerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
        {
            const float epsilon = 1e-6f;

            float t = amount;

            float cosOmega = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y +
                             quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;

            bool flip = false;

            if (cosOmega < 0.0f)
            {
                flip = true;
                cosOmega = -cosOmega;
            }

            float s1, s2;

            if (cosOmega > (1.0f - epsilon))
            {
                // Too close, do straight linear interpolation.
                s1 = 1.0f - t;
                s2 = (flip) ? -t : t;
            }
            else
            {
                float omega = (float)Math.Acos(cosOmega);
                float invSinOmega = (float)(1 / Math.Sin(omega));

                s1 = (float)Math.Sin((1.0f - t) * omega) * invSinOmega;
                s2 = (flip)
                    ? (float)-Math.Sin(t * omega) * invSinOmega
                    : (float)Math.Sin(t * omega) * invSinOmega;
            }

            Quaternion ans;

            ans.X = s1 * quaternion1.X + s2 * quaternion2.X;
            ans.Y = s1 * quaternion1.Y + s2 * quaternion2.Y;
            ans.Z = s1 * quaternion1.Z + s2 * quaternion2.Z;
            ans.W = s1 * quaternion1.W + s2 * quaternion2.W;

            return ans;
        }

        /// <summary>
        /// Creates a Quaternion from a vector and an angle to rotate about the vector.
        /// </summary>
        /// <param name="axis">The vector to rotate around.</param>
        /// <param name="angle">The angle, in radians, to rotate around the vector.</param>
        /// <returns>The created Quaternion.</returns>
        public static Quaternion CreateFromAxisAngle(Vector3 axis, float angle)
        {
            Quaternion ans;

            float halfAngle = angle * 0.5f;
            float s = (float)Math.Sin(halfAngle);
            float c = (float)Math.Cos(halfAngle);

            ans.X = axis.X * s;
            ans.Y = axis.Y * s;
            ans.Z = axis.Z * s;
            ans.W = c;

            return ans;
        }

        /// <summary>
        /// Creates a new Quaternion from the given yaw, pitch, and roll, in radians.
        /// </summary>
        /// <param name="yaw">The yaw angle, in radians, around the Y-axis.</param>
        /// <param name="pitch">The pitch angle, in radians, around the X-axis.</param>
        /// <param name="roll">The roll angle, in radians, around the Z-axis.</param>
        /// <returns></returns>
        public static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        {
            //  Roll first, about axis the object is facing, then
            //  pitch upward, then yaw to face into the new heading
            float sr, cr, sp, cp, sy, cy;

            float halfRoll = roll * 0.5f;
            sr = (float)Math.Sin(halfRoll);
            cr = (float)Math.Cos(halfRoll);

            float halfPitch = pitch * 0.5f;
            sp = (float)Math.Sin(halfPitch);
            cp = (float)Math.Cos(halfPitch);

            float halfYaw = yaw * 0.5f;
            sy = (float)Math.Sin(halfYaw);
            cy = (float)Math.Cos(halfYaw);

            Quaternion result;

            result.X = cy * sp * cr + sy * cp * sr;
            result.Y = sy * cp * cr - cy * sp * sr;
            result.Z = cy * cp * sr - sy * sp * cr;
            result.W = cy * cp * cr + sy * sp * sr;

            return result;
        }

        /// <summary>
        /// Creates a Quaternion from the given rotation matrix.
        /// </summary>
        /// <param name="matrix">The rotation matrix.</param>
        /// <returns>The created Quaternion.</returns>
        public static Quaternion CreateFromRotationMatrix(Matrix4 matrix)
        {
            float trace = matrix.M11 + matrix.M22 + matrix.M33;

            Quaternion q = new Quaternion();

            if (trace > 0.0f)
            {
                float s = (float)Math.Sqrt(trace + 1.0f);
                q.W = s * 0.5f;
                s = 0.5f / s;
                q.X = (matrix.M23 - matrix.M32) * s;
                q.Y = (matrix.M31 - matrix.M13) * s;
                q.Z = (matrix.M12 - matrix.M21) * s;
            }
            else
            {
                if (matrix.M11 >= matrix.M22 && matrix.M11 >= matrix.M33)
                {
                    float s = (float)Math.Sqrt(1.0f + matrix.M11 - matrix.M22 - matrix.M33);
                    float invS = 0.5f / s;
                    q.X = 0.5f * s;
                    q.Y = (matrix.M12 + matrix.M21) * invS;
                    q.Z = (matrix.M13 + matrix.M31) * invS;
                    q.W = (matrix.M23 - matrix.M32) * invS;
                }
                else if (matrix.M22 > matrix.M33)
                {
                    float s = (float)Math.Sqrt(1.0f + matrix.M22 - matrix.M11 - matrix.M33);
                    float invS = 0.5f / s;
                    q.X = (matrix.M21 + matrix.M12) * invS;
                    q.Y = 0.5f * s;
                    q.Z = (matrix.M32 + matrix.M23) * invS;
                    q.W = (matrix.M31 - matrix.M13) * invS;
                }
                else
                {
                    float s = (float)Math.Sqrt(1.0f + matrix.M33 - matrix.M11 - matrix.M22);
                    float invS = 0.5f / s;
                    q.X = (matrix.M31 + matrix.M13) * invS;
                    q.Y = (matrix.M32 + matrix.M23) * invS;
                    q.Z = 0.5f * s;
                    q.W = (matrix.M12 - matrix.M21) * invS;
                }
            }

            return q;
        }
        #endregion
    }
}
