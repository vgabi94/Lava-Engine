using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Matrix3 : IEquatable<Matrix3>
    {
        // For column-major the layout in memory will still be row-major
        // but the multiplication will behave as if the layout is column-major

        #region Public Fields
        public float M11;
        public float M12;
        public float M13;
        public float M21;
        public float M22;
        public float M23;
        public float M31;
        public float M32;
        public float M33;
        #endregion Public Fields

        #region Static Constructors
        private static readonly Matrix3 _identity = new Matrix3
        (
            1f, 0f, 0f,
            0f, 1f, 0f,
            0f, 0f, 1f
        );

        private static readonly Matrix3 _zero = new Matrix3
        (
            0f, 0f, 0f,
            0f, 0f, 0f,
            0f, 0f, 0f
        );

        private static readonly Matrix3 _one = new Matrix3
        (
            1f, 1f, 1f,
            1f, 1f, 1f,
            1f, 1f, 1f
        );

        public static Matrix3 Identity => _identity;
        public static Matrix3 Zero => _zero;
        public static Matrix3 One => _one;
        #endregion

        #region Constructors
        /// <summary>
        /// Constructs the matrix from vectors v1, v2, v3
        /// </summary>
        public Matrix3(Vector3 v1, Vector3 v2, Vector3 v3, bool transpose = false)
            : this (v1.X, v1.Y, v1.Z, v2.X, v2.Y, v2.Z, v3.X, v3.Y, v3.Z, transpose)
        {
        }

        /// <summary>
        /// Constructs the matrix from the array of floats
        /// </summary>
        public Matrix3(float[] values, bool transpose = false)
            : this(values[0], values[1], values[2],
                   values[3], values[4], values[5],
                   values[6], values[7], values[8], transpose) { }

        /// <summary>
        /// Constructs the matrix
        /// </summary>
        public Matrix3(float m11, float m12, float m13,
            float m21, float m22, float m23,
            float m31, float m32, float m33, bool transpose = false)
        {
            if (transpose)
            {
                M11 = m11;
                M12 = m21;
                M13 = m31;
                M21 = m12;
                M22 = m22;
                M23 = m32;
                M31 = m13;
                M32 = m23;
                M33 = m33;
            }
            else
            {
                M11 = m11;
                M12 = m12;
                M13 = m13;
                M21 = m21;
                M22 = m22;
                M23 = m23;
                M31 = m31;
                M32 = m32;
                M33 = m33;
            }
        }
        #endregion Constructors

        #region Properties
        public Vector3 Row1
        {
            get => new Vector3(M11, M12, M13);
            set
            {
                M11 = value.X;
                M12 = value.Y;
                M13 = value.Z;
            }
        }
        public Vector3 Row2
        {
            get => new Vector3(M21, M22, M23);
            set
            {
                M21 = value.X;
                M22 = value.Y;
                M23 = value.Z;
            }
        }
        public Vector3 Row3
        {
            get => new Vector3(M31, M32, M33);
            set
            {
                M31 = value.X;
                M32 = value.Y;
                M33 = value.Z;
            }
        }
        public Vector3 Col1
        {
            get => new Vector3(M11, M21, M31);
            set
            {
                M11 = value.X;
                M21 = value.Y;
                M31 = value.Z;
            }
        }
        public Vector3 Col2
        {
            get => new Vector3(M12, M22, M32);
            set
            {
                M12 = value.X;
                M22 = value.Y;
                M32 = value.Z;
            }
        }
        public Vector3 Col3
        {
            get => new Vector3(M13, M23, M33);
            set
            {
                M13 = value.X;
                M23 = value.Y;
                M33 = value.Z;
            }
        }
        #endregion Properties

        #region Methods
        /// <summary>
        /// Creates a translation matrix.
        /// </summary>
        /// <param name="delta">Amount to translate by in the x, and y directions.</param>
        /// <returns>A Matrix3 object that contains the translation matrix.</returns>
        public static Matrix3 CreateTranslation(Vector2 delta)
        {
            return new Matrix3
            (
                1f, 0f, 0f,
                0f, 1f, 0f,
                delta.X, delta.Y, 1f
            );
        }

        /// <summary>
        /// Creates a translation matrix.
        /// </summary>
        public static Matrix3 CreateTranslation(float X, float Y)
        {
            return new Matrix3
            (
                1f, 0f, 0f,
                0f, 1f, 0f,
                X, Y, 1f
            );
        }

        /// <summary>
        /// Creates a rotation.
        /// </summary>
        /// <param name="radians">Amount to rotate in radians (counter-clockwise).</param>
        /// <returns>A Matrix3 object that contains the rotation matrix.</returns>
        public static Matrix3 CreateRotation(float radians)
        {
            float cos = (float)System.Math.Cos(radians);
            float sin = (float)System.Math.Sin(radians);

            return new Matrix3
            (
                cos, sin, 0f,
                -sin, cos, 0f,
                0f, 0f, 1f
            );
        }

        /// <summary>
        /// Creates a matrix which contains information on how to scale.
        /// </summary>
        /// <param name="scale">Amount to scale by in the x and y direction</param>
        /// <returns>A Matrix3 object that contains the scaling matrix</returns>
        public static Matrix3 CreateScale(Vector2 scale)
        {
            return new Matrix3
            (
                scale.X, 0f, 0f,
                0f, scale.Y, 0f,
                0f, 0f, 1f
            );
        }

        /// <summary>
        /// Creates a uniform scale.
        /// </summary>
        /// <param name="scale">Amount to scale by in the both directions</param>
        /// <returns>A Matrix3 object that contains the scaling matrix</returns>
        public static Matrix3 CreateScale(float scale)
        {
            return new Matrix3
            (
                scale, 0f, 0f,
                0f, scale, 0f,
                0f, 0f, 1f
            );
        }

        /// <summary>
        /// Returns the transpose of this matrix.
        /// </summary>
        /// <returns>A Matrix3 object that contains the transposed matrix</returns>
        public Matrix3 Transpose()
        {
            return new Matrix3
            (   
                M11, M21, M31,
                M12, M22, M32,
                M13, M23, M33
            );
        }

        /// <summary>
        /// Returns the determinant of the 2x2 matrix (row-major).
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Determinant2x2(float a, float b, float c, float d)
        {
            return a * d - b * c;
        }

        /// <summary>
        /// Returns the determinant of the 3x3 matrix (row-major).
        /// </summary>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Determinant3x3(float a, float b, float c, float d,
            float e, float f, float g, float h, float i)
        {
            return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
        }

        /// <summary>
        /// Returns the inverse of this matrix. Returns Zero if non-invertible.
        /// </summary>
        public Matrix3 Inverse()
        {
            float detM = Determinant3x3(M11, M12, M13, M21, M22, M23, M31, M32, M33);
            if (detM != 0f)
            {
                float invDetM = 1 / detM;
                float m11 = invDetM * (M22 * M33 - M23 * M32);
                float m12 = invDetM * (M13 * M32 - M12 * M33);
                float m13 = invDetM * (M12 * M23 - M13 * M22);
                float m21 = invDetM * (M23 * M31 - M21 * M33);
                float m22 = invDetM * (M11 * M33 - M13 * M31);
                float m23 = invDetM * (M13 * M21 - M11 * M23);
                float m31 = invDetM * (M21 * M32 - M22 * M31);
                float m32 = invDetM * (M12 * M31 - M11 * M32);
                float m33 = invDetM * (M11 * M22 - M12 * M21);
                return new Matrix3(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            }
            return _zero;
        }

        /// <summary>
        /// Returns the determinant of this matrix.
        /// </summary>
        public float Determinant()
        {
            return Determinant3x3(M11, M12, M13,
             M21, M22, M23,
             M31, M32, M33);
        }
        #endregion

        #region Operators
        public static Matrix3 operator +(Matrix3 A, Matrix3 B)
        {
            return new Matrix3(A.M11 + B.M11, A.M12 + B.M12, A.M13 + B.M13,
                A.M21 + B.M21, A.M22 + B.M22, A.M23 + B.M23,
                A.M31 + B.M31, A.M32 + B.M32, A.M33 + B.M33);
        }

        public static Matrix3 operator -(Matrix3 A, Matrix3 B)
        {
            return new Matrix3(A.M11 - B.M11, A.M12 - B.M12, A.M13 - B.M13,
                A.M21 - B.M21, A.M22 - B.M22, A.M23 - B.M23,
                A.M31 - B.M31, A.M32 - B.M32, A.M33 - B.M33);
        }

#if COLUMN_MAJOR
        public static Matrix3 operator *(Matrix3 A, Matrix3 B)
        {
            Matrix3 m;

            // First row
            m.M11 = A.M11 * B.M11 + A.M21 * B.M12 + A.M31 * B.M13;
            m.M12 = A.M12 * B.M11 + A.M22 * B.M12 + A.M32 * B.M13;
            m.M13 = A.M13 * B.M11 + A.M23 * B.M12 + A.M33 * B.M13;

            // Second row
            m.M21 = A.M11 * B.M21 + A.M21 * B.M22 + A.M31 * B.M23;
            m.M22 = A.M12 * B.M21 + A.M22 * B.M22 + A.M32 * B.M23;
            m.M23 = A.M13 * B.M21 + A.M23 * B.M22 + A.M33 * B.M23;

            // Third row
            m.M31 = A.M11 * B.M31 + A.M21 * B.M32 + A.M31 * B.M33;
            m.M32 = A.M12 * B.M31 + A.M22 * B.M32 + A.M32 * B.M33;
            m.M33 = A.M13 * B.M31 + A.M23 * B.M32 + A.M33 * B.M33;

            return m;
        }
#else
        public static Matrix3 operator *(Matrix3 A, Matrix3 B)
        {
            Matrix3 m;

            // First row
            m.M11 = A.M11 * B.M11 + A.M12 * B.M21 + A.M13 * B.M31;
            m.M12 = A.M11 * B.M12 + A.M12 * B.M22 + A.M13 * B.M32;
            m.M13 = A.M11 * B.M13 + A.M12 * B.M23 + A.M13 * B.M33;

            // Second row
            m.M21 = A.M21 * B.M11 + A.M22 * B.M21 + A.M23 * B.M31;
            m.M22 = A.M21 * B.M12 + A.M22 * B.M22 + A.M23 * B.M32;
            m.M23 = A.M21 * B.M13 + A.M22 * B.M23 + A.M23 * B.M33;

            // Third row
            m.M31 = A.M31 * B.M11 + A.M32 * B.M21 + A.M33 * B.M31;
            m.M32 = A.M31 * B.M12 + A.M32 * B.M22 + A.M33 * B.M32;
            m.M33 = A.M31 * B.M13 + A.M32 * B.M23 + A.M33 * B.M33;

            return m;
        }
#endif

        public static Matrix3 operator *(Matrix3 M, float d)
        {
            return new Matrix3(M.M11 * d, M.M12 * d, M.M13 * d,
                               M.M21 * d, M.M22 * d, M.M23 * d,
                               M.M31 * d, M.M32 * d, M.M33 * d);
        }

        public static Matrix3 operator *(float d, Matrix3 M)
        {
            return new Matrix3(M.M11 * d, M.M12 * d, M.M13 * d,
                               M.M21 * d, M.M22 * d, M.M23 * d,
                               M.M31 * d, M.M32 * d, M.M33 * d);
        }

        public static Matrix3 operator /(Matrix3 M, float d)
        {
            return new Matrix3(M.M11 / d, M.M12 / d, M.M13 / d,
                               M.M21 / d, M.M22 / d, M.M23 / d,
                               M.M31 / d, M.M32 / d, M.M33 / d);
        }

        public static Matrix3 operator /(float d, Matrix3 M)
        {
            return new Matrix3(d / M.M11, d / M.M12, d / M.M13,
                               d / M.M21, d / M.M22, d / M.M23,
                               d / M.M31, d / M.M32, d / M.M33);
        }
#if COLUMN_MAJOR
        public static Vector3 operator *(Matrix3 M, Vector3 v)
        {
            return new Vector3(v.X * M.M11 + v.Y * M.M21 + v.Z * M.M31,
                v.X * M.M12 + v.Y * M.M22 + v.Z * M.M32,
                v.X * M.M13 + v.Y * M.M23 + v.Z * M.M33);
        }

        public static Vector3 operator *(Vector3 v, Matrix3 M)
        {
            return new Vector3(M.M11 * v.X + M.M12 * v.Y + M.M13 * v.Z,
                M.M21 * v.X + M.M22 * v.Y + M.M23 * v.Z,
                M.M31 * v.X + M.M32 * v.Y + M.M33 * v.Z);
        }
#else
        public static Vector3 operator *(Matrix3 M, Vector3 v)
        {
            return new Vector3(M.M11 * v.X + M.M12 * v.Y + M.M13 * v.Z,
                M.M21 * v.X + M.M22 * v.Y + M.M23 * v.Z,
                M.M31 * v.X + M.M32 * v.Y + M.M33 * v.Z);
        }

        public static Vector3 operator *(Vector3 v, Matrix3 M)
        {
            return new Vector3(v.X * M.M11 + v.Y * M.M21 + v.Z * M.M31,
                v.X * M.M12 + v.Y * M.M22 + v.Z * M.M32,
                v.X * M.M13 + v.Y * M.M23 + v.Z * M.M33);
        }
#endif

        public static bool operator ==(Matrix3 A, Matrix3 B)
        {
            // Check diagonal element first for early out.
            return (A.M11 == B.M11 && A.M22 == B.M22 && A.M33 == B.M33 &&
                    A.M12 == B.M12 && A.M13 == B.M13 &&
                    A.M21 == B.M21 && A.M23 == B.M23 &&
                    A.M31 == B.M31 && A.M32 == B.M32);
        }

        public static bool operator !=(Matrix3 A, Matrix3 B)
        {
            return (A.M11 != B.M11 || A.M12 != B.M12 || A.M13 != B.M13 ||
                    A.M21 != B.M21 || A.M22 != B.M22 || A.M23 != B.M23 ||
                    A.M31 != B.M31 || A.M32 != B.M32 || A.M33 != B.M33);
        }

        public override bool Equals(object obj)
        {
            if (obj is Matrix3)
            {
                return Equals((Matrix3)obj);
            }

            return false;
        }

        public bool Equals(Matrix3 B)
        {
            // Check diagonal element first for early out.
            return (M11 == B.M11 && M22 == B.M22 && M33 == B.M33 &&
                    M12 == B.M12 && M13 == B.M13 &&
                    M21 == B.M21 && M23 == B.M23 &&
                    M31 == B.M31 && M32 == B.M32);
        }

        public override int GetHashCode()
        {
            return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() +
                   M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() +
                   M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode();
        }

        public override string ToString()
        {
            return "{ " + Row1.ToString() + ", " + Row2.ToString() + ", " + Row3.ToString() + " }";
        }
        #endregion
    }
}