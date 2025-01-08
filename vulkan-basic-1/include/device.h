#pragma once
#include<config.h>
#include<set>
#include<optional>

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
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		/**
			\returns whether all of the Queue family indices have been set.
		*/
		inline bool isComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, bool debug);	
	vk::Device create_logical_device(const vk::PhysicalDevice& device, bool debug);
	vk::Queue get_queue(const vk::PhysicalDevice& physicalDevice,const vk::Device& logicalDevice , bool debug);
}