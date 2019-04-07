#include "StaticMesh.h"
#include <Manager\BufferManager.h>
#include "World.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <fstream>

#define ALLOCATE_STATIC_MESH(vertices, indices) StaticMesh* ent = Allocate(); \
auto ioff = CreateVertexIndexBuffer(vertices, indices); \
ent->mVBO = ioff.first; \
ent->mIBO = ioff.second; \
ent->mCount = (uint32_t)indices.size(); \
LOG_INFO("[LOG] Create static mesh {0:#x}\n", (uint64_t)ent); \
return ent

#define CREATE_STATIC_MESH(Type) Engine::StaticMesh::Create(Engine::Vertex##Type##List(vertices, vertices + verticesLength), \
Engine::IndexList(indices, indices + indicesLength))

namespace Engine
{
    MEM_POOL_DEFINE(StaticMesh);

    StaticMesh * StaticMesh::Create(const VertexList & vertices, const IndexList & indices)
    {
        ALLOCATE_STATIC_MESH(vertices, indices);
    }

    StaticMesh * StaticMesh::Create(const Vertex2DList & vertices, const IndexList & indices)
    {
        ALLOCATE_STATIC_MESH(vertices, indices);
    }

    StaticMesh * StaticMesh::Create(const VertexExtList & vertices, const IndexList & indices)
    {
        ALLOCATE_STATIC_MESH(vertices, indices);
    }

    StaticMesh * StaticMesh::Load(const char * path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs | aiProcess_GenUVCoords);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG_ERROR("Assimp error: {}", importer.GetErrorString());
            throw "err";
        }
        else
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;

            for (unsigned int i = 0; i < scene->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[i];

                for (unsigned int j = 0; j < mesh->mNumVertices; j++)
                {
                    Vertex newVertex;

                    newVertex.position = Vector3(
                        mesh->mVertices[j].x,
                        mesh->mVertices[j].y,
                        mesh->mVertices[j].z
                    );

                    newVertex.normal = Vector3(
                        mesh->mNormals[j].x,
                        mesh->mNormals[j].y,
                        mesh->mNormals[j].z
                    );

                    if (mesh->mTextureCoords[0] != nullptr)
                    {
                        newVertex.texcoord = Vector2(
                            mesh->mTextureCoords[0][j].x,
                            mesh->mTextureCoords[0][j].y
                        );
                    }

                    /*if (mesh->mTangents != nullptr)
                    {
                        newVertex.tangent = glm::vec3(
                            mesh->mTangents[j].x,
                            mesh->mTangents[j].y,
                            mesh->mTangents[j].z
                        );

                        newVertex.bitangent = glm::vec3(
                            mesh->mBitangents[j].x,
                            mesh->mBitangents[j].y,
                            mesh->mBitangents[j].z
                        );
                    }*/

                    vertices.push_back(newVertex);
                }

                for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
                {
                    aiFace face = mesh->mFaces[j];
                    for (unsigned int k = 0; k < face.mNumIndices; ++k)
                    {
                        indices.push_back(face.mIndices[k]);
                    }
                }
            }

            return Create(vertices, indices);
        }
    }

    void StaticMesh::Destroy()
    {
        LOG_INFO("[LOG] Delete static mesh {0:#x}\n", (uint64_t)this);
        mAllocator.deleteElement(this);
    }
}

/* EXPORTED INTERFACE */
extern "C"
{
    LAVA_API void* CreateMesh_Native(Engine::Vertex* vertices, int verticesLength,
        uint32_t* indices, int indicesLength)
    {
        return CREATE_STATIC_MESH();
    }

    LAVA_API void* CreateMesh2D_Native(Engine::Vertex2D* vertices, int verticesLength,
        uint32_t* indices, int indicesLength)
    {
        return CREATE_STATIC_MESH(2D);
    }

    LAVA_API void* CreateMeshExt_Native(Engine::VertexExt* vertices, int verticesLength,
        uint32_t* indices, int indicesLength)
    {
        return CREATE_STATIC_MESH(Ext);
    }

    LAVA_API void* LoadFromFile_Native(const char* path)
    {
        /*std::ifstream fin(path, std::ifstream::binary);
        assert(fin.good());

        fin.seekg(0, fin.end);
        size_t size = fin.tellg();
        fin.seekg(0, fin.beg);
        
        char* buffer = new char[size];
        fin.read(buffer, size);
        fin.close();*/

        auto mesh = Engine::StaticMesh::Load(path);
        //delete[] buffer;

        return mesh;
    }
}
