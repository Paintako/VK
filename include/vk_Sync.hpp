#pragma once

#include <vulkan/vulkan.h>

namespace vk {
class vk_Sync {
public:
	vk_Sync(VkDevice &device);
	~vk_Sync() = default;

	void createSyncObjects();
	void cleanupSyncObjects();

	VkSemaphore &getImageAvailableSemaphore() {
		return imageAvailableSemaphore;
	}
	VkSemaphore &getRenderFinishedSemaphore() {
		return renderFinishedSemaphore;
	}
	VkFence &getInFlightFence() { return inFlightFence; }

private:
	VkDevice &device;

	/*
		Semaphores are used to synchronize the image acquisition and rendering
		Sync in GPU, not CPU
	*/
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	/*
		Fence is used to synchronize the CPU and GPU
		Sync in CPU, not GPU
	*/
	VkFence inFlightFence;
};
}  // namespace vk