#define _CRT_SECURE_NO_WARNINGS
#include<queue_families.h>

vkUtil::QueueFamilyIndices vkUtil::findQueueFamilies(const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR surface, bool debug)
{
	vkUtil::QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

	if (debug) {
		std::cout << "There are " << queueFamilies.size() << " queue families available on the system.\n";
	}

	int i = 0;
	for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {

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