#define _CRT_SECURE_NO_WARNINGS
#include "device.h"

/*
* Vulkan separates the concept of physical and logical devices.
*
  A physical device usually represents a single complete implementation of Vulkan
  (excluding instance-level functionality) available to the host,
  of which there are a finite number.

  A logical device represents an instance of that implementation
  with its own state and resources independent of other logical devices.
*/


void vkInit::log_device_properties(const vk::PhysicalDevice& device)
{
	vk::PhysicalDeviceProperties properties = device.getProperties();
	/*
	* typedef struct VkPhysicalDeviceProperties {
		uint32_t                            apiVersion;
		uint32_t                            driverVersion;
		uint32_t                            vendorID;
		uint32_t                            deviceID;
		VkPhysicalDeviceType                deviceType;
		char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
		uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
		VkPhysicalDeviceLimits              limits;
		VkPhysicalDeviceSparseProperties    sparseProperties;
		} VkPhysicalDeviceProperties;
	*/

	std::cout << "Device Name: " << properties.deviceName << "\n";
	std::cout << "Device type: ";
	switch (properties.deviceType) {

	case (vk::PhysicalDeviceType::eCpu):
		std::cout << "CPU\n";
		break;

	case (vk::PhysicalDeviceType::eDiscreteGpu):
		std::cout << "Discrete GPU\n";
		break;

	case (vk::PhysicalDeviceType::eIntegratedGpu):
		std::cout << "Integrated GPU\n";
		break;

	case (vk::PhysicalDeviceType::eVirtualGpu):
		std::cout << "Virtual GPU\n";
		break;

	default:
		std::cout << "Other\n";
	}

}

/**
		Check whether the physical device can support the given extensions.

		\param device the physical device to check
		\param requestedExtensions a list of extension names to check against
		\param debug whether the system is running in debug mode
		\returns whether all of the extensions are requested
	*/


bool vkInit::isSuitable(const vk::PhysicalDevice& device, bool debug)
{
	if (debug) std::cout << "checking if device is suitable \n";

	/*
	* A device is suitable if it can present to the screen, ie support
	* the swapchain extension
	*/

	const std::vector<const char*> requestedExtension = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	if (debug) {
		std::cout << "We are requesting device Extension... \n";
		for (const char* extension : requestedExtension) {
			std::cout << "\t\"" << extension << "\"\n";
		}
	}

	if (bool extensionSupported = checkDeviceExtensionSupport(device, requestedExtension, debug)) {
		if (debug) std::cout << "device can support the requested extensions\n";
	}
	else {
		if (debug) std::cout << "Device can not support the requested extension \n";
		return false;
	}

	return true;
}

bool vkInit::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool& debug)
{
	/*
	* Check if a given physical device can satisfy a list of requested device
	* extensions.
	*/

	std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());
	if (debug) {
		std::cout << "Device can support extensions:\n";
	}

	for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {

		if (debug) {
			std::cout << "\t\"" << extension.extensionName << "\"\n";
		}

		//remove this from the list of required extensions (set checks for equality automatically)
		requiredExtensions.erase(extension.extensionName);
	}
	//if the set is empty then all requirements have been satisfied
	return requiredExtensions.empty();

}

vk::PhysicalDevice vkInit::choose_physical_device(vk::Instance &instance, bool debug)
{
	/*
		* Choose a suitable physical device from a list of candidates.
		* Note: Physical devices are neither created nor destroyed, they exist
		* independently to the program.
	*/

	if (debug) std::cout << "Choosing physical device... \n";

	/*
	* ResultValueType<std::vector<PhysicalDevice, PhysicalDeviceAllocator>>::type
		Instance::enumeratePhysicalDevices( Dispatch const & d )

	  std::vector<vk::PhysicalDevice> instance.enumeratePhysicalDevices( Dispatch const & d = static/default )
	*/
	std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();
	if (debug) std::cout << "There are " << availableDevices.size() << " physical devices available on the system \n";

	for (auto device : availableDevices) {

		if(debug) log_device_properties(device);
		if (isSuitable(device, debug)) return device;
	}
	

	return nullptr;
}

// logical device code starts from here

