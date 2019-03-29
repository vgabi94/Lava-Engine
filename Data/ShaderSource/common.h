#ifndef COMMON_H
#define COMMON_H

#define UNIFORM(set_num, binding_num, type, name) layout(set = set_num, binding = binding_num) uniform type name
#define BUFFER(set_num, binding_num, type, name) layout(set = set_num, binding = binding_num) buffer type name

#define BUFFER0(binding_num, type, name) layout(set = 0, binding = binding_num) buffer type name
#define UNIFORM0(binding_num, type, name) layout(set = 0, binding = binding_num) uniform type name

#define IN(loc, type, name) layout(location = loc) in type name
#define OUT(loc, type, name) layout(location = loc) out type name

#define DECL_POSITION out gl_PerVertex { vec4 gl_Position; }

#define DECL_OBJ_PS layout(push_constant) uniform ObjPS \
{\
    mat4 MVP;\
    mat4 Model;\
    vec3 EyePos; } g_Obj

#define DECL_SKY_PS layout(push_constant) uniform SkyPS \
{\
    mat4 ViewProj;\
    float Exposure;\
    float Gamma; } g_Sky

#define DECL_IRRAD_PS layout(push_constant) uniform IrradPS { float deltaPhi; float deltaTheta; } g_Irrad

// prefiltered environment map
#define DECL_PRENV_PS layout(push_constant) uniform PrenvPS { float roughness; uint numSamples; } g_Prenv

#define PI 3.1415926535897932384626433832795

#endif