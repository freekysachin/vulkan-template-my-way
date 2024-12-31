#define _CRT_SECURE_NO_WARNINGS
#include<engine.h>

Engine::Engine() {
	if (debugMode) std::cout << "Making a graphics Engine \n";

	build_glfw_window();
	make_instance();
	make_debug_messenger();
}

void Engine::build_glfw_window() {
	
	// initialize glfw
	glfwInit();

	// no default rendering client , we weill hook vulkan up to the window later
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //sets the specific wndw hint to the desired value

	//resizing breaks the swapchain, we'll disable it for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (window = glfwCreateWindow(width, height, "title", nullptr, nullptr)) {
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

	//
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
}

void Engine::make_debug_messenger()
{
	debugMessenger = vkInit::make_debug_messenger(instance, dldi);
}

Engine::~Engine() {

	if (debugMode) std::cout << "Engine Distroyed! \n";

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