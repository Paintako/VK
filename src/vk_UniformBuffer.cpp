#include "vk_UniformBuffer.hpp"
#include "vk_Buffer.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace vk {
vk_UniformBuffer::vk_UniformBuffer(vk_Buffer &buffer) : buffer(buffer) {
	// Constructor implementation
	std::cout << "UniformBuffer constructor called" << std::endl;
	createDescriptorSetLayout();
	std::cout << "UniformBuffer created" << std::endl;
}

void vk_UniformBuffer::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;  // Align with the binding in the shader
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount =
		1;	// descriptorCount can be more than 1 if you have multiple uniform
			// buffers

	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	uboLayoutBinding.pImmutableSamplers = nullptr;	// Optional

	// For sampler
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType =
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
		uboLayoutBinding, samplerLayoutBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(buffer.getDevice(), &layoutInfo, nullptr,
									&descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	std::cout << "Descriptor set layout created! ptr: " << descriptorSetLayout
			  << std::endl;
}

void vk_UniformBuffer::createUniformBuffers(uint32_t frame_in_flights) {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(frame_in_flights);
	uniformBuffersMemory.resize(frame_in_flights);
	uniformBuffersMapped.resize(frame_in_flights);

	for (size_t i = 0; i < frame_in_flights; i++) {
		buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(buffer.getDevice(), uniformBuffersMemory[i], 0, bufferSize,
					0, &uniformBuffersMapped[i]);
	}
}

void vk_UniformBuffer::updateUniformBuffer(uint32_t currentImage,
										   VkExtent2D swapChainExtent) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(
					 currentTime - startTime)
					 .count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(360.0f),
							glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view =
		glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(
		glm::radians(45.0f),
		swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void vk_UniformBuffer::cleanupBuffers() {
	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		vkUnmapMemory(buffer.getDevice(), uniformBuffersMemory[i]);
		vkDestroyBuffer(buffer.getDevice(), uniformBuffers[i], nullptr);
		vkFreeMemory(buffer.getDevice(), uniformBuffersMemory[i], nullptr);
	}
	std::cout << "Uniform buffers destroyed!" << std::endl;
}

void vk_UniformBuffer::cleanupDescriptorSetLayout() {
	if (descriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(buffer.getDevice(), descriptorSetLayout,
									 nullptr);
		descriptorSetLayout = VK_NULL_HANDLE;
		std::cout << "Descriptor set layout destroyed" << std::endl;
	}
}
}  // namespace vk