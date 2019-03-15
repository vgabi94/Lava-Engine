#ifdef LAVA_TYPES
    #define gpuVec3 Vector3
    #define gpuVec2 Vector2
    #define gpuVec4 Vector4
    #define gpuMat3 Matrix3
    #define gpuMat4 Matrix4
#else
    #define gpuVec3 vec3
    #define gpuVec2 vec2
    #define gpuVec4 vec4
    #define gpuMat3 mat3
    #define gpuMat4 mat4
#endif