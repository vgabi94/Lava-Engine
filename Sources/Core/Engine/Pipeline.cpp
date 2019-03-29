#include "Pipeline.h"
#include "Swapchain.h"
#include <Common\PushConstantsStructs.h>
#include <Common\VertexDataTypes.h>
#include <Manager\ShaderManager.h>
#include <Manager\ResourceManager.h>
#include "Engine.h"
#include <json.hpp>
#include <fstream>
#include "PipelineHelper.h"
#include <algorithm>
#include <unordered_map>
#include <abulafia_all.h>

namespace Engine
{
#define HAS_PROPERTY(prop) j.find(prop) != j.end()
#define MAKE_BINDING(location, attr, type) vk::DescriptorSetLayoutBinding binding;\
    binding.binding = location;\
    binding.descriptorCount = 1;\
    binding.descriptorType = GetDescriptorType(type, attr);\
    binding.stageFlags = GetShaderStageFlag(file);\
    auto it = std::find_if(bindings.begin(), bindings.end(),\
        [location](const vk::DescriptorSetLayoutBinding& e) { return e.binding == location; });\
    if (it != bindings.end())\
    {\
        it->stageFlags |= binding.stageFlags;\
    }\
    else\
    {\
        bindings.push_back(binding);\
    }
#define temp Pipeline::TempGraphicsPipelineCI

    GraphicsPipelineCI Pipeline::BaseGraphicsPipelineCI;
    GraphicsPipelineCI Pipeline::TempGraphicsPipelineCI;

    // ---- Parser patterns ---- //
    namespace Patterns
    {
        struct Destination
        {
            uint32_t set;
            uint32_t binding;
            std::string attr;
            std::string type;
            std::string ident;
            
            Destination() = default;

            Destination(const std::string& attr, uint32_t s, uint32_t b, const std::string& t, const std::string& ide)
                : set(s), binding(b), type(t), ident(ide), attr(attr) { }

            Destination(const std::string& attr, uint32_t b, const std::string& t, const std::string& ide)
                : set(0), binding(b), type(t), ident(ide), attr(attr) { }
        };

        static auto attr = +abu::char_("ABCDEFGHIJKLMNOPQRSTUVWXYZ0");
        static auto alphaNum = +abu::char_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
        static auto space = abu::char_(" \t\r\n");
        static auto uniform0 = attr >> abu::lit('(') >> abu::int_ >>
            abu::lit(',') >> alphaNum >> abu::lit(',') >> alphaNum >> abu::lit(')') >> abu::lit(';');
        static auto uniform = attr >> abu::lit('(') >> abu::int_ >> abu::lit(',') >> abu::int_ >>
            abu::lit(',') >> alphaNum >> abu::lit(',') >> alphaNum >> abu::lit(')') >> abu::lit(';');
        static auto layout = uniform0.as<std::string, uint32_t, std::string, std::string>()
            | uniform.as<std::string, uint32_t, uint32_t, std::string, std::string>();
        static auto mainPattern = abu::apply_skipper(layout, space);

		/*static auto slot = attr >> abu::lit('(') >> abu::int_ >> abu::lit(')');
		static auto slotPattern = abu::apply_skipper(slot.as<uint32_t>(), space);*/
    }
    // ------------------------- //

