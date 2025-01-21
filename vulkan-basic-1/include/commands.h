#pragma once
#include<config.h>
#include <frame.h>


namespace vkInit {

	struct commandbufferInputChunk
	{
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapchainFrame>& frames;

	};

	vk::CommandPool make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug);
	vk::CommandBuffer make_command_buffers(commandbufferInputChunk inputChunk, bool debug);
}