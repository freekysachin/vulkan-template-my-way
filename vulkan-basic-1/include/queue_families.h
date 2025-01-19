#pragma once
#include<config.h>
#include<optional>

namespace vkUtil {
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


	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, vk::SurfaceKHR surface, bool debug);
}