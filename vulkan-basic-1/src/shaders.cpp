#define _CRT_SECURE_NO_WARNINGS
#include "shaders.h"

std::vector<char> vkUtil::readFile(std::string fileName, bool debug)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (debug && !file.is_open()) {
        std::cout << "Failed to load \"" << fileName << "\"" << std::endl;
    }

    size_t fileSize{ static_cast<size_t>(file.tellg()) };

    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

vk::ShaderModule vkUtil::createModule(std::string fileName, vk::Device logicalDevice, bool debug)
{

    std::vector<char> sourceCode = readFile(fileName, debug);

    vk::ShaderModuleCreateInfo createInfo = {};
    
    createInfo.flags = vk::ShaderModuleCreateFlags();
    createInfo.codeSize = sourceCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());

    try
    {
        return logicalDevice.createShaderModule(createInfo);
    }
    catch (vk::SystemError err)
    {
        if (debug) std::cout << "Failed to create shader module for \"" << fileName << "\"" << std::endl;
    }
    
}
