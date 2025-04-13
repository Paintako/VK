#include "vk_Sync.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace vk {
vk_Sync::vk_Sync(VkDevice &device) : device(device) {
	std::cout << "Sync constructor called" << std::endl;
	createSyncObjects();
	std::cout << "Sync objects created" << std::endl;
}

void vk_Sync::createSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
						  &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr,
						  &renderFinishedSemaphore) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
	}

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create fence!");
	}
	std::cout << "Sync objects created" << std::endl;
}

void vk_Sync::cleanupSyncObjects() {
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	vkDestroyFence(device, inFlightFence, nullptr);
	std::cout << "Sync objects destroyed" << std::endl;
}
}  // namespace vk