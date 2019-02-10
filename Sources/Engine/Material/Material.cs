using System;
using System.Runtime.InteropServices;

namespace Lava.Engine
{
    public class Material
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr NewMaterial_Native(string pipelineType);

        [DllImport("LavaCore.dll")]
        private static extern void SetUniformUInt_Native(IntPtr mat, uint binding, uint value);

        [DllImport("LavaCore.dll")]
        private static extern void SetUniformFloat_Native(IntPtr mat, uint binding, float value);

        public IntPtr NativePtr { get; private set; }

        public Material(string type)
        {
            NativePtr = NewMaterial_Native(type);
        }

        public void SetUniform(uint binding, uint value)
        {
            SetUniformUInt_Native(NativePtr, binding, value);
        }
    }


    //public class AlbedoMaterial : Material
    //{
    //    [DllImport("LavaCore.dll")]
    //    private static extern IntPtr CreateAlbedoMaterial_Native();

    //    [DllImport("LavaCore.dll")]
    //    private static extern IntPtr AlbedoSetTexture_Native(IntPtr rawAlbedo, uint texIndex);

    //    public uint AlbedoTex
    //    {
    //        get { return albedoTex; }
    //        set { AlbedoSetTexture_Native(NativePtr, value); albedoTex = value; }
    //    }

    //    private uint albedoTex;

    //    public AlbedoMaterial()
    //    {
    //        NativePtr = CreateAlbedoMaterial_Native();
    //    }

    //    public AlbedoMaterial(uint albedoTex) : this()
    //    {
    //        AlbedoTex = albedoTex;
    //    }
    //}


    //public class PhongMaterial : Material
    //{
    //    [DllImport("LavaCore.dll")]
    //    private static extern IntPtr CreatePhongMaterial_Native();

    //    [DllImport("LavaCore.dll")]
    //    private static extern IntPtr PhongSetAlbedoTex_Native(IntPtr rawPhong, uint texIndex);

    //    public uint AlbedoTex
    //    {
    //        get { return albedoTex; }
    //        set { PhongSetAlbedoTex_Native(NativePtr, value); albedoTex = value; }
    //    }

    //    private uint albedoTex;

    //    public PhongMaterial()
    //    {
    //        NativePtr = CreatePhongMaterial_Native();
    //    }

    //    public PhongMaterial(uint albedoTex) : this()
    //    {
    //        AlbedoTex = albedoTex;
    //    }
    //}
}
