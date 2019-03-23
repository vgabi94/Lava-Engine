namespace Lava.Engine
{
    // TODO add attenuation
    public sealed class PointLight : Light
    {
        public Lava.Mathematics.Vector3 Position { get; set; }

        internal override LightInfo ToLightInfo()
        {
            LightInfo li = new LightInfo();
            li.intensity = Intensity;
            li.position = Position;
            li.direction = new Mathematics.Vector3(float.NaN);
            li.color = Color.ToVector();
            li.isShadowCaster = IsShadowCaster;
            li.type = LightType.Point;
            return li;
        }

        public PointLight() : this(new Lava.Mathematics.Vector3(0f)) { }

        public PointLight(Lava.Mathematics.Vector3 position) : this(position, Color.WHITE) { }

        public PointLight(Lava.Mathematics.Vector3 position, Color color) : base(color)
        {
            Position = position;
        }
    }
}