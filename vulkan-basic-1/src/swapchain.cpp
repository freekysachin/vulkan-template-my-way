#define _CRT_SECURE_NO_WARNINGS
#include<swapchain.h>
#include<Logging.h>
#include<queue_families.h>


vkInit::SwapChainSupportDetails vkInit::query_swapchain_support(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool debug)
{
	SwapChainSupportDetails support;
	/*
		* typedef struct VkSurfaceCapabilitiesKHR {
			uint32_t                         minImageCount;
			uint32_t                         maxImageCount;
			VkExtent2D                       currentExtent;
			VkExtent2D                       minImageExtent;
			VkExtent2D                       maxImageExtent;
			uint32_t                         maxImageArrayLayers;
			VkSurfaceTransformFlagsKHR       supportedTransforms;
			VkSurfaceTransformFlagBitsKHR    currentTransform;
			VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
			VkImageUsageFlags                supportedUsageFlags;
		} VkSurfaceCapabilitiesKHR;
	*/

	support.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

	if (debug) {
		std::cout << "Swapchain can support the following surface capabilities:\n";

		std::cout << "\tminimum image count: " << support.capabilities.minImageCount << '\n';
		std::cout << "\tmaximum image count: " << support.capabilities.maxImageCount << '\n';

		std::cout << "\tcurrent extent: \n";
		/*typedef struct VkExtent2D {
			uint32_t    width;
			uint32_t    height;
		} VkExtent2D;
		*/
		std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << '\n';
		std::cout << "\t\theight: " << support.capabilities.currentExtent.height << '\n';

		std::cout << "\tminimum supported extent: \n";
		std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << '\n';
		std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << '\n';

		std::cout << "\tmaximum supported extent: \n";
		std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << '\n';
		std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << '\n';

		std::cout << "\tmaximum image array layers: " << support.capabilities.maxImageArrayLayers << '\n';


		std::cout << "\tsupported transforms:\n";
		std::vector<std::string> stringList = vkInit::log_transform_bits(support.capabilities.supportedTransforms);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}

		std::cout << "\tcurrent transform:\n";
		stringList = log_transform_bits(support.capabilities.currentTransform);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}

		std::cout << "\tsupported alpha operations:\n";
		stringList = log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}

		std::cout << "\tsupported image usage:\n";
		stringList = log_image_usage_bits(support.capabilities.supportedUsageFlags);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}
	}

	support.formats = physicalDevice.getSurfaceFormatsKHR(surface);

	if (debug) {

		for (vk::SurfaceFormatKHR supportedFormat : support.formats) {
			/*
			* typedef struct VkSurfaceFormatKHR {
				VkFormat           format;
				VkColorSpaceKHR    colorSpace;
			} VkSurfaceFormatKHR;
			*/

			std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
			std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
		}
	}

	support.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	for (vk::PresentModeKHR presentMode : support.presentModes) {
		std::cout << '\t' << log_present_mode(presentMode) << '\n';
	}
	return support;

}

vk::SurfaceFormatKHR vkInit::choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats)
{
	for (auto format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Unorm
			&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return format;
		}
	}
	return formats[0];
}

vk::PresentModeKHR vkInit::choose_swaphain_present_mode(std::vector<vk::PresentModeKHR> presentModes)
{
	/*for (vk::PresentModeKHR presentMode : presentModes) {
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			return presentMode;
		}
	}*/
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D vkInit::choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilites)
{
	if (capabilites.currentExtent.width != UINT32_MAX) {
		return capabilites.currentExtent;
	}
	else {
		vk::Extent2D extent = { width , height };
		extent.width = std::min(
			capabilites.maxImageExtent.width,
			std::max(capabilites.minImageExtent.width, width)
		);

		extent.height = std::min(
			capabilites.maxImageExtent.height,
			std::max(capabilites.minImageExtent.height, height)
		);

		return extent;
	}
}

