#include "vk_GraphicsPipeline.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>

namespace vk {
    vk_GraphicsPipeline::vk_GraphicsPipeline(VkDevice &device,
                                               VkRenderPass &renderPass,
                                               VkPipelineLayout &pipelineLayout)
        : device(device), renderPass(renderPass), pipelineLayout(pipelineLayout) {
        // Constructor implementation
        std::cout << "GraphicsPipeline constructor called" << std::endl;
    }


    vk_GraphicsPipeline::~vk_GraphicsPipeline() {
        // Destructor implementation
        std::cout << "GraphicsPipeline destructor called" << std::endl;
        // cleanupGraphicsPipeline();
        // cleanupRenderPass();
        // cleanupPipelineLayout();
    }

    static std::vector<char> readFile(const std::string& filename) {
        // Open the file in binary mode and move the cursor to the end to get the size of the file
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }
        
        // tellg() returns the current position of the cursor in the file, since we've moved it to the end, this gives us the size of the file
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        // Move the cursor back to the beginning of the file, and read the file into the buffer
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }


    void vk_GraphicsPipeline::createShaderModule(const char *shaderPath,
                                                 VkShaderModule &shaderModule) {
        auto code = readFile(shaderPath);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        // The Vulkan API expects the code to be in uint32_t format, so we need to reinterpret the data
        // as a pointer to uint32_t. This is safe because the shader code is guaranteed to be aligned to 4 bytes.
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); 

        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
    }

    void vk_GraphicsPipeline::createGraphicsPipeline(const char *vertShaderPath,
                                                     const char *fragShaderPath) {
        createShaderModule(vertShaderPath, vertShaderModule);
        createShaderModule(fragShaderPath, fragShaderModule);

        std::cout << "Creating graphics pipeline..." << std::endl;
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        // Cleanup shader modules after creating the pipeline
        cleanupShaderModule(vertShaderModule);
        cleanupShaderModule(fragShaderModule);
    }
}

