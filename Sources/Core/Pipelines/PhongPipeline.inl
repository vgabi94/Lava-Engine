Pipeline MakePhong(const ShaderStageList& shaderStageList, vk::RenderPass renderPass)
{
    VERTEX_INPUT_CREATE_INFO(Vertex);

    INPUT_ASSEMBLY_CREATE_INFO(TriangleList);

    DYNAMIC_VIEWPORT_AND_SCISSORS;

    RASTERIZER(Fill, Back, CounterClockwise);

    MULTISAMPLING_NONE;

    BLENDING_DISABLE;

    DEPTH_ENABLE;

    vk::DescriptorSetLayoutBinding albedoBinding(
        0, vk::DescriptorType::eCombinedImageSampler,
        1, vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutCreateInfo descSetCI({}, 1, &albedoBinding);
    vk::DescriptorSetLayout descSet = g_vkDevice.createDescriptorSetLayout(descSetCI);

    vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eVertex
        | vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstants));
    
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1, &descSet, 1, &pushConstantRange);
    vk::PipelineLayout pipelineLayout = g_vkDevice.createPipelineLayout(pipelineLayoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo({},
        shaderStageList.size(),
        shaderStageList.data(),
        &vertexInputCreateInfo,
        &inputAssemblyCreateInfo,
        nullptr,
        &viewportState,
        &rasterizer,
        &multisampling,
        &depthStencil,
        &colorBlending,
        &dynamicStateCreateInfo,
        pipelineLayout,
        renderPass,
        0
    );

    Pipeline pipeline;
    pipeline.mPipeline = g_vkDevice.createGraphicsPipeline(nullptr, pipelineInfo);
    pipeline.mPipelineLayout = pipelineLayout;
    pipeline.mDescriptorSetLayout = descSet;

    return pipeline;
}