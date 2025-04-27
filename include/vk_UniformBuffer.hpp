#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "vk_Buffer.hpp"

#include <vector>

namespace vk {
class vk_UniformBuffer {
public:
	vk_UniformBuffer(vk_Buffer &buffer);

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	void createDescriptorSetLayout();
	void createUniformBuffers(uint32_t frame_in_flights);

	void updateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent);

	void cleanupBuffers();
	void cleanupDescriptorSetLayout();

	VkDescriptorSetLayout &getDescriptorSetLayout() {
		return descriptorSetLayout;
	}

	std::vector<VkBuffer> &getUniformBuffers() { return uniformBuffers; }

private:
	vk_Buffer &buffer;

	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void *> uniformBuffersMapped;
};
}  // namespace vk