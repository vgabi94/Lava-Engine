using Lava.Mathematics;

namespace Lava.Engine
{
    public sealed class PerspectiveCamera : Camera
    {
        protected override void ComputeProjection()
        {
            Projection = Matrix4.CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);
        }

        public float FoV
        {
            get { return fov; }
            set { fov = value; ComputeProjection(); }
        }
        private float fov;

        public float Aspect
        {
            get { return aspect; }
            set { aspect = value; ComputeProjection(); }
        }
        private float aspect;

        public void SetPerspective(float fov, float aspect, float zNear, float zFar)
        {
            this.fov = fov;
            this.aspect = aspect;
            nearPlane = zNear;
            farPlane = zFar;
            ComputeProjection();
        }

        protected override void OnFramebufferSize(int width, int height)
        {
            Aspect = (width / (float)height);
        }

        public PerspectiveCamera(Vector3 pos, Vector3 target)
        {
            fov = Mathf.Radians(45);
            aspect = 1.7395f;
            nearPlane = 0.1f;
            farPlane = 800f;

            ComputeProjection();
            SetupCamera(pos, target, Vector3.UnitY);
            ComputeView();

            EventManager.FramebufferResizeEvent += OnFramebufferSize;
        }

        public PerspectiveCamera(Vector3 pos, Vector3 forward, Vector3 up)
        {
            fov = Mathf.Radians(45);
            aspect = 1.7395f;
            nearPlane = 0.1f;
            farPlane = 800f;

            ComputeProjection();
            SetupCamera(pos, pos - forward, up);
            ComputeView();

            EventManager.FramebufferResizeEvent += OnFramebufferSize;
        }

        public PerspectiveCamera() : this(Vector3.Zero, Vector3.UnitZ) { }
    }
}
