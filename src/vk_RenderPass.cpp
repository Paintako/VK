#include "vk_RenderPass.hpp"

#include <iostream>
#include <stdexcept>


namespace vk {
vk_RenderPass::vk_RenderPass(VkDevice &device, VkFormat swapChainImageFormat)
	: device(device), swapChainImageFormat(swapChainImageFormat) {
	// Constructor implementation
	std::cout << "RenderPass constructor called" << std::endl;
	createRenderPass();
	std::cout << "RenderPass created" << std::endl;
}

void vk_RenderPass::createRenderPass() {
	// What is a attachment?
	// An attachment is a resource that can be used as an input or output of a
	// render pass. It can be an image, a buffer, or a depth/stencil buffer.
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void vk_RenderPass::cleanupRenderPass() {
	vkDestroyRenderPass(device, renderPass, nullptr);
	std::cout << "RenderPass destructor called" << std::endl;
}
};	// namespace vk