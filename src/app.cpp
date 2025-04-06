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
	}
}
void vk::App::cleanup() {
	std::cout << "Cleaning up..." << std::endl;
	graphicsPipeline.cleanupGraphicsPipeline();
	std::cout << "Graphics pipeline cleaned up!" << std::endl;
	vkDestroyPipelineLayout(device.getLogicalDevice(),
							pipelineLayout.getPipelineLayout(), nullptr);
	std::cout << "Pipeline layout destroyed!" << std::endl;
	renderPass.cleanupRenderPass();
	std::cout << "Render pass destroyed!" << std::endl;
	swapChain.destroySwapChainImageViews();
	std::cout << "Swap chain image views destroyed!" << std::endl;
	swapChain.destroySwapChain();
	std::cout << "Swap chain destroyed!" << std::endl;

	if (device.getLogicalDevice() != VK_NULL_HANDLE) {
		std::cout << "Destroying logical device..." << std::endl;
		device.destroyLogicalDevice();
		std::cout << "Logical device destroyed!" << std::endl;
	}

	// device.destroyLogicalDevice();
	std::cout << "Logical device destroyed!" << std::endl;
	// if (enableValidationLayers) {
	// 	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	// }

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