#include "window.hpp"


namespace vk {
Window::Window() {
	// Constructor implementation
	std::cout << "Window constructor called" << std::endl;
	createWindow();
	std::cout << "Window created! Pointer: " << window << std::endl;
}
Window::~Window() {
	// Destructor implementation
	std::cout << "Window destructor called" << std::endl;
	// destroyWindow();
}
void Window::createWindow() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // No OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("Failed to create GLFW window");
	}
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
	std::cout << "Creating window surface..." << std::endl;
	std::cout << "Instance: " << instance << std::endl;
	std::cout << "Window: " << window << std::endl;
	std::cout << "Surface: " << surface << std::endl;
	if (window == nullptr) {
		throw std::runtime_error("Window is not created");
	}
	if (instance == nullptr) {
		throw std::runtime_error("Instance is not created");
	}
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
		VK_SUCCESS) {
		std::cerr << "Error creating window surface: " << glfwGetError(nullptr)
				  << std::endl;
		throw std::runtime_error("Failed to create window surface");
	}
}
}  // namespace vk
