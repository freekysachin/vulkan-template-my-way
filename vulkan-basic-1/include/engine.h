#pragma once
#include<config.h>
#include <GLFW/glfw3.h>
#include <instance.h>
#include<Logging.h>
#include<frame.h>

class Engine {
public:
	Engine(int width, int height, GLFWwindow* window, bool debugMode);
	~Engine();

	void render();

private:

	bool debugMode;


	int width;
	int height;
	GLFWwindow* window;

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
	std::vector<vkUtil::SwapchainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	// pipeline related variables
	vk::PipelineLayout layout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;

	// command related variables
	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandbuffer;

	// synchronization related variables
	vk::Fence inFlightFence;
	vk::Semaphore imageAvailable, renderFinished;



	// instance creation
	void make_instance();
	void make_debug_messenger();
	void create_surface();

	// device setup
	void make_devie();
	
	//Swapchain
	void swapchainCreation();

	// pipeline setup
	void make_pipeline();

	// The Last Step
	void final_setup();

	void record_draw_commands(vk::CommandBuffer commandbuffer, uint32_t imageIndex);
};
