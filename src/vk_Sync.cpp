#include "vk_Sync.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace vk {
vk_Sync::vk_Sync(VkDevice &device, size_t max_frames_in_flight)
	: device(device), max_frames_in_flight(max_frames_in_flight) {
	std::cout << "Sync constructor called" << std::endl;
	createSyncObjects(max_frames_in_flight);
	std::cout << "Sync objects created" << std::endl;
}

void vk_Sync::createSyncObjects(size_t max_frames_in_flight) {
	imageAvailableSemaphores.resize(max_frames_in_flight);
	renderFinishedSemaphores.resize(max_frames_in_flight);
	inFlightFences.resize(max_frames_in_flight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < max_frames_in_flight; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
							  &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr,
							  &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) !=
				VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}
	std::cout << "Sync objects created" << std::endl;
}

void vk_Sync::cleanupSyncObjects() {
	for (int i = 0; i < max_frames_in_flight; i++) {
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
		std::cout << "Sync objects destroyed" << std::endl;
	}
	std::cout << "Sync objects destroyed" << std::endl;
}
}  // namespace vk