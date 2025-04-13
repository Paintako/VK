#pragma once

#include <vulkan/vulkan.h>

#include "vk_Device.hpp"
#include "vk_FrameBuffer.hpp"

namespace vk {
class vk_CommandBuffers {
public:
	vk_CommandBuffers(vk_Device &device);
	~vk_CommandBuffers() = default;

	void createCommandBuffer();
	void beginCommandBuffer();
	void endCommandBuffer();
	void submitCommandBuffer();

	void recordCommandBuffer(VkCommandBuffer &commandBuffer,
							 VkRenderPass &renderPass,
							 VkPipeline &graphicsPipeline,
							 VkFramebuffer &frameBuffer,
							 VkExtent2D &swapChainExtent);

	VkCommandBuffer &getCommandBuffer() { return commandBuffer; }
	VkCommandPool &getCommandPool() { return commandPool; }

	void cleanupCommandPool();

private:
	vk_Device &device;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	/* Helper function */
	void createCommandPool();
};
}  // namespace vk