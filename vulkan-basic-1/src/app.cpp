#define _CRT_SECURE_NO_WARNINGS
#include "app.h"


App::App(int width, int height, bool debugMode)
{
	build_glfw_window(width, height, debugMode);

	graphicsEngine = new Engine(width, height, window, debugMode);

}



void App::build_glfw_window(int width, int height, bool debugMode)
{
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

void App::calculate_frame_rate()
{
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;

	if (delta >= 1) {
		int framerate{ std::max(1, int(numFrames / delta)) };
		std::stringstream title;
		title << "Running at " << framerate << " fps.";
		glfwSetWindowTitle(window, title.str().c_str());
		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate);
	}

	++numFrames;
}

void App::run()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		graphicsEngine->render();
		calculate_frame_rate();
	}
}

App::~App()
{
	delete graphicsEngine;
}
