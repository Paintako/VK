#pragma once

#include <vulkan/vulkan.h>

#include "vk_CommandBuffers.hpp"

namespace vk {
class vk_Buffer {
public:
	vk_Buffer(VkDevice &device,
			  VkPhysicalDevice &physicalDevice,
			  vk_CommandBuffers &vk_commandBuffers,
			  VkQueue &graphicsQueue);
	~vk_Buffer() = default;

	void createBuffer(VkDeviceSize size,
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer &buffer,
					  VkDeviceMemory &bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties);

	VkDevice &getDevice() { return device; }

private:
	vk_CommandBuffers &vk_commandBuffers;

	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue graphicsQueue;
};
}  // namespace vk