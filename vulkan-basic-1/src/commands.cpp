#define _CRT_SECURE_NO_WARNINGS
#include "commands.h"
#include<queue_families.h>

vk::CommandPool vkInit::make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug)
{

	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

	try
	{
		return device.createCommandPool(poolInfo);
	}
	catch (vk::SystemError err)
	{
		std::cout << "Command pool creation failed\n";
		return nullptr;
	}

}

vk::CommandBuffer vkInit::make_command_buffers(commandbufferInputChunk inputChunk, bool debug)
{
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = inputChunk.commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	for (int i = 0; i < inputChunk.frames.size(); ++i) {
		try {
			inputChunk.frames[i].commandbuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];
			if (debug) std::cout << "Allocated command buffer for frame " << i << std::endl;
		}
		catch (vk::SystemError err) {
			if (debug) std::cout << "commandbuffer allocation failed \n";
		}
	}

	try
	{
		vk::CommandBuffer commandbuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];
		if (debug) std::cout << "Allocated main commandbuffer \n";
		return commandbuffer;
	}
	catch (const std::exception&)
	{
		if (debug) std::cout << " Failed to allocate main commandbuffer \n";
		return nullptr;
	}


}
