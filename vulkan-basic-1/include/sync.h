#pragma once
#include <config.h>

namespace vkInit {

	vk::Semaphore make_semaphroe(vk::Device device , bool debug);
	vk::Fence make_fence(vk::Device device, bool debug);

}