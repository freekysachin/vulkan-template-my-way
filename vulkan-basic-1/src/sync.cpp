#define _CRT_SECURE_NO_WARNINGS
#include "sync.h"

vk::Semaphore vkInit::make_semaphroe(vk::Device device, bool debug)
{
	vk::SemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.flags = vk::SemaphoreCreateFlags();
	try
	{
		return device.createSemaphore(semaphoreInfo);
	}
	catch (vk::SystemError err)
	{
		if(debug) std::cout<< "vk::Semaphore creation failed" << std::endl;
		return nullptr;
	}
}

vk::Fence vkInit::make_fence(vk::Device device, bool debug)
{
	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

	try
	{
		device.createFence(fenceInfo);
	}
	catch (vk::SystemError err)
	{
		if(debug) std::cout << "vk::Fence creation failed " << std::endl;
		return nullptr;
	}
}
