#ifndef TYPES_H
#define TYPES_H

#ifdef LAVA_TYPES
#include <Common\MathTypes.h>
    #define gpuFloat3 Engine::Vector3
    #define gpuFloat2 Engine::Vector2
    #define gpuFloat4 Engine::Vector4
    #define gpuInt3 Engine::Vector3Int
    #define gpuInt2 Engine::Vector2Int
    #define gpuInt4 Engine::Vector4Int
    #define gpuMat3 Engine::Matrix3
    #define gpuMat4 Engine::Matrix4
#else
    #define gpuFloat3 vec3
    #define gpuFloat2 vec2
    #define gpuFloat4 vec4
    #define gpuInt3 ivec3
    #define gpuInt2 ivec2
    #define gpuInt4 ivec4
    #define gpuMat3 mat3
    #define gpuMat4 mat4
#endif

#endif