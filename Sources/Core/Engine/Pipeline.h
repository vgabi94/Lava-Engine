#pragma once

#include "DescriptorAllocator.h"
#include <optional>
#include <array>

namespace Engine
{
    struct GraphicsPipelineCI
    {
        vk::VertexInputBindingDescription mVertexInputBindingDesc;
        std::vector<vk::VertexInputAttributeDescription> mVertexInputAttributeDesc;
        vk::PipelineVertexInputStateCreateInfo mVertexInput;

        vk::PipelineInputAssemblyStateCreateInfo mInputAssembly;

        vk::PipelineRasterizationStateCreateInfo mRasterization;

        std::optional<vk::SampleMask> mSampleMask;
        vk::PipelineMultisampleStateCreateInfo mMultisample;

        vk::PipelineColorBlendAttachmentState mColorBlendAttachment;
        vk::PipelineColorBlendStateCreateInfo mColorBlendState;

        vk::PipelineDepthStencilStateCreateInfo mDepthStencil;

        std::array<vk::DynamicState, 2> mDynamicStates;
        vk::PipelineDynamicStateCreateInfo mDynamicStateCI;

        vk::PipelineViewportStateCreateInfo mViewportState;

        vk::RenderPass mRenderPass;

        std::vector<std::string> mShaderNames;

        void ToVulkanType(vk::GraphicsPipelineCreateInfo& gpCI);
    };

    typedef std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageList;

    struct Pipeline
    {
        static Pipeline FromJSON(const char* jsonFile);
        static void BuildBase(const char* jsonFile);

        void Destroy()
        {
            mDescAllocator.Destroy();
            g_vkDevice.destroyDescriptorSetLayout(mDescriptorSetLayout);
            g_vkDevice.destroyPipelineLayout(mPipelineLayout);
            g_vkDevice.destroyPipeline(mPipeline);
        }

        vk::DescriptorSet AllocateDescriptorSet();

        vk::Pipeline mPipeline;
        vk::PipelineLayout mPipelineLayout;

        static GraphicsPipelineCI BaseGraphicsPipelineCI;
        static GraphicsPipelineCI TempGraphicsPipelineCI;

        // Set inidices for global data (buffers, uniforms) which do not belong
        // to the material of the entity
        std::vector<uint32_t> mSetIndices;

        std::vector<vk::DescriptorSetLayoutBinding> mBindings;

    private:
        void BuildPipelineLayout(const std::vector<std::string>& shaderNames);
        void GetBindingsFromShader(const char* file, std::vector<vk::DescriptorSetLayoutBinding>& bindings);
        void AddSetIndex(uint32_t index);

        vk::DescriptorSetLayout mDescriptorSetLayout;
        DescriptorAllocator mDescAllocator;
    };
}