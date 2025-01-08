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

vkInit::QueueFamilyIndices vkInit::findQueueFamilies(const vk::PhysicalDevice &PhysicalDevice, bool debug)
{
	vkInit::QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = PhysicalDevice.getQueueFamilyProperties();

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
				std::cout << "Queue Family " << i << " is suitable for graphics and presenting";
			}
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}
	return indices;
}

vk::Device vkInit::create_logical_device(const vk::PhysicalDevice& physicalDevice, bool debug) {

	vkInit::QueueFamilyIndices indices = findQueueFamilies(physicalDevice, debug);
	float queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo(
		vk::DeviceQueueCreateFlags(), indices.graphicsFamily.value(),
		1, &queuePriority
	);
	
	vk::PhysicalDeviceFeatures physicalDeviceFeatures = vk::PhysicalDeviceFeatures();
	std::vector<const char*> enabledLayers;
	if (debug) {
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
	}
	vk::DeviceCreateInfo logicalDeviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(),
		1, &queueCreateInfo,
		(uint32_t)enabledLayers.size(),
		enabledLayers.data(),
		0, nullptr,
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

vk::Queue vkInit::get_queue(const vk::PhysicalDevice& physicalDevice, const vk::Device& logicalDevice, bool debug)
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, debug);

	return logicalDevice.getQueue(indices.graphicsFamily.value(), 0);
}