    static void BuildTempGraphicsPipelineCI(const nlohmann::json& j)
    {
        Pipeline::TempGraphicsPipelineCI = Pipeline::BaseGraphicsPipelineCI;

        if (HAS_PROPERTY("base"))
        {
            std::string base = j["base"];
            if (base != "none" && base != "base.json")
            {
                LOG_ERROR("Pipeline error: Inheritance not supported at this moment.");
            }
        }
        if (HAS_PROPERTY("vertexInput"))
        {
            std::string type = j["vertexInput"];
            if (IsValidVertexType(type))
            {
                GetVertexInputCI(type, temp);
            }
            else
            {
                LOG_ERROR("Pipeline error: Invalid vertex input type {0}!", type.c_str());
            }
        }
        if (HAS_PROPERTY("primitiveTopology"))
        {
            std::string topo = j["primitiveTopology"];
            if (IsValidPrimitiveTopology(topo))
            {
                GetInputAssemblyCI(topo, temp);
            }
            else
            {
                LOG_ERROR("Pipeline error: Invalid primitive topology {}!", topo.c_str());
            }
        }
        if (HAS_PROPERTY("polygonMode"))
        {
            std::string polyMode = j["polygonMode"];
            if (IsValidPolygonMode(polyMode))
            {
                GetPolygonMode(polyMode, temp);
            }
            else
            {
                LOG_ERROR("Pipeline error: Invalid polygon mode {}!", polyMode.c_str());
            }
            
        }
        if (HAS_PROPERTY("cullMode"))
        {
            std::string cullMode = j["cullMode"];
            if (IsValidCullMode(cullMode))
            {
                GetCullMode(cullMode, temp);
            }
            else
            {
                LOG_ERROR("Pipeline error: Invalid cull mode {}!", cullMode.c_str());
            }
        }
        if (HAS_PROPERTY("frontFace"))
        {
            std::string frontFace = j["frontFace"];
            if (IsValidFrontFace(frontFace))
            {
                GetFrontFace(frontFace, temp);
            }
            else
            {
                LOG_ERROR("Pipeline error: Invalid front face {}!", frontFace.c_str());
            }
        }
        if (HAS_PROPERTY("depthClampEnable"))
        {
            bool value = j["depthClampEnable"];
            temp.mRasterization.depthClampEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("rasterizerDiscardEnable"))
        {
            bool value = j["rasterizerDiscardEnable"];
            temp.mRasterization.rasterizerDiscardEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("depthBiasEnable"))
        {
            bool value = j["depthBiasEnable"];
            temp.mRasterization.depthBiasEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("depthBiasConstantFactor"))
        {
            temp.mRasterization.depthBiasConstantFactor = j["depthBiasConstantFactor"];
        }
        if (HAS_PROPERTY("depthBiasClamp"))
        {
            temp.mRasterization.depthBiasClamp = j["depthBiasClamp"];
        }
        if (HAS_PROPERTY("depthBiasSlopeFactor"))
        {
            temp.mRasterization.depthBiasSlopeFactor = j["depthBiasSlopeFactor"];
        }
        if (HAS_PROPERTY("lineWidth"))
        {
            temp.mRasterization.lineWidth = j["lineWidth"];
        }
        if (HAS_PROPERTY("rasterizationSamples"))
        {
            int value = j["rasterizationSamples"];
            temp.mMultisample.rasterizationSamples = static_cast<vk::SampleCountFlagBits>(value);
        }
        if (HAS_PROPERTY("sampleShadingEnable"))
        {
            bool value = j["sampleShadingEnable"];
            temp.mMultisample.sampleShadingEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("minSampleShading"))
        {
            temp.mMultisample.minSampleShading = j["minSampleShading"];
        }
        if (HAS_PROPERTY("sampleMask"))
        {
            if (j["sampleMask"] == nullptr)
            {
                temp.mSampleMask.reset();
            }
            else
            {
                temp.mSampleMask = static_cast<vk::SampleMask>(j["sampleMask"]);
            }
        }
        if (HAS_PROPERTY("alphaToCoverageEnable"))
        {
            bool value = j["alphaToCoverageEnable"];
            temp.mMultisample.alphaToCoverageEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("alphaToOneEnable"))
        {
            bool value = j["alphaToOneEnable"];
            temp.mMultisample.alphaToOneEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("blendEnable"))
        {
            bool value = j["blendEnable"];
            temp.mColorBlendAttachment.blendEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("srcColorBlendFactor"))
        {
            temp.mColorBlendAttachment.srcColorBlendFactor = static_cast<vk::BlendFactor>(j["srcColorBlendFactor"]);
            // TODO serialize BlendFactor enum
            // https://github.com/nlohmann/json
        }
        if (HAS_PROPERTY("dstColorBlendFactor"))
        {
            temp.mColorBlendAttachment.dstColorBlendFactor = static_cast<vk::BlendFactor>(j["dstColorBlendFactor"]);
        }
        if (HAS_PROPERTY("colorBlendOp"))
        {
            temp.mColorBlendAttachment.colorBlendOp = static_cast<vk::BlendOp>(j["colorBlendOp"]);
            // TODO add support for all values
        }
        if (HAS_PROPERTY("srcAlphaBlendFactor"))
        {
            temp.mColorBlendAttachment.srcAlphaBlendFactor = static_cast<vk::BlendFactor>(j["srcAlphaBlendFactor"]);
        }
        if (HAS_PROPERTY("dstAlphaBlendFactor"))
        {
            temp.mColorBlendAttachment.dstAlphaBlendFactor = static_cast<vk::BlendFactor>(j["dstAlphaBlendFactor"]);
        }
        if (HAS_PROPERTY("alphaBlendOp"))
        {
            temp.mColorBlendAttachment.alphaBlendOp = static_cast<vk::BlendOp>(j["alphaBlendOp"]);
        }
        if (HAS_PROPERTY("colorWriteMask"))
        {
            temp.mColorBlendAttachment.colorWriteMask = static_cast<vk::ColorComponentFlagBits>(j["colorWriteMask"]);
        }
        if (HAS_PROPERTY("logicOpEnable"))
        {
            temp.mColorBlendState.logicOpEnable = static_cast<vk::Bool32>(j["logicOpEnable"]);
        }
        if (HAS_PROPERTY("logicOp"))
        {
            temp.mColorBlendState.logicOp = static_cast<vk::LogicOp>(j["logicOp"]);
            // TODO serialize LogicOp enum
            // https://github.com/nlohmann/json
        }
        if (HAS_PROPERTY("depthTestEnable"))
        {
            bool value = j["depthTestEnable"];
            temp.mDepthStencil.depthTestEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("depthWriteEnable"))
        {
            bool value = j["depthWriteEnable"];
            temp.mDepthStencil.depthWriteEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("depthCompareOp"))
        {
            temp.mDepthStencil.depthCompareOp = GetCompareOp(j["depthCompareOp"]);
        }
        if (HAS_PROPERTY("stencilTestEnable"))
        {
            bool value = j["stencilTestEnable"];
            temp.mDepthStencil.stencilTestEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("depthBoundsTestEnable"))
        {
            bool value = j["depthBoundsTestEnable"];
            temp.mDepthStencil.depthBoundsTestEnable = static_cast<vk::Bool32>(value);
        }
        if (HAS_PROPERTY("shaders"))
        {
            std::vector<std::string> shaders = j["shaders"];
            temp.mShaderNames = shaders;
        }
        if (HAS_PROPERTY("renderPass"))
        {
            std::string renderPass = j["renderPass"];
			temp.mRenderPass = g_RenderpassManager.GetPass(renderPass)->GetVkObject();
        }
		if (HAS_PROPERTY("globalsets"))
		{
			temp.mGlobalSets.assign(j["globalsets"].begin(), j["globalsets"].end());
		}
    }
 
