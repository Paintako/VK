#include "vk_Buffer.hpp"

#include <iostream>
#include <stdexcept>

namespace vk {
vk_Buffer::vk_Buffer(VkDevice &device,
					 VkPhysicalDevice &physicalDevice,
					 vk_CommandBuffers &vk_commandBuffers,
					 VkQueue &graphicsQueue)
	: device(device),
	  physicalDevice(physicalDevice),
	  vk_commandBuffers(vk_commandBuffers),
	  graphicsQueue(graphicsQueue) {
	commandPool = vk_commandBuffers.getCommandPool();
}

void vk_Buffer::createBuffer(VkDeviceSize size,
							 VkBufferUsageFlags usage,
							 VkMemoryPropertyFlags properties,
							 VkBuffer &buffer,
							 VkDeviceMemory &bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex =
		findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);

	std::cout << "Buffer created!" << std::endl;
}

void vk_Buffer::copyBuffer(VkBuffer srcBuffer,
						   VkBuffer dstBuffer,
						   VkDeviceSize size) {
	VkCommandBuffer commandBuffer = vk_commandBuffers.beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vk_commandBuffers.endSingleTimeCommands(commandBuffer);
}


uint32_t vk_Buffer::findMemoryType(uint32_t typeFilter,
								   VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) ==
				properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

}  // namespace vk