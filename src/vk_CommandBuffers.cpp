#include "vk_CommandBuffers.hpp"
#include "vk_Vertex.hpp"

#include <iostream>

namespace vk {
vk_CommandBuffers::vk_CommandBuffers(vk_Device &device,
									 size_t max_frames_in_flight)
	: device(device), max_frames_in_flight(max_frames_in_flight) {
	// Constructor implementation
	std::cout << "CommandBuffers constructor called" << std::endl;
	createCommandPool();
	std::cout << "Command pool created" << std::endl;
	createCommandBuffer(max_frames_in_flight);
	std::cout << "Command buffer created" << std::endl;
}

void vk_CommandBuffers::createCommandPool() {
	// Create a command pool
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex =
		device.getQueueFamilyIndices().graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device.getLogicalDevice(), &poolInfo, nullptr,
							&commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
	std::cout << "Command pool created" << std::endl;
}

void vk_CommandBuffers::createCommandBuffer(size_t max_frames_in_flight) {
	// Allocate command buffer
	commandBuffers.resize(max_frames_in_flight);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) max_frames_in_flight;

	if (vkAllocateCommandBuffers(device.getLogicalDevice(), &allocInfo,
								 commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffer!");
	}
	std::cout << "Command buffer allocated" << std::endl;
}

void vk_CommandBuffers::recordCommandBuffer(
	VkCommandBuffer &commandBuffer,
	VkRenderPass &renderPass,
	VkPipeline &graphicsPipeline,
	VkFramebuffer &frameBuffer,
	VkExtent2D &swapChainExtent,
	VkBuffer &vertexBuffer,
	std::vector<Vertex::Vertex_struct> &vertices) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;				   // Optional
	beginInfo.pInheritanceInfo = nullptr;  // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	// Drawing starts by beginning the render pass
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = frameBuffer;

	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	// Begin the render pass, all of the functions that record commands can be
	// recognized by their vkCmd prefix.
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
						 VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
					  graphicsPipeline);

	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// // Dump the vertex buffer contents
	// std::cout << "Vertex buffer contents:" << std::endl;
	// for (const auto &vertex : vertices) {
	// 	std::cout << "Position: (" << vertex.pos.x << ", " << vertex.pos.y
	// 			  << "), Color: (" << vertex.color.r << ", " << vertex.color.g
	// 			  << ", " << vertex.color.b << ")" << std::endl;
	// }

	// Draw the triangle
	vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

	// End the render pass
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void vk_CommandBuffers::cleanupCommandPool() {
	vkDestroyCommandPool(device.getLogicalDevice(), commandPool, nullptr);
	std::cout << "Command pool destroyed" << std::endl;
}
}  // namespace vk