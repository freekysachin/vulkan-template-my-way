#pragma once

#include<config.h>
#include<engine.h>

class App {

private:
	Engine* graphicsEngine;
	GLFWwindow* window;

	double lastTime, currentTime;
	int numFrames;
	float frameTime;

	void build_glfw_window(int width, int height, bool debugMode);

	void calculate_frame_rate();

public:
	App(int width, int height, bool debugMode);
	~App();
	void run();
};