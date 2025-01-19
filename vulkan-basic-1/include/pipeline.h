#pragma once
#include<config.h>
#include<shaders.h>

namespace vkInit {

	struct GraphicsPipelineInBundle
	{
		vk::Device device;
		std::string vertexFilePath;
		std::string fragFilePath;
		vk::Extent2D swapchainExtent;
		vk::Format swapchainFormat;
	};
	struct GraphicsPiplineOutBundle
	{
		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Pipeline pipeline;
	};

	vk::PipelineLayout make_pipeline_layout(vk::Device device, bool debug);
	vk::RenderPass make_render_pass(vk::Device device, vk::Format swapchainFormat , bool debug);

	GraphicsPiplineOutBundle make_graphics_pipeline(GraphicsPipelineInBundle specification, bool debug);
}