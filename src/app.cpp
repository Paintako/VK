#include "app.hpp"

vk::App::App() {
	// Constructor implementation
	std::cout << "App constructor called" << std::endl;
}

vk::App::~App() {
	// Destructor implementation
	std::cout << "App destructor called" << std::endl;
	cleanup();
	std::cout << "App destructor finished" << std::endl;
}
void vk::App::run() {
	initVulkan();
	mainLoop();
	// cleanup();
}
void vk::App::initVulkan() {
	// Vulkan initialization code
	std::cout << "Vulkan initialized!" << std::endl;
}
void vk::App::mainLoop() {
	// Main loop code
	while (!glfwWindowShouldClose(window.getWindow())) {
		glfwPollEvents();
		commandBuffers.recordCommandBuffer(
			commandBuffers.getCommandBuffer(), renderPass.getRenderPass(),
			graphicsPipeline.getGraphicsPipeline(),
			frameBuffer.getswapChainFrameBuffer()[0],
			swapChain.getSwapChainExtent()	// Pass the swap chain extent
		);
	}
}
void vk::App::cleanup() {
	std::cout << "Cleaning up..." << std::endl;
	commandBuffers.cleanupCommandPool();
	std::cout << "Command pool destroyed!" << std::endl;
	frameBuffer.cleanupFrameBuffer();
	std::cout << "Frame buffer destroyed!" << std::endl;
	pipelineLayout.cleanupPipelineLayout();
	std::cout << "Pipeline layout destroyed!" << std::endl;
	renderPass.cleanupRenderPass();
	std::cout << "Render pass destroyed!" << std::endl;
	swapChain.destroySwapChainImageViews();
	std::cout << "Swap chain image views destroyed!" << std::endl;
	swapChain.destroySwapChain();
	std::cout << "Swap chain destroyed!" << std::endl;
	device.destroyLogicalDevice();
	std::cout << "Logical device destroyed!" << std::endl;
	vkDestroySurfaceKHR(instance.getInstance(), instance.getSurface(), nullptr);
	std::cout << "Surface destroyed!" << std::endl;
	vkDestroyInstance(instance.getInstance(), nullptr);
	std::cout << "Instance destroyed!" << std::endl;
	glfwDestroyWindow(window.getWindow());
	std::cout << "Window destroyed!" << std::endl;
	glfwTerminate();
	std::cout << "GLFW terminated!" << std::endl;
	std::cout << "App cleanup finished!" << std::endl;
}