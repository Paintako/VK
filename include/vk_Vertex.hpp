#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace vk {
// Forward declaration of vk_Buffer class
class vk_Buffer;
class Vertex {
public:
	Vertex(vk_Buffer &buffer) : buffer(buffer) {}
	~Vertex() = default;

	struct Vertex_struct {
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;	 // determine how the image is actually mapped to
							 // the geometry
	};

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3>
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
	vk_Buffer &buffer;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
};
}  // namespace vk
