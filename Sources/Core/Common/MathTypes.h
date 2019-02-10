#pragma once

namespace Engine
{
    struct Vector4
    {
        float x, y, z, w;

        Vector4() : x(0), y(0), z(0), w(0) { };
        Vector4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w) { };
    };

    struct Vector3
    {
        float x, y, z;

        Vector3() : x(0), y(0), z(0) { };
        Vector3(float x, float y, float z)
            : x(x), y(y), z(z) { };
    };

    struct Vector2
    {
        float x, y;

        Vector2() : x(0), y(0) { };
        Vector2(float x, float y)
            : x(x), y(y) { };
    };

    typedef Vector4 Quaternion;

    struct Matrix4
    {
        Vector4 row1, row2, row3, row4;
        Matrix4() : row1(), row2(), row3(), row4() { };
    };

    struct PushConstants
    {
        Matrix4 MVP;
        Matrix4 model;
        Vector3 eyePos;
    };
}