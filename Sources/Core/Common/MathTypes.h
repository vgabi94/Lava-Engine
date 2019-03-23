#pragma once

namespace Engine
{
	struct Vector2
	{
		float x, y;

		Vector2() : x(0), y(0) { }
		Vector2(float x, float y)
			: x(x), y(y) { }
	};

	struct Vector3
	{
		float x, y, z;

		Vector3() : x(0), y(0), z(0) { }
		Vector3(float x, float y, float z)
			: x(x), y(y), z(z) { }
	};

    struct Vector4
    {
        float x, y, z, w;

        Vector4() : x(0), y(0), z(0), w(0) { }
        Vector4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w) { }
		Vector4(Vector3 v, float w) : Vector4(v.x, v.y, v.z, w) { }
    };

    typedef Vector4 Quaternion;

    struct Matrix4
    {
        Vector4 row1, row2, row3, row4;
        Matrix4() : row1(), row2(), row3(), row4() { }
    };

	struct Vector2Int
	{
		int x, y;

		Vector2Int() : x(0), y(0) { }
		Vector2Int(int x, int y)
			: x(x), y(y) { }
	};

	struct Vector3Int
	{
		int x, y, z;

		Vector3Int() : x(0), y(0), z(0) { }
		Vector3Int(int x, int y, int z)
			: x(x), y(y), z(z) { }
	};

	struct Vector4Int
	{
		int x, y, z, w;

		Vector4Int() : x(0), y(0), z(0), w(0) { }
		Vector4Int(int x, int y, int z, int w)
			: x(x), y(y), z(z), w(w) { }
		Vector4Int(Vector3 v, int w) : Vector4Int(v.x, v.y, v.z, w) { }
	};
}