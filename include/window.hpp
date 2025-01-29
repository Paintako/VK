#pragma once

/* Standard library */
#include <string>

/* GLFW for surface window*/
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace vk {
class Window {
public:
	Window(int w, int h, std::string title);
	~Window();

	/* Determines whether the surface should be cloese*/
	bool shouldClose();

	/* Creates a window surface, which connects to Vkinstance */
	void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

	/* Get extension from swap chain */
	VkExtent2D getExtent() { return {WIDTH, HEIGHT}; }

private:
	/* Variables & attributes */
	GLFWwindow *window;

	const uint32_t WIDTH;
	const uint32_t HEIGHT;

	std::string windowTitle;


	/* Functions */
	void initWindow();
};
}  // namespace vk