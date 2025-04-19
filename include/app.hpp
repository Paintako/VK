#pragma once

#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "vk_CommandBuffers.hpp"
#include "vk_Device.hpp"
#include "vk_FrameBuffer.hpp"
#include "vk_GraphicsPipeline.hpp"
#include "vk_Instance.hpp"
#include "vk_PipelineLayout.hpp"
#include "vk_RenderPass.hpp"
#include "vk_SwapChain.hpp"
#include "vk_Sync.hpp"
#include "window.hpp"

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
	vk_Sync sync_object{device.getLogicalDevice(), MAX_FRAMES_IN_FLIGHT};

	uint32_t currentFrame = 0;
	uint32_t max_frames_in_flight = MAX_FRAMES_IN_FLIGHT;
};
}  // namespace vk