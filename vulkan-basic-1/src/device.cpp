#define _CRT_SECURE_NO_WARNINGS
#include "device.h"
#include<queue_families.h>

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


vk::Device vkInit::create_logical_device(const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR surface, bool debug) {

	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice,surface ,debug);
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
	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice,surface ,debug);
	
	return {
		logicalDevice.getQueue(indices.graphicsFamily.value() , 0),
		logicalDevice.getQueue(indices.presentFamily.value() , 0)
	};
}


