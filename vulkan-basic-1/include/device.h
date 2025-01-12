#pragma once
#include<config.h>
#include<set>
#include<optional>
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


	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, vk::SurfaceKHR surface, bool debug);
	vk::Device create_logical_device(const vk::PhysicalDevice& device,vk::SurfaceKHR surface , bool debug);
	std::vector<vk::Queue> get_queue(const vk::PhysicalDevice& physicalDevice,const vk::Device& logicalDevice, vk::SurfaceKHR surface, bool debug);


	// -- SwapChain Code Logic --
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct SwapchainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format format;
		vk::Extent2D extent;
	};

	SwapChainSupportDetails query_swapchain_support(const vk::PhysicalDevice& physicalDevice,const vk::SurfaceKHR& surface, bool debug);
	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> format);
	vk::PresentModeKHR choose_swaphain_present_mode(std::vector<vk::PresentModeKHR> presentModes);
	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilites);

	SwapchainBundle create_swapchain(const vk::Device &logicalDevice ,const vk::PhysicalDevice &physicalDevice,const vk::SurfaceKHR &surface, int width, int height, bool debug);
}