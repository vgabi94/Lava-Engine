namespace Lava.Engine
{
    public sealed class DirectionalLight : Light
    {
        public DirectionalLight() : this(new Lava.Mathematics.Vector3(1f, 0f, 0f)) { }

        public DirectionalLight(Lava.Mathematics.Vector3 direction) : this(direction, Color.WHITE) { }

        public DirectionalLight(Lava.Mathematics.Vector3 direction, Color color) : base(color)
        {
            Direction = direction;
        }

        public Lava.Mathematics.Vector3 Direction { get; set; }

        internal override LightInfo ToLightInfo()
        {
            LightInfo li = new LightInfo();
            li.intensity = Intensity;
            li.position = new Mathematics.Vector3(float.NaN);
            li.direction = Direction;
            li.color = Color.ToVector();
            li.isShadowCaster = IsShadowCaster;
            li.type = LightType.Directional;
            return li;
        }
    }
}