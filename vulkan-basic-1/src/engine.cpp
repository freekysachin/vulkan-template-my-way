#define _CRT_SECURE_NO_WARNINGS
#include<engine.h>
#include <device.h>

Engine::Engine() {
	if (debugMode) std::cout << "Making a graphics Engine \n";

	build_glfw_window();
	make_instance();
	make_debug_messenger();
	make_devie();
}

void Engine::build_glfw_window() {
	
	// initialize glfw
	glfwInit();

	// no default rendering client , we weill hook vulkan up to the window later
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //sets the specific wndw hint to the desired value

	//resizing breaks the swapchain, we'll disable it for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (window = glfwCreateWindow(width, height, "glfwWindow", nullptr, nullptr)) {
		if (debugMode) {
			std::cout << "Successfully made a glfw window \n";
		}
	}
	else {
		if (debugMode) std::cout << "GLFW window creation failed \n";
	}

}


void Engine::make_instance() {
	
	instance = vkInit::make_instance(debugMode, "Engine 0.1.0.0");

	/*
		vk::DispatchLoaderDynamic is used for dynamic function dispatch. 
		Vulkan functions can be loaded dynamically because 
		the actual function implementations are provided by the Vulkan driver and can vary 
		between different hardware and platforms
		after creating a Vulkan instance, you can load functions that operate on that instance, 
		such as creating a debug messenger
	*/

	/*  Vulkan helper class DispatchLoaderDynamic provided by the Vulkan-Hpp 
		namespace VULKAN_HPP_NAMESPACE as vk
		{
			class DispatchLoaderDynamic;
		}  // namespace VULKAN_HPP_NAMESPACE
	*/
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);// is not a function but the object to the class DispatchLoaderDynamic
}

void Engine::make_debug_messenger()
{

	/*
		Note* Before creating an debug_messenger we must tell our instance that we are going to be using the
		debugMessenger functionalities so we do this while creating a vulkan instance and passing this crusial
		info in the vk::InstanceCreateInfo createInfo

		You can check that we have done this in the vkInit::make_instance(debugMode, "Engine 0.1.0.0")
	*/

	/* 
	   to create and add the debug_messenger into the instance we are going to need a
	   vk::DebugUtilsMessengerCreateInfoEXT createInfo so we are going to create the cerateInfo
	   into the make_debug_messenger(instance, dldi) method then add the debugMessenger into the instance and return the reference
	   of it back that debugMessenger variable will catch
	*/
	debugMessenger = vkInit::make_debug_messenger(instance, dldi);
}

void Engine::make_devie()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
 	logicalDevice =	 vkInit::create_logical_device(physicalDevice, debugMode);
	graphicsQueue = vkInit::get_queue(physicalDevice, logicalDevice, debugMode);
}

Engine::~Engine() {

	if (debugMode) std::cout << "Engine Distroyed! \n";

	logicalDevice.destroy();

	/*
	* from vulkan_funcs.hpp:
	*
	* void Instance::destroy( Optional<const VULKAN_HPP_NAMESPACE::AllocationCallbacks> allocator = nullptr,
											Dispatch const & d = ::vk::getDispatchLoaderStatic())
	*/
	instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	instance.destroy();

	//terminate glfw
	glfwTerminate();
}