#define UNIFORM(set_num, binding_num, type, name) layout(set = set_num, binding = binding_num) uniform type name
#define BUFFER(set_num, binding_num, type, name) layout(set = set_num, binding = binding_num) buffer type name

#define BUFFER0(binding_num, type, name) layout(set = 0, binding = binding_num) buffer type name
#define UNIFORM0(binding_num, type, name) layout(set = 0, binding = binding_num) uniform type name

#define IN(loc, type, name) layout(location = loc) in type name
#define OUT(loc, type, name) layout(location = loc) out type name

#define DECL_POSITION out gl_PerVertex { vec4 gl_Position; }

#define DECL_GLOBALS layout(push_constant) uniform Globals \
{\
    mat4 MVP;\
    mat4 Model;\
    vec3 EyePos; }

#define DECL_SKY_GLOBALS layout(push_constant) uniform SkyPS \
{\
    mat4 ViewProj;\
    float Exposure;\
    float Gamma; }

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	const float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

vec3 ApplyToneMapping(vec3 color, float exposure)
{
    color = Uncharted2Tonemap(color * exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
    return color;
}

vec3 ApplyGammaCorrection(vec3 color, float gamma)
{
    return pow(color, vec3(1.0f / gamma));
}