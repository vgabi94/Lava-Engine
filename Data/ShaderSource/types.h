#ifdef LAVA_TYPES
#include <Common\MathTypes.h>
    #define gpuVec3 Engine::Vector3
    #define gpuVec2 Engine::Vector2
    #define gpuVec4 Engine::Vector4
    #define gpuMat3 Engine::Matrix3
    #define gpuMat4 Engine::Matrix4
#else
    #define gpuVec3 vec3
    #define gpuVec2 vec2
    #define gpuVec4 vec4
    #define gpuMat3 mat3
    #define gpuMat4 mat4
#endif