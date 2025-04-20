#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace vk {
class Vertex {
public:
	Vertex(VkDevice &device,
		   VkPhysicalDevice &physicalDevice,
		   VkCommandPool &commandPool,
		   VkQueue &graphicsQueue)
		: device(device),
		  physicalDevice(physicalDevice),
		  commandPool(commandPool),
		  graphicsQueue(graphicsQueue) {}
	~Vertex() = default;

	struct Vertex_struct {
		glm::vec2 pos;
		glm::vec3 color;
	};

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2>
	getAttributeDescriptions();

	void createVertexBuffer(std::vector<Vertex::Vertex_struct> &vertices);
	void createIndexBuffer(std::vector<uint16_t> &indices);

	VkBuffer &getVertexBuffer() { return vertexBuffer; }
	VkBuffer &getIndexBuffer() { return indexBuffer; }
	VkDeviceMemory &getVertexBufferMemory() { return vertexBufferMemory; }

	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties);

	void cleanupBuffers();

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue graphicsQueue;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	void createBuffer(VkDeviceSize size,
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer &buffer,
					  VkDeviceMemory &bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};
}  // namespace vk
