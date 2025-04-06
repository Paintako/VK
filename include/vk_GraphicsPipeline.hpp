#pragma once

#include <vulkan/vulkan.h>

namespace vk {
class vk_GraphicsPipeline {
public:
	vk_GraphicsPipeline(VkDevice &device,
						VkRenderPass &renderPass,
						VkPipelineLayout &pipelineLayout);
	~vk_GraphicsPipeline();

	void createGraphicsPipeline(const char *vertShaderPath,
								const char *fragShaderPath);

	VkPipeline getGraphicsPipeline() { return graphicsPipeline; }

private:
	/* Helper functions */
	void createShaderModule(const char *shaderPath,
							VkShaderModule &shaderModule);
	void createPipelineLayout();
	void createRenderPass();
	void createGraphicsPipeline();

	void cleanupShaderModule(VkShaderModule &shaderModule);
	void cleanupPipelineLayout();
	void cleanupRenderPass();
	void cleanupGraphicsPipeline();

	/* Member variables */
	VkDevice &device;
	VkRenderPass &renderPass;
	VkPipelineLayout &pipelineLayout;
	VkPipeline graphicsPipeline;

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
};
} // namespace vk
