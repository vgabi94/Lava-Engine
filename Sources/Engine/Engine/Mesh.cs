using System;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Engine
{
    public abstract class Mesh
    {
        public Matrix4 MVP { get; set; }

        public Vector4 WorldPos { get; set; }

        public IntPtr NativePtr { get; protected set; }
    }


    public class StaticMesh : Mesh
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateMesh_Native(Vertex[] vertices, int verticesLength,
            uint[] indices, int indicesLength);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateMesh2D_Native(Vertex2D[] vertices, int verticesLength,
            uint[] indices, int indicesLength);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr CreateMeshExt_Native(VertexExt[] vertices, int verticesLength,
            uint[] indices, int indicesLength);

        [DllImport("LavaCore.dll")]
        private static extern IntPtr LoadFromFile_Native(string path);


        public StaticMesh(Vertex[] vertices, int verticesLength,
            uint[] indices, int indicesLength)
        {
            NativePtr = CreateMesh_Native(vertices, verticesLength, indices, indicesLength);
        }

        public StaticMesh(Vertex2D[] vertices, int verticesLength,
            uint[] indices, int indicesLength)
        {
            NativePtr = CreateMesh2D_Native(vertices, verticesLength, indices, indicesLength);
        }

        public StaticMesh(VertexExt[] vertices, int verticesLength,
            uint[] indices, int indicesLength)
        {
            NativePtr = CreateMeshExt_Native(vertices, verticesLength, indices, indicesLength);
        }

        public StaticMesh(string path)
        {
            NativePtr = LoadFromFile_Native(path);
        }
    }
}
