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
