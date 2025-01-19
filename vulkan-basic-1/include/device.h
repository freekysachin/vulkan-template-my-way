#pragma once
#include<config.h>
#include<set>
#include<Logging.h>

namespace vkInit {

	// -- Physical Device --
	void log_device_properties(const vk::PhysicalDevice& device);
	bool isSuitable(const vk::PhysicalDevice& device, bool debug);
	bool checkDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions,
		const bool& debug
	);
	vk::PhysicalDevice choose_physical_device(vk::Instance &instance, bool debug);


	// -- logical device --
	vk::Device create_logical_device(const vk::PhysicalDevice& device,vk::SurfaceKHR surface , bool debug);
	std::vector<vk::Queue> get_queue(const vk::PhysicalDevice& physicalDevice,const vk::Device& logicalDevice, vk::SurfaceKHR surface, bool debug);
	
}