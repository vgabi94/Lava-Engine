using System.Runtime.InteropServices;

namespace Lava.Engine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct SkySettings
    {
        public Lava.Mathematics.Vector3 color;
        public uint hdrTex;
        public uint hdrEnv; // Irradiance map
        public bool useTex;
        public float exposure;
        public float gamma;
        public float ambient;

        public void SetColor(Color color)
        {
            this.color = color.ToVector().XYZ;
        }

        public void SetHDRTex(uint hdrTex)
        {
            this.hdrTex = hdrTex;
            useTex = true;
        }
    }
}