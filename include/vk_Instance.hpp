#pragma once

#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace vk {
class vk_Instance {
public:
	vk_Instance();
	~vk_Instance() = default;

	vk_Instance(const vk_Instance &) = delete;
	vk_Instance &operator=(const vk_Instance &) = delete;

	void createInstance();
	void destroyInstance();

	VkInstance getInstance() const { return instance; }

private:
	VkInstance instance = nullptr;
};
}  // namespace vk
