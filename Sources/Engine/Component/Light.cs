using System.Runtime.InteropServices;

namespace Lava.Engine
{
    internal enum LightType
    {
        Directional,
        Point
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct LightInfo
    {
        internal float intensity;
        internal bool isShadowCaster;
        internal LightType type;
        internal Lava.Mathematics.Vector4 color;
        internal Lava.Mathematics.Vector3 position;
        internal Lava.Mathematics.Vector3 direction;
    }

    public abstract class Light : Component
    {
        public Color Color { get; set; }
        public float Intensity { get; set; }
        public bool IsShadowCaster { get; set; }

        internal abstract LightInfo ToLightInfo();

        protected Light() : this(Color.WHITE, 1f, false) { }

        protected Light(Color color, float intensity, bool isShadowCaster)
        {
            Color = color;
            Intensity = intensity;
            IsShadowCaster = isShadowCaster;
        }

        protected Light(Color color, float intensity)
            : this(color, intensity, false) { }

        protected Light(Color color)
            : this(color, 1f, false) { }
    }
}
