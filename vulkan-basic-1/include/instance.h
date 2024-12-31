#pragma once

#include<config.h>
#include<GLFW/glfw3.h>
#include<vector>
// namespace for creation functions/definitions etc.
namespace vkInit {
	vk::Instance make_instance(bool debug, const char* appName);
	bool supported(std::vector<const char*>& extensions , std::vector<const char*>& layers, bool debug);
}
