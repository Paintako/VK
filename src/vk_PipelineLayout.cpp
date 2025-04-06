#include "vk_PipelineLayout.hpp"

#include <iostream>
#include <stdexcept>

namespace vk {
vk_PipelineLayout::vk_PipelineLayout(VkDevice &device) : device(device) {
	// Constructor implementation
	std::cout << "PipelineLayout constructor called" << std::endl;
	createPipelineLayout();
	std::cout << "PipelineLayout created" << std::endl;
}

void vk_PipelineLayout::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;			// Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0;	// Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
							   &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void vk_PipelineLayout::cleanupPipelineLayout() {
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	std::cout << "PipelineLayout destructor called" << std::endl;
}
}  // namespace vk