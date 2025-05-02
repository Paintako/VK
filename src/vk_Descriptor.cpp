#include "vk_Descriptor.hpp"

#include <iostream>
#include <stdexcept>

namespace vk {
vk_Descriptor::vk_Descriptor(uint32_t maxFramesInFlight, VkDevice &device)
	: maxFramesInFlight(maxFramesInFlight), device(device) {
	// Constructor implementation
	std::cout << "Descriptor constructor called" << std::endl;
	createDescriptorPool();
	std::cout << "Descriptor created" << std::endl;
}

void vk_Descriptor::createDescriptorPool() {
	// VkDescriptorPoolSize poolSize{};
	// poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// poolSize.descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

	// VkDescriptorPoolCreateInfo poolInfo{};
	// poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// poolInfo.poolSizeCount = 1;
	// poolInfo.pPoolSizes = &poolSize;
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void vk_Descriptor::createDescriptorSets(
	std::vector<VkBuffer> &uniformBuffers,
	VkDescriptorSetLayout &descriptorSetLayout,
	VkImageView &textureImageView,
	VkSampler &textureSampler) {
	std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight,
											   descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(maxFramesInFlight);
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < maxFramesInFlight; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(vk::vk_UniformBuffer::UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;

		// VkWriteDescriptorSet descriptorWrite{};
		// descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// descriptorWrite.dstSet = descriptorSets[i];
		// descriptorWrite.dstBinding = 0;
		// descriptorWrite.dstArrayElement = 0;
		// descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// descriptorWrite.descriptorCount = 1;
		// descriptorWrite.pBufferInfo = &bufferInfo;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType =
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;


		vkUpdateDescriptorSets(device,
							   static_cast<uint32_t>(descriptorWrites.size()),
							   descriptorWrites.data(), 0, nullptr);
	}
}

void vk_Descriptor::cleanupDescriptorPool() {
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	std::cout << "Descriptor pool destroyed" << std::endl;
}
}  // namespace vk