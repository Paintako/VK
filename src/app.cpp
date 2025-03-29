#include "app.hpp"

vk::App::App() {
	// Constructor implementation
	std::cout << "App constructor called" << std::endl;
}

vk::App::~App() {
	// Destructor implementation
	std::cout << "App destructor called" << std::endl;
	// cleanup();
}
void vk::App::run() {
	initVulkan();
	mainLoop();
	// cleanup();
}
void vk::App::initVulkan() {
	// Vulkan initialization code
	window = Window().getWindow();
	instance = Vulkan_Instance().getInstance();
	Vulkan_Device(instance).pickPhysicalDevice();
}
void vk::App::mainLoop() {
	// Main loop code
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}
void vk::App::cleanup() {
	// Cleanup code
}