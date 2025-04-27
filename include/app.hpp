#pragma once

#include <array>
#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "vk_Buffer.hpp"
#include "vk_CommandBuffers.hpp"
#include "vk_Device.hpp"
#include "vk_FrameBuffer.hpp"
#include "vk_GraphicsPipeline.hpp"
#include "vk_Instance.hpp"
#include "vk_PipelineLayout.hpp"
#include "vk_RenderPass.hpp"
#include "vk_SwapChain.hpp"
#include "vk_Sync.hpp"
#include "vk_Vertex.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

#define MAX_FRAMES_IN_FLIGHT 2

namespace vk {
class App {
public:
	App();
	~App();

	App(const App &) = delete;
	App &operator=(const App &) = delete;

	void run();

	bool framebufferResized = false;

	std::vector<Vertex::Vertex_struct> vertices;  // For vertex buffer
	std::vector<uint16_t> indices;				  // For index buffer

private:
	void initVulkan();
	void mainLoop();
	void cleanup();

	void drawFrame();
	void recreateSwapChain();
	void cleanupSwapChain();

	// Class members
	Window window;
	vk_Instance instance{window};
	vk_Device device{instance.getInstance(), instance.getSurface()};
	vk_SwapChain swapChain{device, instance};
	vk_PipelineLayout pipelineLayout{device.getLogicalDevice()};
	vk_RenderPass renderPass{device.getLogicalDevice(),
							 swapChain.getSwapChainImageFormat()};
	vk_GraphicsPipeline graphicsPipeline{device.getLogicalDevice(),
										 renderPass.getRenderPass(),
										 pipelineLayout.getPipelineLayout()};
	vk_FrameBuffer frameBuffer{device.getLogicalDevice(),
							   renderPass.getRenderPass(), swapChain};
	vk_CommandBuffers commandBuffers{device, MAX_FRAMES_IN_FLIGHT};
	vk_Buffer buffer{device.getLogicalDevice(), device.getPhysicalDevice(),
					 commandBuffers.getCommandPool(),
					 device.getGraphicsQueue()};
	Vertex vertex{buffer};
	vk_Sync sync_object{device.getLogicalDevice(), MAX_FRAMES_IN_FLIGHT};

	uint32_t currentFrame = 0;
	uint32_t max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;
};
}  // namespace vk