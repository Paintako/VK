#include "vk_Image.hpp"

#include <stdexcept>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace vk {
vk_Image::vk_Image(vk_Buffer &buffer, vk_CommandBuffers &commandBuffers)
	: buffer(buffer), commandBuffers(commandBuffers) {
	// Create staging buffer
	createTextureImage();
}

void vk_Image::createTextureImage() {
	int texWidth, texHeight, texChannels;
	stbi_uc *pixels = stbi_load("textures/river-8285515.jpg", &texWidth,
								&texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize =
		texWidth * texHeight * 4;  // Assuming 4 channels (RGBA)

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	buffer.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(buffer.getDevice(), stagingBufferMemory, 0, imageSize, 0,
				&data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(buffer.getDevice(), stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(
		texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
						  VK_IMAGE_LAYOUT_UNDEFINED,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(buffer.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(buffer.getDevice(), stagingBufferMemory, nullptr);

	std::cout << "Texture image created!" << std::endl;
}

void vk_Image::createImage(uint32_t width,
						   uint32_t height,
						   VkFormat format,
						   VkImageTiling tiling,
						   VkImageUsageFlags usage,
						   VkMemoryPropertyFlags properties,
						   VkImage &image,
						   VkDeviceMemory &imageMemory) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;	 // 2D image, so depth is 1
	imageInfo.mipLevels = 1;	 // No mipmaps (mip map is a series of images)
	imageInfo.arrayLayers = 1;	 // array of images
	imageInfo.format = format;
	imageInfo.tiling = tiling;	// linear increases memory BW
	imageInfo.initialLayout =
		VK_IMAGE_LAYOUT_UNDEFINED;	// Because we're using staging buffer, so we
									// don't care about the initial layout
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;	// No multisampling
	imageInfo.sharingMode =
		VK_SHARING_MODE_EXCLUSIVE;	// Only one queue family will use this image

	if (vkCreateImage(buffer.getDevice(), &imageInfo, nullptr, &image) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(buffer.getDevice(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex =
		buffer.findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(buffer.getDevice(), &allocInfo, nullptr,
						 &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(buffer.getDevice(), image, imageMemory, 0);
}

void vk_Image::copyBufferToImage(VkBuffer buffer,
								 VkImage image,
								 uint32_t width,
								 uint32_t height) {
	VkCommandBuffer commandBuffer = commandBuffers.beginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;  // specifies the byte offset in the buffer at
							  // which the pixel values start.
	region.bufferRowLength =
		0;	// ields specify how the pixels are laid out in memory
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {0, 0, 0};
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;

	// Copy the buffer to the image, assuming the image is in the correct layout
	vkCmdCopyBufferToImage(commandBuffer, buffer, image,
						   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
						   &region);  // 1 region

	commandBuffers.endSingleTimeCommands(commandBuffer);
}

void vk_Image::transitionImageLayout(VkImage image,
									 VkFormat format,
									 VkImageLayout oldLayout,
									 VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = commandBuffers.beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
						 nullptr, 0, nullptr, 1, &barrier);
	commandBuffers.endSingleTimeCommands(commandBuffer);
}

void vk_Image::cleanup() {
	vkDestroyImage(buffer.getDevice(), textureImage, nullptr);
	vkFreeMemory(buffer.getDevice(), textureImageMemory, nullptr);
}
}  // namespace vk