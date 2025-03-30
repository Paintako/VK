#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

namespace vk {
class Window {
public:
	Window();
	~Window();

	Window(const Window &) = delete;
	Window &operator=(const Window &) = delete;

	void createWindow();
	void destroyWindow();

	GLFWwindow *getWindow() const { return window; }

	void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
	GLFWwindow *window = nullptr;
};
}  // namespace vk