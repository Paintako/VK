#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace vk {
class vk_Sync {
public:
	vk_Sync(VkDevice &device, size_t max_frames_in_flight);
	~vk_Sync() = default;

	void createSyncObjects(size_t max_frames_in_flight);
	void cleanupSyncObjects();

	VkSemaphore &getImageAvailableSemaphore() {
		return imageAvailableSemaphore;
	}
	VkSemaphore &getRenderFinishedSemaphore() {
		return renderFinishedSemaphore;
	}
	VkFence &getInFlightFence() { return inFlightFence; }

	VkSemaphore &getImageAvailableSemaphore(size_t index) {
		return imageAvailableSemaphores[index];
	}
	VkSemaphore &getRenderFinishedSemaphore(size_t index) {
		return renderFinishedSemaphores[index];
	}
	VkFence &getInFlightFence(size_t index) { return inFlightFences[index]; }

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

	size_t max_frames_in_flight;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
};
}  // namespace vk