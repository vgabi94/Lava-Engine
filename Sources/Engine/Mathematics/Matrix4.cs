using System;
using System.Runtime.InteropServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Matrix4 : IEquatable<Matrix4>
    {
        // For column-major the layout in memory will still be row-major
        // but the multiplication will behave as if the layout is column-major
        // Some implementations were taken from
        // https://referencesource.microsoft.com/#System.Numerics/System/Numerics/Matrix4x4.cs,2efa39a9852658e5

        #region Public Fields
        public float M11;
        public float M12;
        public float M13;
        public float M14;
        public float M21;
        public float M22;
        public float M23;
        public float M24;
        public float M31;
        public float M32;
        public float M33;
        public float M34;
        public float M41;
        public float M42;
        public float M43;
        public float M44;
        #endregion Public Fields

        #region Static Constructors
        private static readonly Matrix4 _identity = new Matrix4
        (
            1f, 0f, 0f, 0f,
            0f, 1f, 0f, 0f,
            0f, 0f, 1f, 0f,
            0f, 0f, 0f, 1f
        );

        private static readonly Matrix4 _zero = new Matrix4
        (
            0f, 0f, 0f, 0f,
            0f, 0f, 0f, 0f,
            0f, 0f, 0f, 0f,
            0f, 0f, 0f, 0f
        );

        private static readonly Matrix4 _one = new Matrix4
        (
            1f, 1f, 1f, 1f,
            1f, 1f, 1f, 1f,
            1f, 1f, 1f, 1f,
            1f, 1f, 1f, 1f
        );

        public static Matrix4 Identity => _identity;
        public static Matrix4 Zero => _zero;
        public static Matrix4 One => _one;
        #endregion

        #region Constructors
        /// <summary>
        /// Constructs the matrix from vectors v1, v2, v3
        /// </summary>
        public Matrix4(Vector4 v1, Vector4 v2, Vector4 v3, Vector4 v4, bool transpose = false)
            : this(v1.X, v1.Y, v1.Z, v1.W, v2.X, v2.Y, v2.Z, v2.W, v3.X, v3.Y, v3.Z, v3.W, v4.X, v4.Y, v4.Z, v4.W, transpose)
        {
        }

        /// <summary>
        /// Constructs the matrix from the array of floats
        /// </summary>
        public Matrix4(float[] values, bool transpose = false)
            : this(values[0], values[1], values[2], values[3],
                   values[4], values[5], values[6], values[7],
                   values[8], values[9], values[10], values[11],
                   values[12], values[13], values[14], values[15], transpose)
        { }

        /// <summary>
        /// Constructs the matrix
        /// </summary>
        public Matrix4(float m11, float m12, float m13, float m14,
            float m21, float m22, float m23, float m24,
            float m31, float m32, float m33, float m34,
            float m41, float m42, float m43, float m44, bool transpose = false)
        {
            if (transpose)
            {
                M11 = m11;
                M12 = m21;
                M13 = m31;
                M14 = m41;
                M21 = m12;
                M22 = m22;
                M23 = m32;
                M24 = m42;
                M31 = m13;
                M32 = m23;
                M33 = m33;
                M34 = m43;
                M41 = m14;
                M42 = m24;
                M43 = m34;
                M44 = m44;
            }
            else
            {
                M11 = m11;
                M12 = m12;
                M13 = m13;
                M14 = m14;
                M21 = m21;
                M22 = m22;
                M23 = m23;
                M24 = m24;
                M31 = m31;
                M32 = m32;
                M33 = m33;
                M34 = m34;
                M41 = m41;
                M42 = m42;
                M43 = m43;
                M44 = m44;
            }
        }

        public Matrix4(Matrix3 m) : this(m.M11, m.M12, m.M13, 0f, m.M21, m.M22, m.M23, 0f, m.M31, m.M32, m.M33, 0f, 0f, 0f, 0f, 1f) { }
        #endregion

        #region Properties
        public Vector4 Row1
        {
            get => new Vector4(M11, M12, M13, M14);
            set
            {
                M11 = value.X;
                M12 = value.Y;
                M13 = value.Z;
                M14 = value.W;
            }
        }
        public Vector4 Row2
        {
            get => new Vector4(M21, M22, M23, M24);
            set
            {
                M21 = value.X;
                M22 = value.Y;
                M23 = value.Z;
                M24 = value.W;
            }
        }
        public Vector4 Row3
        {
            get => new Vector4(M31, M32, M33, M34);
            set
            {
                M31 = value.X;
                M32 = value.Y;
                M33 = value.Z;
                M34 = value.W;
            }
        }
        public Vector4 Row4
        {
            get => new Vector4(M41, M42, M43, M44);
            set
            {
                M41 = value.X;
                M42 = value.Y;
                M43 = value.Z;
                M44 = value.W;
            }
        }
        public Vector4 Col1
        {
            get => new Vector4(M11, M21, M31, M41);
            set
            {
                M11 = value.X;
                M21 = value.Y;
                M31 = value.Z;
                M41 = value.W;
            }
        }
        public Vector4 Col2
        {
            get => new Vector4(M12, M22, M32, M42);
            set
            {
                M12 = value.X;
                M22 = value.Y;
                M32 = value.Z;
                M42 = value.W;
            }
        }
        public Vector4 Col3
        {
            get => new Vector4(M13, M23, M33, M43);
            set
            {
                M13 = value.X;
                M23 = value.Y;
                M33 = value.Z;
                M43 = value.W;
            }
        }
        public Vector4 Col4
        {
            get => new Vector4(M14, M24, M34, M44);
            set
            {
                M14 = value.X;
                M24 = value.Y;
                M34 = value.Z;
                M44 = value.W;
            }
        }
        public Matrix3 Upper3x3
        {
            get => new Matrix3(M11, M12, M13, M21, M22, M23, M31, M32, M33);
            set
            {
                M11 = value.M11;
                M12 = value.M12;
                M13 = value.M13;
                M21 = value.M21;
                M22 = value.M22;
                M23 = value.M23;
                M31 = value.M31;
                M32 = value.M32;
                M33 = value.M33;
            }
        }
        #endregion Properties

        #region Methods
        /// <summary>
        /// Creates a translation matrix.
        /// </summary>
        /// <param name="position">The amount to translate in each axis.</param>
        /// <returns>The translation matrix.</returns>
        public static Matrix4 CreateTranslation(Vector3 delta)
        {
            Matrix4 result;

            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;

            result.M41 = delta.X;
            result.M42 = delta.Y;
            result.M43 = delta.Z;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a translation matrix.
        /// </summary>
        /// <param name="xPosition">The amount to translate on the X-axis.</param>
        /// <param name="yPosition">The amount to translate on the Y-axis.</param>
        /// <param name="zPosition">The amount to translate on the Z-axis.</param>
        /// <returns>The translation matrix.</returns>
        public static Matrix4 CreateTranslation(float xPosition, float yPosition, float zPosition)
        {
            Matrix4 result;

            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;

            result.M41 = xPosition;
            result.M42 = yPosition;
            result.M43 = zPosition;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix for rotating points around the X-axis.
        /// </summary>
        /// <param name="radians">The amount, in radians, by which to rotate around the X-axis.</param>
        /// <returns>The rotation matrix.</returns>
        public static Matrix4 CreateRotationX(float radians)
        {
            Matrix4 result;

            float c = (float)Math.Cos(radians);
            float s = (float)Math.Sin(radians);

            // [  1  0  0  0 ]
            // [  0  c  s  0 ]
            // [  0 -s  c  0 ]
            // [  0  0  0  1 ]
            result.M11 = 1.0f;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = c;
            result.M23 = s;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = -s;
            result.M33 = c;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix for rotating points around the Y-axis.
        /// </summary>
        /// <param name="radians">The amount, in radians, by which to rotate around the Y-axis.</param>
        /// <returns>The rotation matrix.</returns>
        public static Matrix4 CreateRotationY(float radians)
        {
            Matrix4 result;

            float c = (float)Math.Cos(radians);
            float s = (float)Math.Sin(radians);

            // [  c  0 -s  0 ]
            // [  0  1  0  0 ]
            // [  s  0  c  0 ]
            // [  0  0  0  1 ]
            result.M11 = c;
            result.M12 = 0.0f;
            result.M13 = -s;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = 1.0f;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = s;
            result.M32 = 0.0f;
            result.M33 = c;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix for rotating points around the Z-axis.
        /// </summary>
        /// <param name="radians">The amount, in radians, by which to rotate around the Z-axis.</param>
        /// <returns>The rotation matrix.</returns>
        public static Matrix4 CreateRotationZ(float radians)
        {
            Matrix4 result;

            float c = (float)Math.Cos(radians);
            float s = (float)Math.Sin(radians);

            // [  c  s  0  0 ]
            // [ -s  c  0  0 ]
            // [  0  0  1  0 ]
            // [  0  0  0  1 ]
            result.M11 = c;
            result.M12 = s;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = -s;
            result.M22 = c;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = 1.0f;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a matrix that rotates around an arbitrary vector.
        /// </summary>
        /// <param name="axis">The axis to rotate around.</param>
        /// <param name="angle">The angle to rotate around the given axis, in radians.</param>
        /// <returns>The rotation matrix.</returns>
        public static Matrix4 CreateFromAxisAngle(Vector3 axis, float angle)
        {
            // a: angle
            // x, y, z: unit vector for axis.
            //
            // Rotation matrix M can compute by using below equation.
            //
            //        T               T
            //  M = uu + (cos a)( I-uu ) + (sin a)S
            //
            // Where:
            //
            //  u = ( x, y, z )
            //
            //      [  0 -z  y ]
            //  S = [  z  0 -x ]
            //      [ -y  x  0 ]
            //
            //      [ 1 0 0 ]
            //  I = [ 0 1 0 ]
            //      [ 0 0 1 ]
            //
            //
            //     [  xx+cosa*(1-xx)   yx-cosa*yx-sina*z zx-cosa*xz+sina*y ]
            // M = [ xy-cosa*yx+sina*z    yy+cosa(1-yy)  yz-cosa*yz-sina*x ]
            //     [ zx-cosa*zx-sina*y zy-cosa*zy+sina*x   zz+cosa*(1-zz)  ]
            //
            float x = axis.X, y = axis.Y, z = axis.Z;
            float sa = (float)Math.Sin(angle), ca = (float)Math.Cos(angle);
            float xx = x * x, yy = y * y, zz = z * z;
            float xy = x * y, xz = x * z, yz = y * z;

            Matrix4 result;

            result.M11 = xx + ca * (1.0f - xx);
            result.M12 = xy - ca * xy + sa * z;
            result.M13 = xz - ca * xz - sa * y;
            result.M14 = 0.0f;
            result.M21 = xy - ca * xy - sa * z;
            result.M22 = yy + ca * (1.0f - yy);
            result.M23 = yz - ca * yz + sa * x;
            result.M24 = 0.0f;
            result.M31 = xz - ca * xz + sa * y;
            result.M32 = yz - ca * yz - sa * x;
            result.M33 = zz + ca * (1.0f - zz);
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a scaling matrix.
        /// </summary>
        /// <param name="scales">The vector containing the amount to scale by on each axis.</param>
        /// <returns>The scaling matrix.</returns>
        public static Matrix4 CreateScale(Vector3 scales)
        {
            Matrix4 result;

            result.M11 = scales.X;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = scales.Y;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = scales.Z;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a scaling matrix.
        /// </summary>
        /// <param name="xScale">Value to scale by on the X-axis.</param>
        /// <param name="yScale">Value to scale by on the Y-axis.</param>
        /// <param name="zScale">Value to scale by on the Z-axis.</param>
        /// <returns>The scaling matrix.</returns>
        public static Matrix4 CreateScale(float xScale, float yScale, float zScale)
        {
            Matrix4 result;

            result.M11 = xScale;
            result.M12 = 0.0f;
            result.M13 = 0.0f;
            result.M14 = 0.0f;
            result.M21 = 0.0f;
            result.M22 = yScale;
            result.M23 = 0.0f;
            result.M24 = 0.0f;
            result.M31 = 0.0f;
            result.M32 = 0.0f;
            result.M33 = zScale;
            result.M34 = 0.0f;
            result.M41 = 0.0f;
            result.M42 = 0.0f;
            result.M43 = 0.0f;
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a perspective projection matrix based on a field of view, aspect ratio, and near and far view plane distances. 
        /// </summary>
        /// <param name="fieldOfView">Field of view in the y direction, in radians.</param>
        /// <param name="aspectRatio">Aspect ratio, defined as view space width divided by height.</param>
        /// <param name="nearPlaneDistance">Distance to the near view plane.</param>
        /// <param name="farPlaneDistance">Distance to the far view plane.</param>
        /// <returns>The perspective projection matrix.</returns>
        public static Matrix4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        {
            if (fieldOfView <= 0.0f || fieldOfView >= Math.PI)
                throw new ArgumentOutOfRangeException("fieldOfView");

            if (nearPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            if (farPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("farPlaneDistance");

            if (nearPlaneDistance >= farPlaneDistance)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            float yScale = 1.0f / (float)Math.Tan(fieldOfView * 0.5f);
            float xScale = yScale / aspectRatio;

            Matrix4 result;

            result.M11 = xScale;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = yScale;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M31 = result.M32 = 0.0f;
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1.0f;

            result.M41 = result.M42 = result.M44 = 0.0f;
            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

            return result;
        }

        /// <summary>
        /// Creates a perspective projection matrix from the given view volume dimensions.
        /// </summary>
        /// <param name="width">Width of the view volume at the near view plane.</param>
        /// <param name="height">Height of the view volume at the near view plane.</param>
        /// <param name="nearPlaneDistance">Distance to the near view plane.</param>
        /// <param name="farPlaneDistance">Distance to the far view plane.</param>
        /// <returns>The perspective projection matrix.</returns>
        public static Matrix4 CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        {
            if (nearPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            if (farPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("farPlaneDistance");

            if (nearPlaneDistance >= farPlaneDistance)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            Matrix4 result;

            result.M11 = 2.0f * nearPlaneDistance / width;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f * nearPlaneDistance / height;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M31 = result.M32 = 0.0f;
            result.M34 = -1.0f;

            result.M41 = result.M42 = result.M44 = 0.0f;
            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

            return result;
        }

        /// <summary>
        /// Creates a customized, perspective projection matrix.
        /// </summary>
        /// <param name="left">Minimum x-value of the view volume at the near view plane.</param>
        /// <param name="right">Maximum x-value of the view volume at the near view plane.</param>
        /// <param name="bottom">Minimum y-value of the view volume at the near view plane.</param>
        /// <param name="top">Maximum y-value of the view volume at the near view plane.</param>
        /// <param name="nearPlaneDistance">Distance to the near view plane.</param>
        /// <param name="farPlaneDistance">Distance to of the far view plane.</param>
        /// <returns>The perspective projection matrix.</returns>
        public static Matrix4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
        {
            if (nearPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            if (farPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("farPlaneDistance");

            if (nearPlaneDistance >= farPlaneDistance)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            Matrix4 result;

            result.M11 = 2.0f * nearPlaneDistance / (right - left);
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f * nearPlaneDistance / (top - bottom);
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M31 = (left + right) / (right - left);
            result.M32 = (top + bottom) / (top - bottom);
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1.0f;

            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M41 = result.M42 = result.M44 = 0.0f;

            return result;
        }

        /// <summary>
        /// Creates an orthographic perspective matrix from the given view volume dimensions.
        /// </summary>
        /// <param name="width">Width of the view volume.</param>
        /// <param name="height">Height of the view volume.</param>
        /// <param name="zNearPlane">Minimum Z-value of the view volume.</param>
        /// <param name="zFarPlane">Maximum Z-value of the view volume.</param>
        /// <returns>The orthographic projection matrix.</returns>
        public static Matrix4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
        {
            Matrix4 result;

            result.M11 = 2.0f / width;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f / height;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = 1.0f / (zNearPlane - zFarPlane);
            result.M31 = result.M32 = result.M34 = 0.0f;

            result.M41 = result.M42 = 0.0f;
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Builds a customized, orthographic projection matrix.
        /// </summary>
        /// <param name="left">Minimum X-value of the view volume.</param>
        /// <param name="right">Maximum X-value of the view volume.</param>
        /// <param name="bottom">Minimum Y-value of the view volume.</param>
        /// <param name="top">Maximum Y-value of the view volume.</param>
        /// <param name="zNearPlane">Minimum Z-value of the view volume.</param>
        /// <param name="zFarPlane">Maximum Z-value of the view volume.</param>
        /// <returns>The orthographic projection matrix.</returns>
        public static Matrix4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
        {
            Matrix4 result;

            result.M11 = 2.0f / (right - left);
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f / (top - bottom);
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = 1.0f / (zNearPlane - zFarPlane);
            result.M31 = result.M32 = result.M34 = 0.0f;

            result.M41 = (left + right) / (left - right);
            result.M42 = (top + bottom) / (bottom - top);
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Creates a view matrix.
        /// </summary>
        /// <param name="cameraPosition">The position of the camera.</param>
        /// <param name="cameraTarget">The target towards which the camera is pointing.</param>
        /// <param name="cameraUpVector">The direction that is "up" from the camera's point of view.</param>
        /// <returns>The view matrix.</returns>
        public static Matrix4 LookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector)
        {
            Vector3 zaxis = (cameraPosition - cameraTarget).Normalize();
            Vector3 xaxis = (Vector3.Cross(cameraUpVector, zaxis)).Normalize();
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);

            Matrix4 result;

            result.M11 = xaxis.X;
            result.M12 = yaxis.X;
            result.M13 = zaxis.X;
            result.M14 = 0.0f;
            result.M21 = xaxis.Y;
            result.M22 = yaxis.Y;
            result.M23 = zaxis.Y;
            result.M24 = 0.0f;
            result.M31 = xaxis.Z;
            result.M32 = yaxis.Z;
            result.M33 = zaxis.Z;
            result.M34 = 0.0f;
            result.M41 = -Vector3.Dot(xaxis, cameraPosition);
            result.M42 = -Vector3.Dot(yaxis, cameraPosition);
            result.M43 = -Vector3.Dot(zaxis, cameraPosition);
            result.M44 = 1.0f;

            return result;
        }

        /// <summary>
        /// Transposes the rows and columns of this matrix.
        /// </summary>
        /// <returns>The transposed matrix.</returns>
        public Matrix4 Transpose()
        {
            Matrix4 result;

            result.M11 = this.M11;
            result.M12 = this.M21;
            result.M13 = this.M31;
            result.M14 = this.M41;
            result.M21 = this.M12;
            result.M22 = this.M22;
            result.M23 = this.M32;
            result.M24 = this.M42;
            result.M31 = this.M13;
            result.M32 = this.M23;
            result.M33 = this.M33;
            result.M34 = this.M43;
            result.M41 = this.M14;
            result.M42 = this.M24;
            result.M43 = this.M34;
            result.M44 = this.M44;

            return result;
        }

        /// <summary>
        /// Calculates the determinant of the matrix.
        /// </summary>
        /// <returns>The determinant of the matrix.</returns>
        public float Determinant()
        {
            // Cost of operation
            // 17 adds and 28 muls.
            // add: 6 + 8 + 3 = 17
            // mul: 12 + 16 = 28

            float a = M11, b = M12, c = M13, d = M14;
            float e = M21, f = M22, g = M23, h = M24;
            float i = M31, j = M32, k = M33, l = M34;
            float m = M41, n = M42, o = M43, p = M44;

            float kp_lo = k * p - l * o;
            float jp_ln = j * p - l * n;
            float jo_kn = j * o - k * n;
            float ip_lm = i * p - l * m;
            float io_km = i * o - k * m;
            float in_jm = i * n - j * m;

            return a * (f * kp_lo - g * jp_ln + h * jo_kn) -
                   b * (e * kp_lo - g * ip_lm + h * io_km) +
                   c * (e * jp_ln - f * ip_lm + h * in_jm) -
                   d * (e * jo_kn - f * io_km + g * in_jm);
        }

        /// <summary>
        /// Attempts to calculate the inverse of this matrix. If successful, result will contain the inverted matrix.
        /// </summary>
        /// <returns>The inverse of this matrix or Zero if non-invertible.</returns>
        public Matrix4 Inverse()
        {
            // Cost of operation
            // 53 adds, 104 muls, and 1 div.
            Matrix4 result;

            float a = this.M11, b = this.M12, c = this.M13, d = this.M14;
            float e = this.M21, f = this.M22, g = this.M23, h = this.M24;
            float i = this.M31, j = this.M32, k = this.M33, l = this.M34;
            float m = this.M41, n = this.M42, o = this.M43, p = this.M44;

            float kp_lo = k * p - l * o;
            float jp_ln = j * p - l * n;
            float jo_kn = j * o - k * n;
            float ip_lm = i * p - l * m;
            float io_km = i * o - k * m;
            float in_jm = i * n - j * m;

            float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
            float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
            float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
            float a14 = -(e * jo_kn - f * io_km + g * in_jm);

            float det = a * a11 + b * a12 + c * a13 + d * a14;

            if (Math.Abs(det) < float.Epsilon)
            {
                return _zero;
            }

            float invDet = 1.0f / det;

            result.M11 = a11 * invDet;
            result.M21 = a12 * invDet;
            result.M31 = a13 * invDet;
            result.M41 = a14 * invDet;

            result.M12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
            result.M22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
            result.M32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
            result.M42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

            float gp_ho = g * p - h * o;
            float fp_hn = f * p - h * n;
            float fo_gn = f * o - g * n;
            float ep_hm = e * p - h * m;
            float eo_gm = e * o - g * m;
            float en_fm = e * n - f * m;

            result.M13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
            result.M23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
            result.M33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
            result.M43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

            float gl_hk = g * l - h * k;
            float fl_hj = f * l - h * j;
            float fk_gj = f * k - g * j;
            float el_hi = e * l - h * i;
            float ek_gi = e * k - g * i;
            float ej_fi = e * j - f * i;

            result.M14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
            result.M24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
            result.M34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
            result.M44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

            return result;
        }

        /// <summary>
        /// Linearly interpolates between the corresponding values of two matrices.
        /// </summary>
        /// <param name="matrix1">The first source matrix.</param>
        /// <param name="matrix2">The second source matrix.</param>
        /// <param name="amount">The relative weight of the second source matrix.</param>
        /// <returns>The interpolated matrix.</returns>
        public static Matrix4 Lerp(Matrix4 matrix1, Matrix4 matrix2, float amount)
        {
            Matrix4 result;

            // First row
            result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
            result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
            result.M13 = matrix1.M13 + (matrix2.M13 - matrix1.M13) * amount;
            result.M14 = matrix1.M14 + (matrix2.M14 - matrix1.M14) * amount;

            // Second row
            result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
            result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
            result.M23 = matrix1.M23 + (matrix2.M23 - matrix1.M23) * amount;
            result.M24 = matrix1.M24 + (matrix2.M24 - matrix1.M24) * amount;

            // Third row
            result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
            result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;
            result.M33 = matrix1.M33 + (matrix2.M33 - matrix1.M33) * amount;
            result.M34 = matrix1.M34 + (matrix2.M34 - matrix1.M34) * amount;

            // Fourth row
            result.M41 = matrix1.M41 + (matrix2.M41 - matrix1.M41) * amount;
            result.M42 = matrix1.M42 + (matrix2.M42 - matrix1.M42) * amount;
            result.M43 = matrix1.M43 + (matrix2.M43 - matrix1.M43) * amount;
            result.M44 = matrix1.M44 + (matrix2.M44 - matrix1.M44) * amount;

            return result;
        }

        /// <summary>
        /// Creates a Matrix that reflects the coordinate system about a specified Plane.
        /// </summary>
        /// <param name="value">The Plane about which to create a reflection.</param>
        /// <returns>A new matrix expressing the reflection.</returns>
        public static Matrix4 CreateReflection(Plane value)
        {
            value = value.Normalize();

            float a = value.Normal.X;
            float b = value.Normal.Y;
            float c = value.Normal.Z;

            float fa = -2.0f * a;
            float fb = -2.0f * b;
            float fc = -2.0f * c;

            Matrix4 result;

            result.M11 = fa * a + 1.0f;
            result.M12 = fb * a;
            result.M13 = fc * a;
            result.M14 = 0.0f;

            result.M21 = fa * b;
            result.M22 = fb * b + 1.0f;
            result.M23 = fc * b;
            result.M24 = 0.0f;

            result.M31 = fa * c;
            result.M32 = fb * c;
            result.M33 = fc * c + 1.0f;
            result.M34 = 0.0f;

            result.M41 = fa * value.D;
            result.M42 = fb * value.D;
            result.M43 = fc * value.D;
            result.M44 = 1.0f;

            return result;
        }
        #endregion

        #region Operators
        public static Matrix4 operator +(Matrix4 value1, Matrix4 value2)
        {
            Matrix4 result;

            result.M11 = value1.M11 + value2.M11;
            result.M12 = value1.M12 + value2.M12;
            result.M13 = value1.M13 + value2.M13;
            result.M14 = value1.M14 + value2.M14;
            result.M21 = value1.M21 + value2.M21;
            result.M22 = value1.M22 + value2.M22;
            result.M23 = value1.M23 + value2.M23;
            result.M24 = value1.M24 + value2.M24;
            result.M31 = value1.M31 + value2.M31;
            result.M32 = value1.M32 + value2.M32;
            result.M33 = value1.M33 + value2.M33;
            result.M34 = value1.M34 + value2.M34;
            result.M41 = value1.M41 + value2.M41;
            result.M42 = value1.M42 + value2.M42;
            result.M43 = value1.M43 + value2.M43;
            result.M44 = value1.M44 + value2.M44;

            return result;
        }

        public static Matrix4 operator -(Matrix4 value1, Matrix4 value2)
        {
            Matrix4 result;

            result.M11 = value1.M11 - value2.M11;
            result.M12 = value1.M12 - value2.M12;
            result.M13 = value1.M13 - value2.M13;
            result.M14 = value1.M14 - value2.M14;
            result.M21 = value1.M21 - value2.M21;
            result.M22 = value1.M22 - value2.M22;
            result.M23 = value1.M23 - value2.M23;
            result.M24 = value1.M24 - value2.M24;
            result.M31 = value1.M31 - value2.M31;
            result.M32 = value1.M32 - value2.M32;
            result.M33 = value1.M33 - value2.M33;
            result.M34 = value1.M34 - value2.M34;
            result.M41 = value1.M41 - value2.M41;
            result.M42 = value1.M42 - value2.M42;
            result.M43 = value1.M43 - value2.M43;
            result.M44 = value1.M44 - value2.M44;

            return result;
        }

        public static Matrix4 operator -(Matrix4 value)
        {
            Matrix4 m;

            m.M11 = -value.M11;
            m.M12 = -value.M12;
            m.M13 = -value.M13;
            m.M14 = -value.M14;
            m.M21 = -value.M21;
            m.M22 = -value.M22;
            m.M23 = -value.M23;
            m.M24 = -value.M24;
            m.M31 = -value.M31;
            m.M32 = -value.M32;
            m.M33 = -value.M33;
            m.M34 = -value.M34;
            m.M41 = -value.M41;
            m.M42 = -value.M42;
            m.M43 = -value.M43;
            m.M44 = -value.M44;

            return m;
        }

#if COLUMN_MAJOR
        public static Matrix4 operator *(Matrix4 A, Matrix4 B)
        {
            Matrix4 result;

            // First row
            result.M11 = A.M11 * B.M11 + A.M21 * B.M12 + A.M31 * B.M13 + A.M41 * B.M14;
            result.M12 = A.M12 * B.M11 + A.M22 * B.M12 + A.M32 * B.M13 + A.M42 * B.M14;
            result.M13 = A.M13 * B.M11 + A.M23 * B.M12 + A.M33 * B.M13 + A.M43 * B.M14;
            result.M14 = A.M14 * B.M11 + A.M24 * B.M12 + A.M34 * B.M13 + A.M44 * B.M14;

            // Second row
            result.M21 = A.M11 * B.M21 + A.M21 * B.M22 + A.M31 * B.M23 + A.M41 * B.M24;
            result.M22 = A.M12 * B.M21 + A.M22 * B.M22 + A.M32 * B.M23 + A.M42 * B.M24;
            result.M23 = A.M13 * B.M21 + A.M23 * B.M22 + A.M33 * B.M23 + A.M43 * B.M24;
            result.M24 = A.M14 * B.M21 + A.M24 * B.M22 + A.M34 * B.M23 + A.M44 * B.M24;

            // Third row
            result.M31 = A.M11 * B.M31 + A.M21 * B.M32 + A.M31 * B.M33 + A.M41 * B.M34;
            result.M32 = A.M12 * B.M31 + A.M22 * B.M32 + A.M32 * B.M33 + A.M42 * B.M34;
            result.M33 = A.M13 * B.M31 + A.M23 * B.M32 + A.M33 * B.M33 + A.M43 * B.M34;
            result.M34 = A.M14 * B.M31 + A.M24 * B.M32 + A.M34 * B.M33 + A.M44 * B.M34;

            // Fourth row
            result.M41 = A.M11 * B.M41 + A.M21 * B.M42 + A.M31 * B.M43 + A.M41 * B.M44;
            result.M42 = A.M12 * B.M41 + A.M22 * B.M42 + A.M32 * B.M43 + A.M42 * B.M44;
            result.M43 = A.M13 * B.M41 + A.M23 * B.M42 + A.M33 * B.M43 + A.M43 * B.M44;
            result.M44 = A.M14 * B.M41 + A.M24 * B.M42 + A.M34 * B.M43 + A.M44 * B.M44;

            return result;
        }
#else
        public static Matrix4 operator *(Matrix4 A, Matrix4 B)
        {
            Matrix4 result;

            // First row
            result.M11 = A.M11 * B.M11 + A.M12 * B.M21 + A.M13 * B.M31 + A.M14 * B.M41;
            result.M12 = A.M11 * B.M12 + A.M12 * B.M22 + A.M13 * B.M32 + A.M14 * B.M42;
            result.M13 = A.M11 * B.M13 + A.M12 * B.M23 + A.M13 * B.M33 + A.M14 * B.M43;
            result.M14 = A.M11 * B.M14 + A.M12 * B.M24 + A.M13 * B.M34 + A.M14 * B.M44;

            // Second row
            result.M21 = A.M21 * B.M11 + A.M22 * B.M21 + A.M23 * B.M31 + A.M24 * B.M41;
            result.M22 = A.M21 * B.M12 + A.M22 * B.M22 + A.M23 * B.M32 + A.M24 * B.M42;
            result.M23 = A.M21 * B.M13 + A.M22 * B.M23 + A.M23 * B.M33 + A.M24 * B.M43;
            result.M24 = A.M21 * B.M14 + A.M22 * B.M24 + A.M23 * B.M34 + A.M24 * B.M44;

            // Third row
            result.M31 = A.M31 * B.M11 + A.M32 * B.M21 + A.M33 * B.M31 + A.M34 * B.M41;
            result.M32 = A.M31 * B.M12 + A.M32 * B.M22 + A.M33 * B.M32 + A.M34 * B.M42;
            result.M33 = A.M31 * B.M13 + A.M32 * B.M23 + A.M33 * B.M33 + A.M34 * B.M43;
            result.M34 = A.M31 * B.M14 + A.M32 * B.M24 + A.M33 * B.M34 + A.M34 * B.M44;

            // Fourth row
            result.M41 = A.M41 * B.M11 + A.M42 * B.M21 + A.M43 * B.M31 + A.M44 * B.M41;
            result.M42 = A.M41 * B.M12 + A.M42 * B.M22 + A.M43 * B.M32 + A.M44 * B.M42;
            result.M43 = A.M41 * B.M13 + A.M42 * B.M23 + A.M43 * B.M33 + A.M44 * B.M43;
            result.M44 = A.M41 * B.M14 + A.M42 * B.M24 + A.M43 * B.M34 + A.M44 * B.M44;

            return result;
        }
#endif

        public static Matrix4 operator *(Matrix4 value1, float value2)
        {
            Matrix4 result;

            result.M11 = value1.M11 * value2;
            result.M12 = value1.M12 * value2;
            result.M13 = value1.M13 * value2;
            result.M14 = value1.M14 * value2;
            result.M21 = value1.M21 * value2;
            result.M22 = value1.M22 * value2;
            result.M23 = value1.M23 * value2;
            result.M24 = value1.M24 * value2;
            result.M31 = value1.M31 * value2;
            result.M32 = value1.M32 * value2;
            result.M33 = value1.M33 * value2;
            result.M34 = value1.M34 * value2;
            result.M41 = value1.M41 * value2;
            result.M42 = value1.M42 * value2;
            result.M43 = value1.M43 * value2;
            result.M44 = value1.M44 * value2;

            return result;
        }

        public static Matrix4 operator *(float value2, Matrix4 value1)
        {
            Matrix4 result;

            result.M11 = value1.M11 * value2;
            result.M12 = value1.M12 * value2;
            result.M13 = value1.M13 * value2;
            result.M14 = value1.M14 * value2;
            result.M21 = value1.M21 * value2;
            result.M22 = value1.M22 * value2;
            result.M23 = value1.M23 * value2;
            result.M24 = value1.M24 * value2;
            result.M31 = value1.M31 * value2;
            result.M32 = value1.M32 * value2;
            result.M33 = value1.M33 * value2;
            result.M34 = value1.M34 * value2;
            result.M41 = value1.M41 * value2;
            result.M42 = value1.M42 * value2;
            result.M43 = value1.M43 * value2;
            result.M44 = value1.M44 * value2;

            return result;
        }

        public static Matrix4 operator *(Matrix4 value, Quaternion rotation)
        {
            // Compute rotation matrix.
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

            float q11 = 1.0f - yy2 - zz2;
            float q21 = xy2 - wz2;
            float q31 = xz2 + wy2;

            float q12 = xy2 + wz2;
            float q22 = 1.0f - xx2 - zz2;
            float q32 = yz2 - wx2;

            float q13 = xz2 - wy2;
            float q23 = yz2 + wx2;
            float q33 = 1.0f - xx2 - yy2;

            Matrix4 result;

            // First row
            result.M11 = value.M11 * q11 + value.M12 * q21 + value.M13 * q31;
            result.M12 = value.M11 * q12 + value.M12 * q22 + value.M13 * q32;
            result.M13 = value.M11 * q13 + value.M12 * q23 + value.M13 * q33;
            result.M14 = value.M14;

            // Second row
            result.M21 = value.M21 * q11 + value.M22 * q21 + value.M23 * q31;
            result.M22 = value.M21 * q12 + value.M22 * q22 + value.M23 * q32;
            result.M23 = value.M21 * q13 + value.M22 * q23 + value.M23 * q33;
            result.M24 = value.M24;

            // Third row
            result.M31 = value.M31 * q11 + value.M32 * q21 + value.M33 * q31;
            result.M32 = value.M31 * q12 + value.M32 * q22 + value.M33 * q32;
            result.M33 = value.M31 * q13 + value.M32 * q23 + value.M33 * q33;
            result.M34 = value.M34;

            // Fourth row
            result.M41 = value.M41 * q11 + value.M42 * q21 + value.M43 * q31;
            result.M42 = value.M41 * q12 + value.M42 * q22 + value.M43 * q32;
            result.M43 = value.M41 * q13 + value.M42 * q23 + value.M43 * q33;
            result.M44 = value.M44;

            return result;
        }

        public static Matrix4 operator /(Matrix4 value1, float value2)
        {
            Matrix4 result;

            result.M11 = value1.M11 / value2;
            result.M12 = value1.M12 / value2;
            result.M13 = value1.M13 / value2;
            result.M14 = value1.M14 / value2;
            result.M21 = value1.M21 / value2;
            result.M22 = value1.M22 / value2;
            result.M23 = value1.M23 / value2;
            result.M24 = value1.M24 / value2;
            result.M31 = value1.M31 / value2;
            result.M32 = value1.M32 / value2;
            result.M33 = value1.M33 / value2;
            result.M34 = value1.M34 / value2;
            result.M41 = value1.M41 / value2;
            result.M42 = value1.M42 / value2;
            result.M43 = value1.M43 / value2;
            result.M44 = value1.M44 / value2;

            return result;
        }

        public static Matrix4 operator /(float value2, Matrix4 value1)
        {
            Matrix4 result;

            result.M11 = value2 / value1.M11;
            result.M12 = value2 / value1.M12;
            result.M13 = value2 / value1.M13;
            result.M14 = value2 / value1.M14;
            result.M21 = value2 / value1.M21;
            result.M22 = value2 / value1.M22;
            result.M23 = value2 / value1.M23;
            result.M24 = value2 / value1.M24;
            result.M31 = value2 / value1.M31;
            result.M32 = value2 / value1.M32;
            result.M33 = value2 / value1.M33;
            result.M34 = value2 / value1.M34;
            result.M41 = value2 / value1.M41;
            result.M42 = value2 / value1.M42;
            result.M43 = value2 / value1.M43;
            result.M44 = value2 / value1.M44;

            return result;
        }

#if COLUMN_MAJOR
        public static Vector4 operator *(Matrix4 M, Vector4 v)
        {
            return new Vector4(v.X * M.M11 + v.Y * M.M21 + v.Z * M.M31 + v.W * M.M41,
                v.X * M.M12 + v.Y * M.M22 + v.Z * M.M32 + v.W * M.M42,
                v.X * M.M13 + v.Y * M.M23 + v.Z * M.M33 + v.W * M.M43,
                v.X * M.M14 + v.Y * M.M24 + v.Z * M.M34 + v.W * M.M44);
            
        }

        public static Vector4 operator *(Vector4 v, Matrix4 M)
        {
            return new Vector4(M.M11 * v.X + M.M12 * v.Y + M.M13 * v.Z + M.M14 * v.W,
                M.M21 * v.X + M.M22 * v.Y + M.M23 * v.Z + M.M24 * v.W,
                M.M31 * v.X + M.M32 * v.Y + M.M33 * v.Z + M.M34 * v.W,
                M.M41 * v.X + M.M42 * v.Y + M.M43 * v.Z + M.M44 * v.W);
        }
#else
        public static Vector4 operator *(Matrix4 M, Vector4 v)
        {
            return new Vector4(M.M11 * v.X + M.M12 * v.Y + M.M13 * v.Z + M.M14 * v.W,
                M.M21 * v.X + M.M22 * v.Y + M.M23 * v.Z + M.M24 * v.W,
                M.M31 * v.X + M.M32 * v.Y + M.M33 * v.Z + M.M34 * v.W,
                M.M41 * v.X + M.M42 * v.Y + M.M43 * v.Z + M.M44 * v.W);
        }

        public static Vector4 operator *(Vector4 v, Matrix4 M)
        {
            return new Vector4(v.X * M.M11 + v.Y * M.M21 + v.Z * M.M31 + v.W * M.M41,
                v.X * M.M12 + v.Y * M.M22 + v.Z * M.M32 + v.W * M.M42,
                v.X * M.M13 + v.Y * M.M23 + v.Z * M.M33 + v.W * M.M43,
                v.X * M.M14 + v.Y * M.M24 + v.Z * M.M34 + v.W * M.M44);
        }
#endif

        public static bool operator ==(Matrix4 value1, Matrix4 value2)
        {
            // Check diagonal element first for early out.
            return (value1.M11 == value2.M11 && value1.M22 == value2.M22 && value1.M33 == value2.M33 && value1.M44 == value2.M44 &&
                                                value1.M12 == value2.M12 && value1.M13 == value2.M13 && value1.M14 == value2.M14 &&
                    value1.M21 == value2.M21 && value1.M23 == value2.M23 && value1.M24 == value2.M24 &&
                    value1.M31 == value2.M31 && value1.M32 == value2.M32 && value1.M34 == value2.M34 &&
                    value1.M41 == value2.M41 && value1.M42 == value2.M42 && value1.M43 == value2.M43);
        }

        public static bool operator !=(Matrix4 value1, Matrix4 value2)
        {
            return (value1.M11 != value2.M11 || value1.M12 != value2.M12 || value1.M13 != value2.M13 || value1.M14 != value2.M14 ||
                    value1.M21 != value2.M21 || value1.M22 != value2.M22 || value1.M23 != value2.M23 || value1.M24 != value2.M24 ||
                    value1.M31 != value2.M31 || value1.M32 != value2.M32 || value1.M33 != value2.M33 || value1.M34 != value2.M34 ||
                    value1.M41 != value2.M41 || value1.M42 != value2.M42 || value1.M43 != value2.M43 || value1.M44 != value2.M44);
        }

        public override bool Equals(object obj)
        {
            if (obj is Matrix4)
            {
                return Equals((Matrix4)obj);
            }

            return false;
        }

        public bool Equals(Matrix4 other)
        {
            return (M11 == other.M11 && M22 == other.M22 && M33 == other.M33 && M44 == other.M44 && // Check diagonal element first for early out.
                                        M12 == other.M12 && M13 == other.M13 && M14 == other.M14 &&
                    M21 == other.M21 && M23 == other.M23 && M24 == other.M24 &&
                    M31 == other.M31 && M32 == other.M32 && M34 == other.M34 &&
                    M41 == other.M41 && M42 == other.M42 && M43 == other.M43);
        }

        public override int GetHashCode()
        {
            return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() + M14.GetHashCode() +
                   M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() + M24.GetHashCode() +
                   M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode() + M34.GetHashCode() +
                   M41.GetHashCode() + M42.GetHashCode() + M43.GetHashCode() + M44.GetHashCode();
        }

        public override string ToString()
        {
            return "{ " + Row1.ToString() + ", " + Row2.ToString() + ", " + Row3.ToString() + ", " + Row4.ToString() + "}";
        }
#endregion
    }
}
