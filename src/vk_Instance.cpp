#include "vk_Instance.hpp"

namespace vk {
vk_Instance::vk_Instance(Window &window) : window(window) {
	std::cout << "Creating Vulkan vk_Instance..." << std::endl;
	// Check if validation layers are supported
	if (!(checkValidationLayerSupport())) {
		throw std::runtime_error(
			"validation layers requested, but not "
			"available!");
	}
	createInstance();
	// Create window surface
	window.createWindowSurface(instance, &surface);
	std::cout << "Vulkan vk_Instance created!" << std::endl;
}

// Get required extensions, which are needed for Vulkan to work with GLFW
// (createSurface)
std::vector<const char *> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(glfwExtensions,
										 glfwExtensions + glfwExtensionCount);
	return extensions;
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
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount =
		static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vk_Instance!");
	}
}

bool vk_Instance::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (layerCount == 0) {
		std::cout << "validation layers requested, but not available!"
				  << std::endl;
		return false;
	}
	std::cout << "Number of validation layers: " << layerCount << std::endl;

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const auto &layerProperties : availableLayers) {
		std::cout << "\t" << layerProperties.layerName << std::endl;
	}

	for (const char *layerName : validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				std::cout << "\tLayer " << layerName << " found!" << std::endl;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

}  // namespace vk