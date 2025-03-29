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
	window = Window().getWindow();
	instance = vk_Instance().getInstance();
	logicalDevice = vk_Device(instance).getLogicalDevice();
	std::cout << "Vulkan initialized!" << std::endl;
}
void vk::App::mainLoop() {
	// Main loop code
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}
void vk::App::cleanup() {
	vkDestroyDevice(logicalDevice, nullptr);

	// vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}