#pragma once

#include <vulkan/vulkan.h>
#include "vk_UniformBuffer.hpp"

#include <vector>

namespace vk {
class vk_Descriptor {
public:
	vk_Descriptor(uint32_t maxFramesInFlight, VkDevice &device);
	void createDescriptorPool();
	void createDescriptorSets(std::vector<VkBuffer> &uniformBuffers,
							  VkDescriptorSetLayout &descriptorSetLayout,
							  VkImageView &textureImageView,
							  VkSampler &textureSampler);

	std::vector<VkDescriptorSet> &getDescriptorSets() { return descriptorSets; }

	void cleanupDescriptorPool();

private:
	uint32_t maxFramesInFlight;
	VkDevice &device;

	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;
};
}  // namespace vk