    Pipeline Pipeline::FromJSON(const char * jsonFile)
    {
        THROW_IF(std::string(jsonFile).find("base.json") != std::string::npos, "The base pipeline has already been loaded!");

        using json = nlohmann::json;

        std::ifstream fin(jsonFile);
        json j;
        fin >> j;

        BuildTempGraphicsPipelineCI(j);

        vk::GraphicsPipelineCreateInfo graphicsPipelineCI;
        TempGraphicsPipelineCI.ToVulkanType(graphicsPipelineCI);

        std::vector<std::string> shadersFullPath;
        shadersFullPath.resize(temp.mShaderNames.size());
        for (size_t i = 0; i < shadersFullPath.size(); i++)
        {
            shadersFullPath[i] = g_EngineSettings.ShaderModulePath + "\\" + temp.mShaderNames[i] + ".spv";
        }
        auto shaderStages = g_ShaderManager.CreateShaderStages(shadersFullPath);

        // TODO check if pipeline name matches shader name

        graphicsPipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
        graphicsPipelineCI.pStages = shaderStages.data();

        Pipeline pipeline;
        pipeline.BuildPipelineLayout(temp.mShaderNames);
        graphicsPipelineCI.layout = pipeline.mPipelineLayout;
        pipeline.mPipeline = g_vkDevice.createGraphicsPipeline(nullptr, graphicsPipelineCI);

        return pipeline;
    }

