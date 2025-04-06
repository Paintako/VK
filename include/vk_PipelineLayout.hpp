#pragma once

#include <vulkan/vulkan.h>

namespace vk {
class vk_PipelineLayout {
public:
	vk_PipelineLayout(VkDevice &device);
	~vk_PipelineLayout() = default;

	void cleanupPipelineLayout();
	VkPipelineLayout &getPipelineLayout() { return pipelineLayout; };

private:
	VkDevice &device;
	VkPipelineLayout pipelineLayout;

	void createPipelineLayout();
};
}  // namespace vk