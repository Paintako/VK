#pragma once

#include <vulkan/vulkan.h>

#include "vk_Device.hpp"
#include "vk_FrameBuffer.hpp"
#include "vk_Vertex.hpp"

#include <vector>

namespace vk {
class vk_CommandBuffers {
public:
	vk_CommandBuffers(vk_Device &device, size_t max_frames_in_flight);
	~vk_CommandBuffers() = default;

	void createCommandBuffer(size_t max_frames_in_flight);
	void beginCommandBuffer();
	void endCommandBuffer();
	void submitCommandBuffer();

	void recordCommandBuffer(VkCommandBuffer &commandBuffer,
							 VkRenderPass &renderPass,
							 VkPipeline &graphicsPipeline,
							 VkFramebuffer &frameBuffer,
							 VkExtent2D &swapChainExtent,
							 VkBuffer &vertexBuffer,
							 VkBuffer &indexBuffer,
							 std::vector<Vertex::Vertex_struct> &vertices,
							 std::vector<uint16_t> &indices);

	VkCommandBuffer &getCommandBuffer() { return commandBuffer; }
	VkCommandPool &getCommandPool() { return commandPool; }
	VkCommandBuffer &getCommandBuffer(size_t index) {
		return commandBuffers[index];
	}

	void cleanupCommandPool();

private:
	vk_Device &device;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	std::vector<VkCommandBuffer> commandBuffers;

	size_t max_frames_in_flight;

	/* Helper function */
	void createCommandPool();
};
}  // namespace vk