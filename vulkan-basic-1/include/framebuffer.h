#pragma once

#include<config.h>
#include<frame.h>

namespace vkInit {

	struct framebufferInput {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	void make_framebuffers(framebufferInput inputChunk, std::vector<vkUtil::SwapchainFrame>& frames, bool debug);
}