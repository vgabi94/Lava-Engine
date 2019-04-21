using Lava.Mathematics;

namespace Lava.Engine
{
    public sealed class OrthoCamera : Camera
    {
        protected override void ComputeProjection()
        {
            Projection = Matrix4.CreateOrthographic(width, height, nearPlane, farPlane);
        }

        public float Width
        {
            get { return width; }
            set { width = value; ComputeProjection(); }
        }
        private float width;

        public float Height
        {
            get { return height; }
            set { height = value; ComputeProjection(); }
        }
        private float height;

        public void SetPerspective(float width, float height, float zNear, float zFar)
        {
            this.width = width;
            this.height = height;
            nearPlane = zNear;
            farPlane = zFar;
            ComputeProjection();
        }

        protected override void OnFramebufferSize(int width, int height)
        {
            this.width = width;
            this.height = height;
        }

        public OrthoCamera(Vector3 pos, Vector3 target)
        {
            width = 1336f;
            height = 768f;
            nearPlane = 0f;
            farPlane = 1f;

            ComputeProjection();
            SetupCamera(pos, target, Vector3.UnitY);
            ComputeView();

            EventManager.FramebufferResizeEvent += OnFramebufferSize;
        }

        public OrthoCamera(Vector3 pos, Vector3 forward, Vector3 up)
        {
            width = 1336f;
            height = 720f;
            nearPlane = 0f;
            farPlane = 1f;

            ComputeProjection();
            SetupCamera(pos, pos - forward, up);
            ComputeView();

            EventManager.FramebufferResizeEvent += OnFramebufferSize;
        }

        /// <summary>
        /// This constructor does not add OnFramebufferSize callback
        /// </summary>
        internal OrthoCamera(Vector3 pos, Vector3 forward, Vector3 up, float width, float height, float near, float far)
        {
            this.width = width;
            this.height = height;
            nearPlane = near;
            farPlane = far;

            ComputeProjection();
            SetupCamera(pos, pos - forward, up);
            ComputeView();

            //EventManager.FramebufferResizeEvent += OnFramebufferSize;
        }

        public OrthoCamera() : this(Vector3.Zero, Vector3.UnitZ) { }
    }
}
