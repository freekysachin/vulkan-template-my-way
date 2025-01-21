#define _CRT_SECURE_NO_WARNINGS
#include<engine.h>
#include <device.h>
#include<swapchain.h>
#include<pipeline.h>
#include<framebuffer.h>
#include<commands.h>
#include<sync.h>

Engine::Engine(int width, int height, GLFWwindow* window, bool debugMode) {

	this->debugMode = debugMode;
	this->width = width;
	this->height = height;
	this->window = window;

	if (debugMode) std::cout << "Making a graphics Engine \n";

	make_instance();
	make_debug_messenger();
	create_surface();
	make_devie();
	swapchainCreation();
	make_pipeline();
	final_setup();
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

void Engine::create_surface()
{
	VkSurfaceKHR c_style_surface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS) {
		if (debugMode) std::cout << "failed to abstract the glfw surface for vulkan \n";

	}
	else if (debugMode) std::cout << "Successfully abstracted glfw surface \n";
	surface = c_style_surface;
}

void Engine::make_devie()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
 	logicalDevice =	 vkInit::create_logical_device(physicalDevice, surface,debugMode);
	std::vector<vk::Queue> queues = vkInit::get_queue(physicalDevice, logicalDevice, surface,debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
}

void Engine::swapchainCreation()
{
	// query and log the swapchain support 
	//vkInit::query_swapchain_support(physicalDevice, surface, debugMode);
	vkInit::SwapchainBundle bundle = vkInit::create_swapchain(logicalDevice, physicalDevice, surface, width, height, debugMode);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;

}

void Engine::make_pipeline()
{
	vkInit::GraphicsPipelineInBundle specification = {};
	specification.device = logicalDevice;
	specification.vertexFilePath = "shaders/vertex.spv";
	specification.fragFilePath = "shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainFormat = swapchainFormat;

	vkInit::GraphicsPiplineOutBundle output = vkInit::make_graphics_pipeline(specification, debugMode);
	layout = output.layout;
	renderpass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::final_setup() {

	vkInit::framebufferInput framebufferInput;
	framebufferInput.device = logicalDevice;
	framebufferInput.renderpass = renderpass;
	framebufferInput.swapchainExtent = swapchainExtent;

	vkInit::make_framebuffers(framebufferInput, swapchainFrames, debugMode);

	commandPool = vkInit::make_command_pool(logicalDevice, physicalDevice, surface, debugMode);
	vkInit::commandbufferInputChunk commandbufferInput = {logicalDevice , commandPool , swapchainFrames};

	mainCommandbuffer = vkInit::make_command_buffers(commandbufferInput, debugMode);

	inFlightFence = vkInit::make_fence(logicalDevice, debugMode);
	imageAvailable = vkInit::make_semaphroe(logicalDevice, debugMode);
	renderFinished = vkInit::make_semaphroe(logicalDevice, debugMode);

}

void Engine::record_draw_commands(vk::CommandBuffer commandbuffer, uint32_t imageIndex)
{
	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		commandbuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) {
		if (debugMode) std::cout << "Failed to begin recording command buffer \n";
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderpass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].framebuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue clearColor = { std::array<float,4>{1.0f, 0.5f, 0.25f, 1.0f} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	commandbuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	commandbuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	commandbuffer.draw(3, 1, 0, 0);

	commandbuffer.endRenderPass();

	try
	{
		commandbuffer.end();
	}
	catch (vk::SystemError err)
	{
		if (debugMode)std::cout << "Failed to finish recording command buffer\n";
	}

}

void Engine::render()
{
 	logicalDevice.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);
	logicalDevice.resetFences(1, &inFlightFence);

	uint32_t imageIndex{ logicalDevice.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailable, nullptr).value };

	vk::CommandBuffer commandbuffer = swapchainFrames[imageIndex].commandbuffer;

	commandbuffer.reset();

	record_draw_commands(commandbuffer, imageIndex);

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = { imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandbuffer;

	vk::Semaphore signalSemaphores[] = { renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	try
	{
		graphicsQueue.submit(submitInfo, inFlightFence);
	}
	catch (vk::SystemError err)
	{
		if (debugMode) std::cout << "Failed to submit draw command buffer \n";
	}

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapchains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);

}

Engine::~Engine() {

	logicalDevice.waitIdle();
	if (debugMode) std::cout << "Engine Distroyed! \n";


	logicalDevice.destroyFence(inFlightFence);
	logicalDevice.destroySemaphore(imageAvailable);
	logicalDevice.destroySemaphore(renderFinished);


	logicalDevice.destroyCommandPool(commandPool);
	logicalDevice.destroyPipeline(pipeline);
	logicalDevice.destroyPipelineLayout(layout);
	logicalDevice.destroyRenderPass(renderpass);

	for (const vkUtil::SwapchainFrame &frame : swapchainFrames) {
		logicalDevice.destroyImageView(frame.imageView);
		logicalDevice.destroyFramebuffer(frame.framebuffer);
	}
	logicalDevice.destroySwapchainKHR(swapchain);
	logicalDevice.destroy();
	instance.destroySurfaceKHR(surface);

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


