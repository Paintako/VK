#pragma once

#include <vulkan/vulkan.h>

namespace vk {
class vk_RenderPass {
public:
	vk_RenderPass(VkDevice &device, VkFormat swapChainImageFormat);
	~vk_RenderPass() = default;

	void cleanupRenderPass();
	VkRenderPass &getRenderPass() { return renderPass; };

private:
	VkDevice &device;
	VkRenderPass renderPass;
	VkFormat swapChainImageFormat;

	void createRenderPass();
};
}  // namespace vk