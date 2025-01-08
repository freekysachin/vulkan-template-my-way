#pragma once
#include<config.h>
#include<set>

namespace vkInit {

	void log_device_properties(const vk::PhysicalDevice& device);
	bool isSuitable(const vk::PhysicalDevice& device, bool debug);
	bool checkDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions,
		const bool& debug
	);
	vk::PhysicalDevice choose_physical_device(vk::Instance &instance, bool debug);
}