vkInit::SwapchainBundle vkInit::create_swapchain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, int width, int height, bool debug)
{
	SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface, debug);
	vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);
	vk::PresentModeKHR presentMode = choose_swaphain_present_mode(support.presentModes);
	vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

	uint32_t imageCount = std::min(
		support.capabilities.maxImageCount,
		support.capabilities.minImageCount + 1
	);

	/*
		* VULKAN_HPP_CONSTEXPR SwapchainCreateInfoKHR(
	  VULKAN_HPP_NAMESPACE::SwapchainCreateFlagsKHR flags_         = {},
	  VULKAN_HPP_NAMESPACE::SurfaceKHR              surface_       = {},
	  uint32_t                                      minImageCount_ = {},
	  VULKAN_HPP_NAMESPACE::Format                  imageFormat_   = VULKAN_HPP_NAMESPACE::Format::eUndefined,
	  VULKAN_HPP_NAMESPACE::ColorSpaceKHR   imageColorSpace_  = VULKAN_HPP_NAMESPACE::ColorSpaceKHR::eSrgbNonlinear,
	  VULKAN_HPP_NAMESPACE::Extent2D        imageExtent_      = {},
	  uint32_t                              imageArrayLayers_ = {},
	  VULKAN_HPP_NAMESPACE::ImageUsageFlags imageUsage_       = {},
	  VULKAN_HPP_NAMESPACE::SharingMode     imageSharingMode_ = VULKAN_HPP_NAMESPACE::SharingMode::eExclusive,
	  uint32_t                              queueFamilyIndexCount_ = {},
	  const uint32_t *                      pQueueFamilyIndices_   = {},
	  VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR preTransform_ =
		VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR::eIdentity,
	  VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR compositeAlpha_ =
		VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR::eOpaque,
	  VULKAN_HPP_NAMESPACE::PresentModeKHR presentMode_  = VULKAN_HPP_NAMESPACE::PresentModeKHR::eImmediate,
	  VULKAN_HPP_NAMESPACE::Bool32         clipped_      = {},
	  VULKAN_HPP_NAMESPACE::SwapchainKHR   oldSwapchain_ = {} ) VULKAN_HPP_NOEXCEPT
	*/

	vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(),
		surface,
		imageCount,
		format.format,
		format.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment
	);

	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, false);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value() , indices.presentFamily.value() };

	if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	swapchainCreateInfo.preTransform = support.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;

	swapchainCreateInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

	SwapchainBundle bundle{};
	try {
		bundle.swapchain = logicalDevice.createSwapchainKHR(swapchainCreateInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create swap chain!");
	}

	std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
	bundle.frames.resize(images.size());

	for (size_t i = 0; i < images.size(); i++) {
		/*
			* ImageViewCreateInfo( VULKAN_HPP_NAMESPACE::ImageViewCreateFlags flags_ = {},
						   VULKAN_HPP_NAMESPACE::Image                image_ = {},
						   VULKAN_HPP_NAMESPACE::ImageViewType    viewType_  = VULKAN_HPP_NAMESPACE::ImageViewType::e1D,
						   VULKAN_HPP_NAMESPACE::Format           format_    = VULKAN_HPP_NAMESPACE::Format::eUndefined,
						   VULKAN_HPP_NAMESPACE::ComponentMapping components_            = {},
						   VULKAN_HPP_NAMESPACE::ImageSubresourceRange subresourceRange_ = {} ) VULKAN_HPP_NOEXCEPT
				: flags( flags_ )
				, image( image_ )
				, viewType( viewType_ )
				, format( format_ )
				, components( components_ )
				, subresourceRange( subresourceRange_ )
		*/
		vk::ImageViewCreateInfo createInfo = {};
		createInfo.image = images[i];
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = format.format;
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		bundle.frames[i].image = images[i];
		bundle.frames[i].imageView = logicalDevice.createImageView(createInfo);
	}
	bundle.format = format.format;
	bundle.extent = extent;

	return bundle;
}