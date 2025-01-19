#pragma once
#include<config.h>

namespace vkUtil {
	std::vector<char> readFile(std::string fileName, bool debug);
	vk::ShaderModule createModule(std::string fileName, vk::Device logicalDevice, bool debug);
}