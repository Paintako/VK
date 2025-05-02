#include "vk_Vertex.hpp"
#include "vk_Buffer.hpp"

#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace vk {
VkVertexInputBindingDescription Vertex::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex::Vertex_struct);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3>
Vertex::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex::Vertex_struct, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex::Vertex_struct, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex::Vertex_struct, texCoord);

	return attributeDescriptions;
}

void Vertex::createVertexBuffer(std::vector<Vertex::Vertex_struct> &vertices) {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	// Staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	std::cout << "Creating staging buffer!" << std::endl;
	std::cout << "Buffer size: " << bufferSize << std::endl;
	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);
	std::cout << "Staging buffer created!" << std::endl;
	// Copy data to staging buffer
	void *data;
	vkMapMemory(buffer.getDevice(), stagingBufferMemory, 0, bufferSize, 0,
				&data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(buffer.getDevice(), stagingBufferMemory);

	std::cout << "Mapped memory!" << std::endl;

	std::cout << "Creating vertex buffer!" << std::endl;
	buffer.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	std::cout << "Vertex buffer created!" << std::endl;

	// Copy data from staging buffer to vertex buffer
	buffer.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(buffer.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(buffer.getDevice(), stagingBufferMemory, nullptr);
}

void Vertex::createIndexBuffer(std::vector<uint16_t> &indices) {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(buffer.getDevice(), stagingBufferMemory, 0, bufferSize, 0,
				&data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(buffer.getDevice(), stagingBufferMemory);

	buffer.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	buffer.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(buffer.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(buffer.getDevice(), stagingBufferMemory, nullptr);
}

void Vertex::cleanupBuffers() {
	if (indexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(buffer.getDevice(), indexBuffer, nullptr);
		vkFreeMemory(buffer.getDevice(), indexBufferMemory, nullptr);
	}
	if (vertexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(buffer.getDevice(), vertexBuffer, nullptr);
		vkFreeMemory(buffer.getDevice(), vertexBufferMemory, nullptr);
	}
}
};	// namespace vk