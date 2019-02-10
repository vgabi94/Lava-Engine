using System.Runtime.InteropServices;

namespace Lava.Engine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vertex2D
    {
        public Mathematics.Vector2 position;
        public Mathematics.Vector2 texcoord;

        public Vertex2D(Mathematics.Vector2 position, Mathematics.Vector2 texcoord)
        {
            this.position = position;
            this.texcoord = texcoord;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vertex
    {
        public Mathematics.Vector3 position;
        public Mathematics.Vector3 normal;
        public Mathematics.Vector2 texcoord;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct VertexExt
    {
        public Mathematics.Vector3 position;
        public Mathematics.Vector3 normal;
        public Mathematics.Vector3 tangent;
        public Mathematics.Vector2 texcoord;
    }
}
