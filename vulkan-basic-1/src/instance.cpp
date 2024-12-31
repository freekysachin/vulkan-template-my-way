#define _CRT_SECURE_NO_WARNINGS
#include<instance.h>

vk::Instance vkInit::make_instance(bool debug, const char* appName)
{
	if (debug) std::cout << "Making instance \n";

	/*
		An Instance	stores all the pre-application state info, it is a vulkan handle
		(An opaque integer or pointer value to refer to a vulkan object)

		from vulkan_handles.hpp
		class Instance {
		...
		}
	*/

	uint32_t version{ 0 };

	//scans the computer and put the "vulkan version" in version variable
	vkEnumerateInstanceVersion(&version);
	if (debug) {
		std::cout << "System can support vulkan variant: " << VK_API_VERSION_VARIANT(version)
			<< ", Major: " << VK_API_VERSION_MAJOR(version)
			<< ", Minor: " << VK_API_VERSION_MINOR(version)
			<< ", Patch: " << VK_API_VERSION_PATCH(version) << "\n";
	}

	// we can use this version, should be sure to set the patch to 0 for best compatibility 
	version &= ~(0xFFFU);


	//or we can drop to the	earlier version to ensure compatibility with more devices
	// VK_MAKE_API_VERSION(variant , major , minor , patch)
	version = VK_MAKE_API_VERSION(0, 1, 0, 0);


	/*
	* from vulkan_struct.hpp
	* 
	* VULKAN_HPP_CONSTEXPR ApplicationInfor( const char* pAppName_ ={},
	*									uint32_t appVersion_ {},
	*									const char* pEngineName_ ={},
	*									uint32_t engineVersion_ ={},
	*									uint32_t apiversion_ = {}
	*/
	// is a constructor that takes many arguments 
	vk::ApplicationInfo appInfo = vk::ApplicationInfo(  
		appName,
		version,
		"Doing it my way",
		version,
		version
	);

	// next step is we need extensions 
	/*
	* Extensions in Vulkan are optional features or functionalities
	* that are not part of the core Vulkan specification. 
	* They allow applications to access additional features provided by the Vulkan implementation (e.g., a GPU driver),
	* that may not be universally supported across all Vulkan-capable devices.
	* for example
	* Extensions such as VK_EXT_debug_utils enable Vulkan's debug layers, 
	* which help developers debug and validate Vulkan API usage.
	* Platform-Specific Surface Extensions:
	*  - VK_KHR_win32_surface (Windows)
	*  - VK_KHR_xlib_surface or VK_KHR_wayland_surface (Linux
	*  - VK_EXT_metal_surface (macOS)
	*/

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	// GLFWAPI const char** glfwGetRequiredInstanceExtensions(uint32_t* count);
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	/* 
	   This converts the array of const char* returned by glfwGetRequiredInstanceExtensions
	   into a more modern and safer std::vector.
	*/
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (debug) extensions.push_back("VK_EXT_debug_utils");

	if (debug)
	{
		std::cout << "extensions to be required for GLFW \n";
		for (const char* extensionName : extensions) {
			std::cout << "\t\"" << extensionName << "\"\n";
		}
	}

	std::vector<const char*> layers;
	if (debug) {
		layers.push_back("VK_LAYER_KHRONOS_validation");
	}


	if (!supported(extensions, layers, debug)) {
		return nullptr;
	}

	/*
	* from vulkan_structs.hpp:
	* 
	* InstanceCreateInfo( VULKAN_HPP_NAMESPACE::InstanceCreateFlags flags_ = {},
	*						const VULKAN_HPP_NAMESPACE::ApplicationInfo* appInfo ={},
	*						uint32_t									 enabledLayerCount_ = {},
	*						const char* const *							 ppEnabledLayersNames_ ={},
	*						uint32_t									 enabledExtensionCount_ ={},
	*						const char* const *							 ppEnabledExtensionNames_ ={} )
	*/

	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		static_cast<uint32_t>(layers.size()),layers.data(), // enabled layers
		static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
	);

	try
	{
		/*
			* from vulkan_funcs.h:
			*
			* createInstance( const VULKAN_HPP_NAMESPACE::InstanceCreateInfo &          createInfo,
					Optional<const VULKAN_HPP_NAMESPACE::AllocationCallbacks> allocator = nullptr,
					Dispatch const &                                          d = ::vk::getDispatchLoaderStatic())

			*/
		return vk::createInstance(createInfo);
	}
	catch (vk::SystemError err)
	{
		if (debug) {
			std::cout << "Failed to create Instance!\n";
		}
		return nullptr;
	}	
}

bool vkInit::supported(std::vector<const char*>& extensions, std::vector<const char*>& layers, bool debug)
{
	// check extension support
	std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();
	if (debug) {
		std::cout << "Devie can support the following extensions says vulkan: \n";
		for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
			std::cout << '\t' << supportedExtension.extensionName << "\n";
		}
	}
	bool found;
	for (const char* extension : extensions) {
		found = false;
		for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
			if (strcmp(extension, supportedExtension.extensionName) == 0) {
				found = true;
				if (debug) {
					std::cout << "Extension \"" << extension << "\" is supported! \n";
				}
			}
		}
		if (!found) {
			if(debug) std::cout<< "Extension \"" << extension << "\" is not supported! \n";
			return false;
		}
	}

	// check Layer support
	std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();
	if (debug) {
		std::cout << "Device can support the following layers \n";
		for (vk::LayerProperties supportedLayer : supportedLayers) {
			std::cout<< '\t' << supportedLayer.layerName << '\n';
		}
	}
	for (const char* layer : layers) {
		found = false;
		for (vk::LayerProperties supportedLayer : supportedLayers) {
			if (strcmp(layer, supportedLayer.layerName) == 0) {
				if(debug) std::cout<< "Layer \"" << layer << "\" is supported!\n";
				found = true;
			}
		}
		if (!found) {
			if (debug) {
				std::cout << "Layer \"" << layer << "\" is not supported!\n";
			}
			return false;
		}
	}
	return true;
}
