using Lava.Mathematics;
using System.Runtime.InteropServices;

namespace Lava.Engine
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct IBLProbeInfo
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        public Matrix4[] matrices;
        public Vector3 position;
    }

    public sealed class IBLProbe : Component
    {
        public Vector3 Position { get; set; }

        internal IBLProbeInfo info;

        protected internal override void OnInit()
        {
            const float near = 0.1f;
            const float far = 512f;
            const float aspect = 1f;
            float fov = Mathf.Radians(90f);

            PerspectiveCamera posZ = new PerspectiveCamera(Vector3.Zero, Vector3.UnitZ, Vector3.UnitY, fov, aspect, near, far);

            PerspectiveCamera negZ = new PerspectiveCamera(Vector3.Zero, Vector3.UnitZ, Vector3.UnitY, fov, aspect, near, far);
            negZ.Rotate(180f, Vector3.UnitX);

            PerspectiveCamera posX = new PerspectiveCamera(Vector3.Zero, Vector3.UnitZ, Vector3.UnitY, fov, aspect, near, far);
            posX.Rotate(-90f, Vector3.UnitY);

            PerspectiveCamera negX = new PerspectiveCamera(Vector3.Zero, Vector3.UnitZ, Vector3.UnitY, fov, aspect, near, far);
            negX.Rotate(90f, Vector3.UnitY);

            PerspectiveCamera posY = new PerspectiveCamera(Vector3.Zero, Vector3.UnitZ, Vector3.UnitY, fov, aspect, near, far);
            posY.Rotate(90f, Vector3.UnitX);

            PerspectiveCamera negY = new PerspectiveCamera(Vector3.Zero, Vector3.UnitZ, Vector3.UnitY, fov, aspect, near, far);
            negY.Rotate(-90f, Vector3.UnitX);

            // Cube matrices
            PerspectiveCamera[] cameras =
            {
                // POSITIVE_X
			    // glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f))
                //new PerspectiveCamera(Position, -Vector3.UnitX, Vector3.UnitY, fov, aspect, near, far),
                posX,
			    
                // NEGATIVE_X
			    // glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f))
                //new PerspectiveCamera(Position, Vector3.UnitX, Vector3.UnitY, fov, aspect, near, far),
                negX,
                
                // POSITIVE_Y
			    // glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
                //new PerspectiveCamera(Position, Vector3.UnitY, Vector3.UnitX, fov, aspect, near, far),
                posY,
                
                // NEGATIVE_Y
			    // glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
                //new PerspectiveCamera(Position, -Vector3.UnitY, Vector3.UnitX, fov, aspect, near, far),
                negY,
                
                // POSITIVE_Z
			    // glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f))
                //new PerspectiveCamera(Position, Vector3.UnitZ, -Vector3.UnitY, fov, aspect, near, far),
                posZ,
                
                // NEGATIVE_Z
			    // glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f))
                //new PerspectiveCamera(Position, -Vector3.UnitZ, -Vector3.UnitY, fov, aspect, near, far),
                negZ
            };

            info.position = Position;
            info.matrices = new Matrix4[cameras.Length];
            for (int i = 0; i < cameras.Length; i++)
            {
                //info.matrices[i] = cameras[i].ViewProjOpenGL;
                info.matrices[i] = cameras[i].SkyViewProj;
                //info.matrices[i] = cameras[i].ViewProjection;
            }
        }
    }
}