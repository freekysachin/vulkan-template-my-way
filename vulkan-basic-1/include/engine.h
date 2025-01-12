#pragma once
#include<config.h>
#include <GLFW/glfw3.h>
#include <instance.h>
#include<Logging.h>

class Engine {
public:
	Engine();

	~Engine();
private:

	bool debugMode = true;


	int width{ 640 };
	int height{ 480 };
	GLFWwindow* window{ nullptr };

	//vulkan instance related varaiables
	vk::Instance instance{ nullptr };
	vk::DispatchLoaderDynamic dldi; // dynamic instance dispatcher
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::SurfaceKHR surface{ nullptr };

	//device related variables
	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device logicalDevice{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain{ nullptr };
	std::vector<vk::Image> swapchainImages;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	void build_glfw_window();

	// instance creation
	void make_instance();
	void make_debug_messenger();
	void create_surface();

	// device setup
	void make_devie();
	
	//Swapchain
	void swapchainCreation();
};
