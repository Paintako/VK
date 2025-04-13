#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_Instance.hpp"

#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

namespace vk {
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	// Not all devices support graphics and presentation queues, so we need to
	// check if the device supports them
	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class vk_Device {
public:
	vk_Device(VkInstance &instance, VkSurfaceKHR &surface);
	~vk_Device() = default;

	vk_Device(const vk_Device &) = delete;
	vk_Device &operator=(const vk_Device &) = delete;

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	void createLogicalDevice();
	void destroyLogicalDevice();

	VkDevice &getLogicalDevice() { return logicalDevice; }
	VkPhysicalDevice &getPhysicalDevice() { return physicalDevice; }
	QueueFamilyIndices &getQueueFamilyIndices() { return queueFamilyIndices; }

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	VkQueue &getGraphicsQueue() { return graphicsQueue; }
	VkQueue &getPresentQueue() { return presentQueue; }

private:
	VkSurfaceKHR surface = nullptr;

	VkInstance instance = nullptr;
	VkPhysicalDevice physicalDevice = nullptr;
	VkDevice logicalDevice = nullptr;

	/* Queue families */
	QueueFamilyIndices queueFamilyIndices;
	VkQueue graphicsQueue = nullptr;
	VkQueue presentQueue = nullptr;

	/* Device extensions */
	const std::vector<const char *> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	/* Helper function */
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
};
}  // namespace vk