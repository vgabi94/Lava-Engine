#pragma once
#include "MathTypes.h"
#include <vulkan\vulkan.hpp>
#include <array>

namespace Engine
{
    struct Vertex2D
    {
        Vector3 position;
        Vector2 texcoord;

        static vk::VertexInputBindingDescription GetBindingDescription(uint32_t binding = 0,
            vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex)
        {
            return vk::VertexInputBindingDescription(
                binding,
                sizeof(Vertex2D),
                inputRate
            );
        }

        static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions(uint32_t binding = 0)
        {
            return std::array<vk::VertexInputAttributeDescription, 2>
            {
                vk::VertexInputAttributeDescription(0, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex2D, position)),
                    vk::VertexInputAttributeDescription(1, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex2D, texcoord))
            };
        }
    };

    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector2 texcoord;

        static vk::VertexInputBindingDescription GetBindingDescription(uint32_t binding = 0, 
            vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex)
        {
            return vk::VertexInputBindingDescription(
                binding,
                sizeof(Vertex),
                inputRate
            );
        }

        static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions(uint32_t binding = 0)
        {
            return std::array<vk::VertexInputAttributeDescription, 3>
            {
                vk::VertexInputAttributeDescription(0, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)),
                vk::VertexInputAttributeDescription(1, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)),
                vk::VertexInputAttributeDescription(2, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex, texcoord))
            };
        }
    };

    struct VertexExt
    {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        Vector2 texcoord;

        static vk::VertexInputBindingDescription GetBindingDescription(uint32_t binding = 0,
            vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex)
        {
            return vk::VertexInputBindingDescription(
                binding,
                sizeof(VertexExt),
                inputRate
            );
        }

        static std::array<vk::VertexInputAttributeDescription, 4> GetAttributeDescriptions(uint32_t binding = 0)
        {
            return std::array<vk::VertexInputAttributeDescription, 4>
            {
                vk::VertexInputAttributeDescription(0, binding, vk::Format::eR32G32B32Sfloat, offsetof(VertexExt, position)),
                    vk::VertexInputAttributeDescription(1, binding, vk::Format::eR32G32B32Sfloat, offsetof(VertexExt, normal)),
                    vk::VertexInputAttributeDescription(2, binding, vk::Format::eR32G32B32Sfloat, offsetof(VertexExt, tangent)),
                    vk::VertexInputAttributeDescription(3, binding, vk::Format::eR32G32Sfloat, offsetof(VertexExt, texcoord))
            };
        }
    };

	struct VertexPos
	{
		Vector3 position;

		VertexPos(float x, float y, float z) : position(x, y, z) { }

		static vk::VertexInputBindingDescription GetBindingDescription(uint32_t binding = 0,
			vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex)
		{
			return vk::VertexInputBindingDescription(
				binding,
				sizeof(VertexPos),
				inputRate
			);
		}

		static std::array<vk::VertexInputAttributeDescription, 1> GetAttributeDescriptions(uint32_t binding = 0)
		{
			return std::array<vk::VertexInputAttributeDescription, 1>
			{
				vk::VertexInputAttributeDescription(0, binding, vk::Format::eR32G32B32Sfloat, offsetof(VertexPos, position))
			};
		}
	};

    typedef std::vector<Vertex> VertexList;
    typedef std::vector<VertexPos> VertexPosList;
    typedef std::vector<Vertex2D> Vertex2DList;
    typedef std::vector<VertexExt> VertexExtList;
    typedef std::vector<uint32_t> IndexList;
}