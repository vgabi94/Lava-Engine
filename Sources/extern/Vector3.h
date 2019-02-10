/*
 * Copyright (c) 2018 Gabriel Vaduva
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#pragma once

#include <initializer_list>
#include <cassert>
#include <algorithm>

// otr from Octree
namespace otr
{
    struct Vector3
    {
        union
        {
            struct
            {
                float x, y, z;
            };

            float data[3];
        };

        Vector3() : Vector3(0.f, 0.f, 0.f) { }

        Vector3(float x, float y, float z) : x(x), y(y), z(z) { }
        
        Vector3(float value) : x(value), y(value), z(value) { }

        Vector3(std::initializer_list<float> l)
        {
            assert(l.size() == 3);
            std::copy(l.begin(), l.end(), data);
        }

        float& operator[](unsigned int i)
        {
            return data[i];
        }

        const float& operator[](unsigned int i) const
        {
            return data[i];
        }

        bool operator==(const Vector3& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3& other) const
        {
            return !(*this == other);
        }

        Vector3 operator+(const Vector3& other) const
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        Vector3 operator-(const Vector3& other) const
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        Vector3 operator*(float k) const
        {
            return Vector3(x * k, y * k, z * k);
        }
    };
}