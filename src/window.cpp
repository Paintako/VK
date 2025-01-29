#include "window.hpp"

#include <stdexcept>

namespace vk {
Window::Window(int w, int h, std::string title)
	: WIDTH(w), HEIGHT(h), windowTitle(title) {
	initWindow();
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API,
				   GLFW_NO_API);  // No OpenGL context, using Vulkan instead
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window =
		glfwCreateWindow(WIDTH, HEIGHT, windowTitle.c_str(), nullptr, nullptr);
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
		VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}
}  // namespace vk