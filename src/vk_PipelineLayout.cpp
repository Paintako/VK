#include "vk_PipelineLayout.hpp"

#include <iostream>
#include <stdexcept>

namespace vk {
vk_PipelineLayout::vk_PipelineLayout(VkDevice &device,
									 VkDescriptorSetLayout &descriptorSetLayout)
	: device(device), descriptorSetLayout(descriptorSetLayout) {
	// Constructor implementation
	std::cout << "PipelineLayout constructor called" << std::endl;
	createPipelineLayout();
	std::cout << "PipelineLayout created" << std::endl;
}

void vk_PipelineLayout::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;	// Optional
	std::cout << "Using descriptor set layout: " << descriptorSetLayout
			  << std::endl;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;	// Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0;			// Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
							   &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void vk_PipelineLayout::cleanupPipelineLayout() {
	if (pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
		std::cout << "Pipeline layout destroyed" << std::endl;
	}
}
}  // namespace vk