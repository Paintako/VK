#pragma once

#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "vk_Device.hpp"
#include "vk_Instance.hpp"
#include "window.hpp"

namespace vk {
class App {
public:
	App();
	~App();

	App(const App &) = delete;
	App &operator=(const App &) = delete;

	void run();

private:
	void initVulkan();
	void mainLoop();
	void cleanup();

	// Class members
	Window window;
	vk_Instance instance{window};
	vk_Device device{instance.getInstance(), instance.getSurface()};
};
}  // namespace vk