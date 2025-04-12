#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "window.hpp"

namespace vk {
class vk_Instance {
public:
	vk_Instance(Window &window);
	~vk_Instance() = default;

	vk_Instance(const vk_Instance &) = delete;
	vk_Instance &operator=(const vk_Instance &) = delete;

	void createInstance();
	void destroyInstance();

	// Return a reference to the Vulkan instance
	VkInstance &getInstance() { return instance; }
	VkSurfaceKHR &getSurface() { return surface; }
	// Return a reference to the window
	Window &getWindow_reference() { return window; }

private:
	Window &window;
	VkSurfaceKHR surface;
	VkInstance instance;

	/* Extension layers */
	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation"};

	/* Helper function */
	bool checkValidationLayerSupport();
};
}  // namespace vk
