#pragma once

#include <vulkan/vulkan.h>

#include "vk_Buffer.hpp"
#include "vk_CommandBuffers.hpp"

namespace vk {
class vk_Image {
public:
	vk_Image(vk_Buffer &buffer, vk_CommandBuffers &commandBuffers);
	void createTextureImage();
	void createImage(uint32_t width,
					 uint32_t height,
					 VkFormat format,
					 VkImageTiling tiling,
					 VkImageUsageFlags usage,
					 VkMemoryPropertyFlags properties,
					 VkImage &image,
					 VkDeviceMemory &imageMemory);
	void copyBufferToImage(VkBuffer buffer,
						   VkImage image,
						   uint32_t width,
						   uint32_t height);
	void transitionImageLayout(VkImage image,
							   VkFormat format,
							   VkImageLayout oldLayout,
							   VkImageLayout newLayout);

	void cleanup();

private:
	vk_Buffer &buffer;
	vk_CommandBuffers &commandBuffers;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
};
}  // namespace vk