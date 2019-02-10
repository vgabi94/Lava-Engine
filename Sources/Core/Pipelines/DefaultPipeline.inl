Pipeline MakeDefault(const ShaderStageList& shaderStageList, vk::RenderPass renderPass)
{
    VERTEX_INPUT_CREATE_INFO(Vertex2D);

    INPUT_ASSEMBLY_CREATE_INFO(TriangleList);

    DYNAMIC_VIEWPORT_AND_SCISSORS;

    RASTERIZER(Fill, Back, Clockwise);

    MULTISAMPLING_NONE;

    BLENDING_DISABLE;

    DEPTH_ENABLE;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 0, nullptr);
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
    return pipeline;
}