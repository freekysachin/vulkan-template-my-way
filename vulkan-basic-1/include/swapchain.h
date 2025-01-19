#pragma once
#include<config.h>
#include<frame.h>


namespace vkInit {
	// -- SwapChain Code Logic --
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct SwapchainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<vkUtil::SwapchainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};



	SwapChainSupportDetails query_swapchain_support(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool debug);
	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> format);
	vk::PresentModeKHR choose_swaphain_present_mode(std::vector<vk::PresentModeKHR> presentModes);
	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilites);

	SwapchainBundle create_swapchain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, int width, int height, bool debug);
}