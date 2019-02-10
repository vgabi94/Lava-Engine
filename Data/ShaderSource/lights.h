struct LightSource
{
    vec4 position;
    vec4 direction;
    // x = type of light
    // y = attenuation
    vec4 info;
    // TODO
}

layout(constant_id = 0) const int NUM_LIGHTS = 32;

layout(std140, set = 1, binding = 0) uniform LightConsts
{
    LightSource Light[NUM_LIGHTS];
}