using Lava.Mathematics;

namespace Lava.Engine
{
    public enum CameraType
    {
        Perspective, Orthographic
    }

    public abstract class Camera : Component
    {
        public float NearPlane
        {
            get { return nearPlane; }
            set { nearPlane = value; ComputeProjection(); }
        }
        protected float nearPlane;

        public float FarPlane
        {
            get { return farPlane; }
            set { farPlane = value; ComputeProjection(); }
        }
        protected float farPlane;
        
        public Vector3 ViewTarget
        {
            get { return viewTarget; }
            set { viewTarget = value; SetupCamera(position, value, Vector3.UnitY); ComputeView(); }
        }
        protected Vector3 viewTarget;

        public Vector3 Forward
        {
            get { return forward; }
            set { forward = value; SetupCamera(position, position - value, Vector3.UnitY); ComputeView(); }
        }
        protected Vector3 forward;

        public Vector3 Up
        {
            get { return up; }
            set { up = value; SetupCamera(position, position - forward, value); ComputeView(); }
        }
        protected Vector3 up;

        public Vector3 Right { get; protected set; }
        public Matrix4 View { get; protected set; }
        public Matrix4 Projection { get; protected set; }

        public Vector3 Position
        {
            get { return position; }
            set { position = value; ComputeView(); }
        }
        protected Vector3 position;

        public CameraType Type { get; set; }

        protected abstract void ComputeProjection();

        protected void SetupCamera(Vector3 pos, Vector3 target, Vector3 upAxis)
        {
            position = pos;
            forward = (position - target).Normalize();
            Right = upAxis.Cross(forward).Normalize();
            up = forward.Cross(Right).Normalize();
        }

        protected void ComputeView()
        {
            View = Matrix4.LookAt(position, position + forward, up);
        }
        
        protected abstract void OnFramebufferSize(int width, int height);

        // Vulkan clip space correction matrix.
        // https://github.com/LunarG/VulkanSamples/commit/0dd36179880238014512c0637b0ba9f41febe803
        private static readonly Matrix4 CLIP = new Matrix4(new float[] {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.0f, 0.0f, 0.5f, 1.0f
        });

        // No UP flipping
        private static readonly Matrix4 CLIP2 = new Matrix4(new float[] {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.0f, 0.0f, 0.5f, 1.0f
        });

        public static Camera Main { get; set; }

        /// <summary>
        /// No Vulkan clip correction applied
        /// </summary>
        public Matrix4 ViewProjGL
        {
            get
            {
                return Projection * View;
            }
        }

        public Matrix4 ViewProjection
        {
            get
            {
                return CLIP * Projection * View;
            }
        }

        public Matrix4 SkyViewProj
        {
            get
            {
                return CLIP * Projection * new Matrix4(View.Upper3x3);
            }
        }

        /// <summary>
        /// No Vulkan clip correction applied
        /// </summary>
        public Matrix4 SkyViewProjGL
        {
            get
            {
                return Projection * new Matrix4(View.Upper3x3);
            }
        }

        public void Rotate(float degrees, Vector3 axis)
        {
            Quaternion quat = Quaternion.CreateFromAxisAngle(axis, Mathf.Radians(degrees));
            Forward = (quat * forward).Normalize();
        }
    }
}
