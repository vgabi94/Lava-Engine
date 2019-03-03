#pragma once

#define VERTEX_INPUT_CREATE_INFO(Type) auto bindingDescription = Type::GetBindingDescription();\
auto attributeDescriptions = Type::GetAttributeDescriptions();\
temp.mVertexInputBindingDesc = bindingDescription;\
temp.mVertexInputAttributeDesc.resize(attributeDescriptions.size());\
temp.mVertexInputAttributeDesc.assign(attributeDescriptions.begin(), attributeDescriptions.end())

#define INPUT_ASSEMBLY_CREATE_INFO(Type) temp.mInputAssembly.topology = vk::PrimitiveTopology::e##Type;\
temp.mInputAssembly.primitiveRestartEnable = VK_FALSE

#define POLYGON_MODE(Type) temp.mRasterization.polygonMode = vk::PolygonMode::e##Type

#define CULL_MODE(Type) temp.mRasterization.cullMode = vk::CullModeFlagBits::e##Type

#define FRONT_FACE(Type) temp.mRasterization.frontFace = vk::FrontFace::e##Type

namespace Engine
{
    static bool IsTextureOrSampler(const std::string& type)
    {
        if (type == "sampler2D" || type == "texture2D" || type == "samplerCube"
            || type == "sampler" || type == "textureCube" || type == "texture3D"
            || type == "sampler3D" || type == "sampler1D" || type == "texture1D"
            || type == "texture2DArray" || type == "texture1DArray" || type == "textureBuffer"
            || type == "image1D" || type == "image2D" || type == "image3D"
            || type == "samplerBuffer" || type == "imageBuffer" || type == "texture3DArray")
        {
            return true;
        }
        return false;
    }

    static vk::DescriptorType GetDescriptorType(const std::string& type, const std::string& attr)
    {
        if (attr == "UNIFORM" || attr == "UNIFORM0") // TODO error in this funciton
        {
            if (type == "sampler")
            {
                return vk::DescriptorType::eSampler;
            }
            else if (type == "samplerBuffer")
            {
                return vk::DescriptorType::eUniformTexelBuffer;
            }
            else if (type.find("sampler") != std::string::npos)
            {
                return vk::DescriptorType::eCombinedImageSampler;
            }
            else if (type.find("texture") != std::string::npos)
            {
                return vk::DescriptorType::eSampledImage;
            }
            else if (type == "imageBuffer")
            {
                return vk::DescriptorType::eStorageTexelBuffer;
            }
            else if (type.find("image") != std::string::npos)
            {
                return vk::DescriptorType::eStorageImage;
            }
            else
            {
                return vk::DescriptorType::eUniformBuffer; // TODO dynamic buffers
            }
        }
        else // if buffer
        {
            return vk::DescriptorType::eStorageBuffer;
        }
    }

	static vk::CompareOp GetCompareOp(const std::string& op)
	{
		if (op == "never")
			return vk::CompareOp::eNever;
		if (op == "less")
			return vk::CompareOp::eLess;
		if (op == "equal")
			return vk::CompareOp::eEqual;
		if (op == "lequal")
			return vk::CompareOp::eLessOrEqual;
		if (op == "greater")
			return vk::CompareOp::eGreater;
		if (op == "notequal")
			return vk::CompareOp::eNotEqual;
		if (op == "gequal")
			return vk::CompareOp::eGreaterOrEqual;
		if (op == "always")
			return vk::CompareOp::eAlways;
		
		return vk::CompareOp::eNever;
	}

    static bool IsValidVertexType(const std::string& type)
    {
        if (type == "Vertex" || type == "Vertex2D" || type == "VertexExt" || type == "VertexPos")
            return true;
        return false;
    }

    static bool IsValidPrimitiveTopology(const std::string& type)
    {
        if (type == "PointList" || type == "LineList" || type == "LineStrip"
            || type == "TriangleList" || type == "TriangleStrip" || type == "TriangleFan"
            || type == "PatchList")
        {
            return true;
        }
        return false;
    }

    static bool IsValidPolygonMode(const std::string& type)
    {
        if (type == "Fill" || type == "Line" || type == "Point" || type == "FillRectangleNV")
            return true;
        return false;
    }

    static bool IsValidCullMode(const std::string& type)
    {
        if (type == "Front" || type == "Back" || type == "FrontAndBack" || type == "None")
            return true;
        return false;
    }

    static bool IsValidFrontFace(const std::string& type)
    {
        if (type == "CounterClockwise" || type == "Clockwise")
            return true;
        return false;
    }

    static void GetVertexInputCI(std::string type, GraphicsPipelineCI& temp)
    {
        vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo;
        
        if (type == "Vertex2D")
        {
            VERTEX_INPUT_CREATE_INFO(Vertex2D);
        }
        else if (type == "Vertex")
        {
            VERTEX_INPUT_CREATE_INFO(Vertex);
        }
        else if (type == "VertexExt")
        {
            VERTEX_INPUT_CREATE_INFO(VertexExt);
        }
		else if (type == "VertexPos")
		{
			VERTEX_INPUT_CREATE_INFO(VertexPos);
		}
    }

    static void GetInputAssemblyCI(std::string type, GraphicsPipelineCI& temp)
    {
        if (type == "PointList")
        {
            INPUT_ASSEMBLY_CREATE_INFO(PointList);
        }
        else if (type == "LineList")
        {
            INPUT_ASSEMBLY_CREATE_INFO(LineList);
        }
        else if (type == "LineStrip")
        {
            INPUT_ASSEMBLY_CREATE_INFO(LineStrip);
        }
        else if (type == "TriangleList")
        {
            INPUT_ASSEMBLY_CREATE_INFO(TriangleList);
        }
        else if (type == "TriangleStrip")
        {
            INPUT_ASSEMBLY_CREATE_INFO(TriangleStrip);
        }
        else if (type == "TriangleFan")
        {
            INPUT_ASSEMBLY_CREATE_INFO(TriangleFan);
        }
        else if (type == "PatchList")
        {
            INPUT_ASSEMBLY_CREATE_INFO(PatchList);
        }
    }

    static void GetPolygonMode(std::string type, GraphicsPipelineCI& temp)
    {
        if (type == "Fill")
        {
            POLYGON_MODE(Fill);
        }
        else if (type == "Line")
        {
            POLYGON_MODE(Line);
        }
        else if (type == "Point")
        {
            POLYGON_MODE(Point);
        }
        else if (type == "FillRectangleNV")
        {
            POLYGON_MODE(FillRectangleNV);
        }
    }

    static void GetCullMode(std::string type, GraphicsPipelineCI& temp)
    {
        if (type == "Back")
        {
            CULL_MODE(Back);
        }
        else if (type == "Front")
        {
            CULL_MODE(Front);
        }
        else if (type == "FrontAndBack")
        {
            CULL_MODE(FrontAndBack);
        }
        else if (type == "None")
        {
            CULL_MODE(None);
        }
    }

    static void GetFrontFace(std::string type, GraphicsPipelineCI& temp)
    {
        if (type == "CounterClockwise")
        {
            FRONT_FACE(CounterClockwise);
        }
        else if (type == "Clockwise")
        {
            FRONT_FACE(Clockwise);
        }
    }
}

#undef VERTEX_INPUT_CREATE_INFO
#undef INPUT_ASSEMBLY_CREATE_INFO
#undef POLYGON_MODE
#undef CULL_MODE