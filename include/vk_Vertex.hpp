#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace vk {
class Vertex {
public:
	Vertex() = default;
	~Vertex() = default;

	struct Vertex_struct {
		glm::vec2 pos;
		glm::vec3 color;
	};

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2>
	getAttributeDescriptions();
	void createVertexBuffer(std::vector<Vertex::Vertex_struct> &vertices,
							VkDevice device,
							VkPhysicalDevice physicalDevice);

	VkBuffer &getVertexBuffer() { return vertexBuffer; }
	VkDeviceMemory &getVertexBufferMemory() { return vertexBufferMemory; }

	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties,
							VkPhysicalDevice physicalDevice);

private:
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
};
}  // namespace vk
