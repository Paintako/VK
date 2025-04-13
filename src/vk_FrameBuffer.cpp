#include "vk_FrameBuffer.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace vk {
vk_FrameBuffer::vk_FrameBuffer(VkDevice &device,
							   VkRenderPass &renderPass,
							   vk_SwapChain &swapChain)
	: device(device), renderPass(renderPass), swapChain(swapChain) {
	std::cout << "FrameBuffer constructor called" << std::endl;
	createFrameBuffer(swapChain.getSwapChainImageViews());
	std::cout << "FrameBuffer created" << std::endl;
}

void vk_FrameBuffer::createFrameBuffer(
	std::vector<VkImageView> &swapChainImageViews) {
	swapChainFramebuffers.resize(swapChainImageViews.size());
	VkExtent2D swapChainExtent = swapChain.getSwapChainExtent();

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::cout << "Creating framebuffer " << i
				  << ", attachments: " << swapChainImageViews[i] << std::endl;
		VkImageView attachments[] = {swapChainImageViews[i]};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
								&swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void vk_FrameBuffer::cleanupFrameBuffer() {
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}
	std::cout << "Framebuffers destroyed" << std::endl;
}
}  // namespace vk