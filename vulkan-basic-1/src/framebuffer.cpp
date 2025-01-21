#define _CRT_SECURE_NO_WARNINGS
#include "framebuffer.h"

void vkInit::make_framebuffers(framebufferInput inputChunk, std::vector<vkUtil::SwapchainFrame>& frames, bool debug)
{
	for (auto i = 0; i < frames.size(); i++) {
		
		std::vector<vk::ImageView> attachments = {
			frames[i].imageView
		};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.flags = vk::FramebufferCreateFlags();
		framebufferInfo.renderPass = inputChunk.renderpass;
		framebufferInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = inputChunk.swapchainExtent.width;
		framebufferInfo.height = inputChunk.swapchainExtent.height;
		framebufferInfo.layers = 1;

		try
		{
			frames[i].framebuffer = inputChunk.device.createFramebuffer(framebufferInfo);
			if (debug) std::cout << "Created framebuffer for the frame " << i << std::endl;
		}
		catch (vk::SystemError err)
		{
			if(debug) std::cout<<"Failed to create the frame buffer " << i << std::endl;
		}

	}
}
