#pragma once

#include "devices.hpp"

// std
#include <string>
#include <vector>

namespace vk {

/* Configuration struct to store pipeline configuration info */
struct PipelineConfigInfo {
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class Pipeline {
public:
	Pipeline(Device &device,
			 const std::string &vertFilepath,
			 const std::string &fragFilepath,
			 const PipelineConfigInfo &configInfo);
	~Pipeline();

	// Not copyable or movable
	Pipeline(const Pipeline &) = delete;
	void operator=(const Pipeline &) = delete;

	// Bind command buffer to graphics pipeline
	void bind(VkCommandBuffer commandBuffer);

	static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width,
														uint32_t height);

private:
	static std::vector<char> readFile(const std::string &filepath);

	void createGraphicsPipeline(const std::string &vertFilepath,
								const std::string &fragFilepath,
								const PipelineConfigInfo &configInfo);

	void createShaderModule(const std::vector<char> &code,
							VkShaderModule *shaderModule);

	Device &device;
	VkPipeline graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
};
}  // namespace vk