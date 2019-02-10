using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Lava.Mathematics
{
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Plane : IEquatable<Plane>
    {
        // Some implementations were taken from
        // https://referencesource.microsoft.com/#System.Numerics/System/Numerics/Plane.cs

        #region Enumerations
        public enum PlaneSide
        {
            Negative, Positive, Both, None
        };
        #endregion

        #region Public Fields
        /// <summary>
        /// The normal vector of the Plane.
        /// </summary>
        public Vector3 Normal;
        /// <summary>
        /// The distance of the Plane along its normal from the origin.
        /// </summary>
        public float D;
        #endregion

        #region Operators
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator ==(Plane value1, Plane value2)
        {
            return (value1.Normal.X == value2.Normal.X &&
                    value1.Normal.Y == value2.Normal.Y &&
                    value1.Normal.Z == value2.Normal.Z &&
                    value1.D == value2.D);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool operator !=(Plane value1, Plane value2)
        {
            return (value1.Normal.X != value2.Normal.X ||
                    value1.Normal.Y != value2.Normal.Y ||
                    value1.Normal.Z != value2.Normal.Z ||
                    value1.D != value2.D);
        }
        #endregion

        #region Constructors
        /// <summary>
        /// Constructs a Plane from the X, Y, and Z components of its normal, and its distance from the origin on that normal.
        /// </summary>
        /// <param name="x">The X-component of the normal.</param>
        /// <param name="y">The Y-component of the normal.</param>
        /// <param name="z">The Z-component of the normal.</param>
        /// <param name="d">The distance of the Plane along its normal from the origin.</param>
        public Plane(float x, float y, float z, float d)
        {
            Normal = new Vector3(x, y, z);
            this.D = d;
        }

        /// <summary>
        /// Constructs a Plane from the given normal and distance along the normal from the origin.
        /// </summary>
        /// <param name="normal">The Plane's normal vector.</param>
        /// <param name="d">The Plane's distance from the origin along its normal vector.</param>
        public Plane(Vector3 normal, float d)
        {
            this.Normal = normal;
            this.D = d;
        }

        /// <summary>
        /// Constructs a Plane from the given Vector4.
        /// </summary>
        /// <param name="value">A vector whose first 3 elements describe the normal vector, 
        /// and whose W component defines the distance along that normal from the origin.</param>
        public Plane(Vector4 value)
        {
            Normal = new Vector3(value.X, value.Y, value.Z);
            D = value.W;
        }
        #endregion

        #region Methods
        /// <summary>
        /// Check which side a AxisAlignedBoundingBox falls to on the plane.
        /// </summary>
        /// <param name="box">AxisAlignedBoundingBox.</param>
        /// <returns>PlaneSide.Negative, PlaneSide.Positive or PlaneSide.Both.</returns>
        public PlaneSide Intersects(AxisAlignedBoundingBox box)
        {
            float distance = DistanceFromPoint(box.Center);
            float mdist = System.Math.Abs(Vector3.Dot(Normal, box.Size * 0.5f));
            if (distance < -mdist) return PlaneSide.Negative;
            else if (distance > mdist) return PlaneSide.Positive;
            else return PlaneSide.Both;
        }

        /// <summary>
        /// Calculates the dot product of a Plane and Vector4.
        /// </summary>
        /// <param name="plane">The Plane.</param>
        /// <param name="value">The Vector4.</param>
        /// <returns>The dot product.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float Dot(Plane plane, Vector4 value)
        {
            return plane.Normal.X * value.X +
                   plane.Normal.Y * value.Y +
                   plane.Normal.Z * value.Z +
                   plane.D * value.W;
        }

        /// <summary>
        /// Get the minimum distance from an arbitrary point and the plane.
        /// </summary>
        /// <param name="Point">The Vector3 point to check.</param>
        /// <returns>The minimum distance between the plane and point.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public float DistanceFromPoint(Vector3 Point)
        {
            return Normal.X * Point.X +
                    Normal.Y * Point.Y +
                    Normal.Z * Point.Z +
                    D;
        }

        /// <summary>
        /// Returns the dot product of a specified Vector3 and the Normal vector of this Plane.
        /// </summary>
        /// <param name="plane">The plane.</param>
        /// <param name="value">The Vector3.</param>
        /// <returns>The resulting dot product.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float DotNormal(Plane plane, Vector3 value)
        {
            return plane.Normal.X * value.X +
                    plane.Normal.Y * value.Y +
                    plane.Normal.Z * value.Z;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool Equals(Plane other)
        {
            return(Normal.X == other.Normal.X &&
                   Normal.Y == other.Normal.Y &&
                   Normal.Z == other.Normal.Z &&
                   D == other.D);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public override bool Equals(object obj)
        {
            if (obj is Plane)
            {
                return Equals((Plane)obj);
            }

            return false;
        }

        public override string ToString()
        {
            return string.Format("Normal: {0} D: {1}", Normal, D);
        }

        public override int GetHashCode()
        {
            return Normal.GetHashCode() + D.GetHashCode();
        }

        /// <summary>
        /// Creates a new Plane whose normal vector is the source Plane's normal vector normalized.
        /// </summary>
        /// <returns>The normalized Plane.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public Plane Normalize()
        {
            const float FLT_EPSILON = 1.192092896e-07f; // smallest such that 1.0+FLT_EPSILON != 1.0
            float f = this.Normal.X * this.Normal.X + this.Normal.Y * this.Normal.Y + this.Normal.Z * this.Normal.Z;

            if (Math.Abs(f - 1.0f) < FLT_EPSILON)
            {
                return this; // It already normalized, so we don't need to further process.
            }

            float fInv = 1.0f / (float)Math.Sqrt(f);

            return new Plane(
                this.Normal.X * fInv,
                this.Normal.Y * fInv,
                this.Normal.Z * fInv,
                this.D * fInv);
        }
        #endregion
    }
}
