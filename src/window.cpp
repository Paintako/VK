#include "window.hpp"


namespace vk {
Window::Window() {
	// Constructor implementation
	std::cout << "Window constructor called" << std::endl;
	createWindow();
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
}  // namespace vk
