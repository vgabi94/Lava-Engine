using Lava.Mathematics;

namespace Lava.Engine
{
    public class Transform : Component
    {
        public Matrix4 Model
        {
            get
            {
                if (dirty)
                {
                    model = ComputeModel();
                    dirty = false;
                }
                return model;
            }
        }
        private Matrix4 model;

        private Matrix4 ComputeModel()
        {
            return Matrix4.CreateTranslation(Position)
                * Rotation.Matrix4
                * Matrix4.CreateScale(Scale);
        }

        // Origin
        public Vector3 Position
        {
            get => _position;
            set { _position = value; dirty = true; }
        }

        public Vector3 Scale
        {
            get => _scale;
            set { _scale = value; dirty = true; }
        }

        public Quaternion Rotation
        {
            get => _rotation;
            set { _rotation = value; dirty = true; }
        }

        protected internal override void OnInit()
        {
            Position = Vector3.Zero;
            Scale = Vector3.One;
            Rotation = Quaternion.Identity;
        }

        private bool dirty;

        private Vector3 _position;
        private Vector3 _scale;
        private Quaternion _rotation;
    }
}