vkInit::QueueFamilyIndices vkInit::findQueueFamilies(const vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR surface, bool debug)
{
	vkInit::QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

	if (debug) {
		std::cout << "There are " << queueFamilies.size() << " queue families available on the system.\n";
	}

	int i = 0;
	for (const vk::QueueFamilyProperties &queueFamily : queueFamilies) {

		/*
		* // Provided by VK_VERSION_1_0
			typedef struct VkQueueFamilyProperties {
			VkQueueFlags    queueFlags;
			uint32_t        queueCount;
			uint32_t        timestampValidBits;
			VkExtent3D      minImageTransferGranularity;
			} VkQueueFamilyProperties;

			queueFlags is a bitmask of VkQueueFlagBits indicating capabilities of the queues in this queue family.

			queueCount is the unsigned integer count of queues in this queue family. Each queue family must support
			at least one queue.

			timestampValidBits is the unsigned integer count of meaningful bits in the timestamps written via
			vkCmdWriteTimestamp. The valid range for the count is 36..64 bits, or a value of 0,
			indicating no support for timestamps. Bits outside the valid range are guaranteed to be zeros.

			minImageTransferGranularity is the minimum granularity supported for image transfer
			operations on the queues in this queue family.
		*/

		/*
		* // Provided by VK_VERSION_1_0
			typedef enum VkQueueFlagBits {
			VK_QUEUE_GRAPHICS_BIT = 0x00000001,
			VK_QUEUE_COMPUTE_BIT = 0x00000002,
			VK_QUEUE_TRANSFER_BIT = 0x00000004,
			VK_QUEUE_SPARSE_BINDING_BIT = 0x00000008,
			} VkQueueFlagBits;
		*/
		/*std::cout << "Queue Family " << i << ":\n";
		std::cout << "  Queue Count: " << queueFamily.queueCount << "\n";
		std::cout << "  Timestamp Valid Bits: " << queueFamily.timestampValidBits << "\n";
		std::cout << "  Min Image Transfer Granularity: "
			<< queueFamily.minImageTransferGranularity.width << "x"
			<< queueFamily.minImageTransferGranularity.height << "x"
			<< queueFamily.minImageTransferGranularity.depth << "\n";

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			std::cout << "    VK_QUEUE_GRAPHICS_BIT is set\n";
		}
		if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
			std::cout << "    VK_QUEUE_COMPUTE_BIT is set\n";
		}
		if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) {
			std::cout << "    VK_QUEUE_TRANSFER_BIT is set\n";
		}
		if (queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding) {
			std::cout << "    VK_QUEUE_SPARSE_BINDING_BIT is set\n";
		}*/

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
			indices.presentFamily = i;

			if (debug) {
				std::cout << "Queue Family " << i << " is suitable for graphics and presenting \n";
			}
		}

		if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
			indices.presentFamily = i;
			if (debug)std::cout << "Queue Family " << i << " is suitable for presenting\n";
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}
	return indices;
}

vk::Device vkInit::create_logical_device(const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR surface, bool debug) {

	vkInit::QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface ,debug);
	std::vector<uint32_t> uniqueIndices;
	uniqueIndices.push_back(indices.graphicsFamily.value());
	if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
		uniqueIndices.push_back(indices.presentFamily.value());
	}
	float queuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
	for (auto uniqueQueueFamilyIndex : uniqueIndices) {

		queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(), uniqueQueueFamilyIndex,
			1, &queuePriority
		));

	}
	
	
	vk::PhysicalDeviceFeatures physicalDeviceFeatures = vk::PhysicalDeviceFeatures();
	std::vector<const char*> enabledLayers;
	if (debug) {
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
	}
	std::vector<const char*> enabledExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	vk::DeviceCreateInfo logicalDeviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(),
		(uint32_t)queueCreateInfo.size(), queueCreateInfo.data(),
		(uint32_t)enabledLayers.size(),
		enabledLayers.data(),
		(uint32_t)enabledExtensions.size(), enabledExtensions.data(),
		&physicalDeviceFeatures
	);

	try
	{
		vk::Device logicalDevice = physicalDevice.createDevice(logicalDeviceInfo);
		if (debug) std::cout << "GPU has been succssfully abstracted! \n";	
		return logicalDevice;
	}
	catch (vk::SystemError err)
	{
		if (debug) std::cout << "Device Creation failed \n";
		return nullptr;
	}
	return nullptr;

}

std::vector<vk::Queue> vkInit::get_queue(const vk::PhysicalDevice& physicalDevice, const vk::Device& logicalDevice, vk::SurfaceKHR surface, bool debug)
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface ,debug);
	
	return {
		logicalDevice.getQueue(indices.graphicsFamily.value() , 0),
		logicalDevice.getQueue(indices.presentFamily.value() , 0)
	};
}

vkInit::SwapChainSupportDetails vkInit::query_swapchain_support(const vk::PhysicalDevice& physicalDevice,const vk::SurfaceKHR& surface, bool debug)
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
	SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface , debug);
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

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, false);
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

	bundle.images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
	bundle.format = format.format;
	bundle.extent = extent;

	return bundle;
}