    void Pipeline::BuildBase(const char * jsonFile)
    {
        THROW_IF(std::string(jsonFile).find("base.json") == std::string::npos, "The base pipeline file must be base.json!");

        using json = nlohmann::json;

        std::ifstream fin(jsonFile);
        json j;
        fin >> j;

        BuildTempGraphicsPipelineCI(j);
        BaseGraphicsPipelineCI = TempGraphicsPipelineCI;
    }
    
    vk::DescriptorSet Pipeline::AllocateDescriptorSet()
    {
        return mDescAllocator.AllocateDescriptorSet();
    }

	void Pipeline::BindGlobalDescSets(vk::CommandBuffer cmdBuff) const
	{
		if (mSetIndices.empty()) return;

		uint32_t firstSet = mSetIndices[0];
		std::vector<vk::DescriptorSet> descSets;
		descSets.reserve(mSetIndices.size());
		
		for (auto slot : mSetIndices)
		{
			auto ds = g_ResourceManager.GetDescriptorSet(slot);
			THROW_IF(!ds, "Cannot bind null descriptor set!");
			descSets.push_back(ds);
		}
		
		cmdBuff.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			mPipelineLayout, firstSet, descSets.size(), descSets.data(),
			0, nullptr);
	}

    void Pipeline::BuildPipelineLayout(const std::vector<std::string>& shaderNames)
    {
        std::vector<std::string> shaderPath;
        shaderPath.resize(shaderNames.size());
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        for (size_t i = 0; i < shaderNames.size(); i++)
        {
            shaderPath[i] = g_EngineSettings.ShaderSourcePath + "\\" + shaderNames[i];
            GetBindingsFromShader(shaderPath[i].c_str(), bindings);
        }

		// Needed for uniform initialization in materials
        mBindings = bindings;

        // Convert to poolSizes and init the descriptor allocator
        std::vector<vk::DescriptorPoolSize> poolSizes;
        std::unordered_map<vk::DescriptorType, uint32_t> freq;
        for (size_t i = 0; i < bindings.size(); i++)
        {
            ++freq[bindings[i].descriptorType];
        }
        for (auto it = freq.begin(); it != freq.end(); it++)
        {
            poolSizes.emplace_back(it->first, DescriptorAllocator::MAX_NUM_SETS * it->second);
        }

        // Create descriptor set layout
        vk::DescriptorSetLayoutCreateInfo descSetCI({},
            static_cast<uint32_t>(bindings.size()),
            bindings.data());

        mDescriptorSetLayout = g_vkDevice.createDescriptorSetLayout(descSetCI);

        mDescAllocator.Init(poolSizes, mDescriptorSetLayout);
        
        vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eVertex
            | vk::ShaderStageFlagBits::eFragment, 0, sizeof(ObjPS));

		// ADD custom push constants here!
		if (shaderNames[0].find("sky") != std::string::npos
			|| shaderNames[0].find("filtercube") != std::string::npos)
		{
			pushConstantRange.size = sizeof(SkyPS);
		}

		// Copy global sets from temp to this pipeline
		for (auto i : temp.mGlobalSets)
		{
			AddSetIndex(i);
		}

		std::sort(mSetIndices.begin(), mSetIndices.end());

        // Add all descriptor layout sets used by this pipeline
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.reserve(1 + mSetIndices.size());
        descriptorSetLayouts.push_back(mDescriptorSetLayout);

        for (size_t i = 0; i < mSetIndices.size(); i++)
        {
            descriptorSetLayouts.push_back(g_ResourceManager.GetDescriptorSetLayoutAt(mSetIndices[i]));
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({},
            static_cast<uint32_t>(descriptorSetLayouts.size()),
            descriptorSetLayouts.data(),
            1, &pushConstantRange);
        mPipelineLayout = g_vkDevice.createPipelineLayout(pipelineLayoutInfo);
    }

    void Pipeline::GetBindingsFromShader(const char * file, std::vector<vk::DescriptorSetLayoutBinding>& bindings)
    {
        std::ifstream in(file);
        std::string line;
		// TODO DELETE
        //std::vector<uint32_t> setIndices;

        while (std::getline(in, line))
        {
            if (line.find("UNIFORM") == 0 || line.find("BUFFER") == 0)
            {
                Patterns::Destination dest;
                auto ok = abu::parse(line.begin(), line.end(), Patterns::mainPattern, dest);
                THROW_IF(ok != abu::Result::SUCCESS, "Parser error at line: {}", line);
                
                vk::DescriptorSetLayoutBinding binding;
                if (dest.set == 0)
                {
                    vk::DescriptorSetLayoutBinding binding; 
                        binding.binding = dest.binding; 
                        binding.descriptorCount = 1; 
                        binding.descriptorType = GetDescriptorType(dest.type, dest.attr); 
                        binding.stageFlags = GetShaderStageFlag(file); 
                        auto it = std::find_if(bindings.begin(), bindings.end(), 
                            [&dest](const vk::DescriptorSetLayoutBinding& e) { return e.binding == dest.binding; });
                        if (it != bindings.end())
                        {
                            it->stageFlags |= binding.stageFlags; 
                        }
                        else
                        {
                            bindings.push_back(binding); 
                        }
                }
                else
                {
					THROW("Slots other than 0 are not supported!");
                }
            }
			/*else if (line.find("SLOT") == 0)
			{
				uint32_t dest;
				auto ok = abu::parse(line.begin(), line.end(), Patterns::slotPattern, dest);
				THROW_IF(ok != abu::Result::SUCCESS, "Slot parsing error at line: {}", line);
				setIndices.push_back(dest);
			}*/
        }

        /*for (auto i : setIndices)
        {
            AddSetIndex(i);
        }*/
    }

    void Pipeline::AddSetIndex(uint32_t index)
    {
        bool found = false;
        for (size_t i = 0; i < mSetIndices.size(); i++)
        {
            if (mSetIndices[i] == index)
            {
                found = true;
                break;
            }
        }
        if (!found)
            mSetIndices.push_back(index);
    }

    void GraphicsPipelineCI::ToVulkanType(vk::GraphicsPipelineCreateInfo& gpCI)
    {
        mVertexInput.vertexBindingDescriptionCount = 1;
        mVertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(mVertexInputAttributeDesc.size());
        mVertexInput.pVertexBindingDescriptions = &mVertexInputBindingDesc;
        mVertexInput.pVertexAttributeDescriptions = mVertexInputAttributeDesc.data();

        mMultisample.pSampleMask = mSampleMask.has_value() ? &mSampleMask.value() : nullptr;

        mColorBlendState.attachmentCount = 1;
        mColorBlendState.pAttachments = &mColorBlendAttachment;

        gpCI.pVertexInputState = &mVertexInput;
        gpCI.pInputAssemblyState = &mInputAssembly;

        mDynamicStates[0] = vk::DynamicState::eViewport;
        mDynamicStates[1] = vk::DynamicState::eScissor;
        mDynamicStateCI.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
        mDynamicStateCI.pDynamicStates = mDynamicStates.data();
        gpCI.pDynamicState = &mDynamicStateCI;

        mViewportState.viewportCount = 1;
        mViewportState.pViewports = nullptr;
        mViewportState.scissorCount = 1;
        mViewportState.pScissors = nullptr;
        gpCI.pViewportState = &mViewportState;

        gpCI.pTessellationState = nullptr;

        gpCI.pRasterizationState = &mRasterization;

        gpCI.pMultisampleState = &mMultisample;

        gpCI.pDepthStencilState = &mDepthStencil;

        gpCI.pColorBlendState = &mColorBlendState;

        gpCI.renderPass = mRenderPass;
        gpCI.subpass = 0;
    }

#undef HAS_PROPERTY
#undef MAKE_BINDING
#undef temp
}
