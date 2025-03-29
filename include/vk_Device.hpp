#pragma once

#include <vulkan/vulkan.hpp>

#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

namespace vk {
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() { return graphicsFamily.has_value(); }
};

class vk_Device {
public:
	vk_Device(VkInstance instance);
	~vk_Device() = default;

	vk_Device(const vk_Device &) = delete;
	vk_Device &operator=(const vk_Device &) = delete;

	void createLogicalDevice();
	void destroyLogicalDevice();

	VkDevice getLogicalDevice() const { return logicalDevice; }
	void pickPhysicalDevice();

private:
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