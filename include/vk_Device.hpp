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

	void createLogicalDevice();
	void destroyLogicalDevice();

	VkDevice getLogicalDevice() const { return logicalDevice; }
	void pickPhysicalDevice();

private:
	VkSurfaceKHR surface = nullptr;

	VkInstance instance = nullptr;
	VkPhysicalDevice physicalDevice = nullptr;
	VkDevice logicalDevice = nullptr;

	VkQueue graphicsQueue = nullptr;
	VkQueue presentQueue = nullptr;

	/* Helper function */
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
}  // namespace vk