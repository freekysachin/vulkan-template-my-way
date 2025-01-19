#define _CRT_SECURE_NO_WARNINGS
#include "pipeline.h"

vk::PipelineLayout vkInit::make_pipeline_layout(vk::Device device, bool debug)
{
    vk::PipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.flags = vk::PipelineLayoutCreateFlags();
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pushConstantRangeCount = 0;
    try {
        return device.createPipelineLayout(layoutInfo);
    }
    catch (vk::SystemError err) {
        if (debug) std::cout << "failed to create pipeline layout" << std::endl;
    }
}

vk::RenderPass vkInit::make_render_pass(vk::Device device, vk::Format swapchainFormat, bool debug)
{
    vk::AttachmentDescription colorAttachment = {};
    colorAttachment.flags = vk::AttachmentDescriptionFlags();
    colorAttachment.format = swapchainFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass = {};
    subpass.flags = vk::SubpassDescriptionFlags();
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::RenderPassCreateInfo renderpassInfo = {};
    renderpassInfo.flags = vk::RenderPassCreateFlags();
    renderpassInfo.attachmentCount = 1;
    renderpassInfo.pAttachments = &colorAttachment;
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpass;

    try
    {
        return device.createRenderPass(renderpassInfo);
    }
    catch (vk::SystemError err)
    {
        if (debug) std::cout << "Failed to create render pass \n";
    }

}

vkInit::GraphicsPiplineOutBundle vkInit::make_graphics_pipeline(GraphicsPipelineInBundle specification, bool debug)
{

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.flags = vk::PipelineCreateFlags();

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;            // shader stages vector

    // Vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.flags = vk::PipelineVertexInputStateCreateFlags();
    vertexInputState.vertexBindingDescriptionCount = 0;
    vertexInputState.vertexAttributeDescriptionCount = 0;

    pipelineCreateInfo.pVertexInputState = &vertexInputState;

    // Input Assembly
    vk::PipelineInputAssemblyStateCreateInfo assemblyState = {};
    assemblyState.flags = vk::PipelineInputAssemblyStateCreateFlags();
    assemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    
    pipelineCreateInfo.pInputAssemblyState = &assemblyState;

    // Vertex Shader                                                        // 1st shader stage here
    if (debug) std::cout << "Create Vertex shader module \n";

    vk::ShaderModule vertexShader = vkUtil::createModule(specification.vertexFilePath, specification.device, debug);
    vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
    vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
    vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertexShaderInfo.module = vertexShader;
    vertexShaderInfo.pName = "main";
    shaderStages.push_back(vertexShaderInfo);

    // Viewport and scissor
    vk::Viewport viewPort = {};
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float)specification.swapchainExtent.width;
    viewPort.height = (float)specification.swapchainExtent.height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    vk::Rect2D scissor = {};
    scissor.offset.x = (int32_t)0.0f;
    scissor.offset.y = (int32_t)0.0f;
    scissor.extent = specification.swapchainExtent;
    vk::PipelineViewportStateCreateInfo viewportState = {};
    viewportState.flags = vk::PipelineViewportStateCreateFlags();
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewPort;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    pipelineCreateInfo.pViewportState = &viewportState;

    // Rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterizerState = {};
    rasterizerState.flags = vk::PipelineRasterizationStateCreateFlags();
    rasterizerState.depthClampEnable = VK_FALSE;
    rasterizerState.rasterizerDiscardEnable = VK_FALSE;
    rasterizerState.polygonMode = vk::PolygonMode::eFill;
    rasterizerState.lineWidth = 1.0f;
    rasterizerState.cullMode = vk::CullModeFlagBits::eBack;
    rasterizerState.frontFace = vk::FrontFace::eClockwise;
    rasterizerState.depthBiasEnable = VK_FALSE;

    pipelineCreateInfo.pRasterizationState = &rasterizerState;

    //  Fragment Shader                                                  // 2nd shader stage here
    if (debug) std::cout << "Create Fragment shader module \n";

    vk::ShaderModule fragShader = vkUtil::createModule(specification.fragFilePath, specification.device, debug);
    vk::PipelineShaderStageCreateInfo fragShaderInfo = {};
    fragShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
    fragShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderInfo.module = fragShader;
    fragShaderInfo.pName = "main";
    shaderStages.push_back(fragShaderInfo);

    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();

    // Multisampling  -- We are choosng 'No Multi Sampling'
    vk::PipelineMultisampleStateCreateInfo multisamplingState = {};
    multisamplingState.flags = vk::PipelineMultisampleStateCreateFlags();
    multisamplingState.sampleShadingEnable = VK_FALSE;
    multisamplingState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    pipelineCreateInfo.pMultisampleState = &multisamplingState;

    // Color Blend
    vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    vk::PipelineColorBlendStateCreateInfo colorBlendingState = {};
    colorBlendingState.flags = vk::PipelineColorBlendStateCreateFlags();
    colorBlendingState.logicOpEnable = VK_FALSE;
    colorBlendingState.logicOp = vk::LogicOp::eCopy;
    colorBlendingState.attachmentCount = 1;
    colorBlendingState.pAttachments = &colorBlendAttachment;
    colorBlendingState.blendConstants[0] = 0.0f;
    colorBlendingState.blendConstants[1] = 0.0f;
    colorBlendingState.blendConstants[2] = 0.0f;
    colorBlendingState.blendConstants[3] = 0.0f;
    
    pipelineCreateInfo.pColorBlendState = &colorBlendingState;

    // Pipeline Layout
    if (debug) std::cout << "Create Pipeline layout \n";
    vk::PipelineLayout layout = make_pipeline_layout(specification.device, debug);
    pipelineCreateInfo.layout = layout;

    // Render Pass
    if (debug) std::cout << "Create Renderpass \n";
    vk::RenderPass renderpass = make_render_pass(specification.device, specification.swapchainFormat, debug);
    pipelineCreateInfo.renderPass = renderpass;


    // Extra Stuff
    pipelineCreateInfo.basePipelineHandle = nullptr;

    // Make Pipeline
    if (debug) std::cout << "Create Pipeline \n";
    vk::Pipeline graphicsPipeline;
    try
    {
        graphicsPipeline = (specification.device.createGraphicsPipeline(nullptr, pipelineCreateInfo)).value;
    }
    catch (vk::SystemError err)
    {
        if (debug) std::cout << "Failed to create Graphics Pipeline \n";
    }

    GraphicsPiplineOutBundle output = {};

    output.layout = layout;
    output.renderpass = renderpass;
    output.pipeline = graphicsPipeline;


    specification.device.destroyShaderModule(vertexShader);
    specification.device.destroyShaderModule(fragShader);
    return output;
}
