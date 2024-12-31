#pragma once
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

	//vulkan instance
	vk::Instance instance{ nullptr };

	//debug callback
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };

	// dynamic instance dispatcher
	vk::DispatchLoaderDynamic dldi;

	void build_glfw_window();

	// instance creation
	void make_instance();

	void make_debug_messenger();
};
