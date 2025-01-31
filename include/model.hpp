#pragma once

#include "devices.hpp"

#define GLM_FORCE_RADIANS  // force glm to use radians, no matter which version
						   // of GLM you are using
#define GLM_FORCE_DEPTH_ZERO_TO_ONE	 // force glm to use depth from 0 to 1, no
									 // matter which version of GLM you are
									 // using
#include <glm/glm.hpp>

#include <vector>
// model class is used to load vertex data from CPU (staging buffer) to GPU
// (device local buffer)
namespace vk {
class Model {
public:
	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription>
		getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription>
		getAttributeDescriptions();
	};

	Model(Device &device, const std::vector<Vertex> &vertices);
	~Model();

	// Not copyable or movable
	Model(const Model &) = delete;
	void operator=(const Model &) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

private:
	void createVertexBuffer(const std::vector<Vertex> &vertices);

	Device &device;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	uint32_t vertexCount;
};
}  // namespace vk