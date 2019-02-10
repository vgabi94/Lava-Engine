#pragma once

#include <Common\VertexDataTypes.h>
#include <MemoryPool.h>
#include <Common\Constants.h>

namespace Engine
{
    // Represents a graphical entity in the world
    class StaticMesh
    {
        friend struct Entity;

    public:
        static StaticMesh* Create(const VertexList& vertices, const IndexList& indices);
        static StaticMesh* Create(const Vertex2DList& vertices, const IndexList& indices);
        static StaticMesh* Create(const VertexExtList& vertices, const IndexList& indices);

        static StaticMesh* Load(const char* buffer, size_t length);
        static StaticMesh* Load(const char* path);

        void Destroy();

        MEM_POOL_DECLARE(StaticMesh);
    
    private:
        uint32_t mVBO, mIBO;
        uint32_t mCount;
    };
}