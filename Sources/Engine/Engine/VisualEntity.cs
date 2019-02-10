using System;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Engine
{
    public class VisualEntity : Entity
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateEntity_Native();

        [DllImport("LavaCore.dll")]
        private static extern void AddMaterial_Native(IntPtr ent, IntPtr mat);

        [DllImport("LavaCore.dll")]
        private static extern void AddMesh_Native(IntPtr ent, IntPtr mesh);

        [DllImport("LavaCore.dll")]
        private static extern void SetMVP_Native(IntPtr ent, Matrix4 mvp);

        [DllImport("LavaCore.dll")]
        private static extern void SetModel_Native(IntPtr ent, Matrix4 model);

        [DllImport("LavaCore.dll")]
        private static extern void SetPosition_Native(IntPtr ent, Vector3 pos);

        public Mesh Mesh
        {
            get { return mesh; }
            set { AddMesh_Native(NativePtr, value.NativePtr); mesh = value; }
        }
        private Mesh mesh;

        public Material Material
        {
            get { return material; }
            set { AddMaterial_Native(NativePtr, value.NativePtr); material = value; }
        }
        private Material material;

        public Transform Transform { get; set; }

        public IntPtr NativePtr { get; private set; }

        public bool IsStatic { get; set; }

        public VisualEntity() : base()
        {
            NativePtr = CreateEntity_Native();
            Transform = AddComponent<Transform>();
            IsStatic = false;
        }

        public VisualEntity(Mesh mesh) : this()
        {
            Mesh = mesh;
        }

        public VisualEntity(Mesh mesh, Material material) : this(mesh)
        {
            Material = material;
        }

        internal void UpdatePosition()
        {
            SetPosition_Native(NativePtr, Transform.Position);
        }

        internal override void Update()
        {
            base.Update();

            if (!IsStatic)
            {
                Matrix4 mvp = Camera.Main.ViewProjection * Transform.Model;
                SetMVP_Native(NativePtr, mvp);
                SetModel_Native(NativePtr, Transform.Model);
                UpdatePosition();
            }
        }

        // TODO:
        //private List<Entity> children;
    }
}
