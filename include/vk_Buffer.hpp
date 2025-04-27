#pragma once

#include <vulkan/vulkan.h>

namespace vk {
class vk_Buffer {
public:
	vk_Buffer(VkDevice &device,
			  VkPhysicalDevice &physicalDevice,
			  VkCommandPool &commandPool,
			  VkQueue &graphicsQueue);
	~vk_Buffer() = default;

	void createBuffer(VkDeviceSize size,
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer &buffer,
					  VkDeviceMemory &bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkDevice &getDevice() { return device; }

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue graphicsQueue;

	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties);
};
}  // namespace vk