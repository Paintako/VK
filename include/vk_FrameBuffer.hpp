#pragma once

#include <vulkan/vulkan.h>

#include "vk_SwapChain.hpp"

namespace vk {
class vk_FrameBuffer {
public:
	vk_FrameBuffer(VkDevice &device,
				   VkRenderPass &renderPass,
				   vk_SwapChain &swapChain);
	~vk_FrameBuffer() = default;

	void createFrameBuffer(std::vector<VkImageView> &swapChainImageViews);
	std::vector<VkFramebuffer> &getswapChainFrameBuffer() {
		return swapChainFramebuffers;
	}
	void cleanupFrameBuffer();

private:
	/* Member variables */
	VkDevice &device;
	VkRenderPass &renderPass;
	vk_SwapChain &swapChain;
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;

	std::vector<VkFramebuffer> swapChainFramebuffers;
};
}  // namespace vk
