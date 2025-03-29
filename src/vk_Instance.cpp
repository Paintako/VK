#include "vk_Instance.hpp"

namespace vk {
vk_Instance::vk_Instance() {
	std::cout << "Creating Vulkan vk_Instance..." << std::endl;
	createInstance();
	std::cout << "Vulkan vk_Instance created!" << std::endl;
}

void vk_Instance::createInstance() {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	/* 	Vulkan is a platform agnostic API, which means that you need an
	   extension to interface with the window system. GLFW provides a function
	   to get the required extensions for Vulkan.
	   glfwGetRequiredInstanceExtensions returns the number of extensions in the
	   array and fills the array with the names of the extensions. The function
	   returns a pointer to the array of extension names, which is stored in
	   glfwExtensions. glfwExtensions =
	   glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	*/
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vk_Instance!");
	}
}
}  // namespace vk