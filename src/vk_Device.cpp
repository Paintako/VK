#include "vk_Device.hpp"

namespace vk {
vk_Device::vk_Device(VkInstance &instance, VkSurfaceKHR &surface)
	: instance{instance}, surface{surface} {
	std::cout << "Creating Vulkan Device..." << std::endl;
	std::cout << "Instance: " << instance << std::endl;
	pickPhysicalDevice();
	createLogicalDevice();
	std::cout << "Vulkan Device created!" << std::endl;
}

void vk_Device::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::cout << "Number of physical devices: " << deviceCount << std::endl;

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto &device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	std::cout << "Physical device found!" << std::endl;

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool checkDeviceExtensionSupport(
	VkPhysicalDevice device,
	const std::vector<const char *> &deviceExtensions) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
										 nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
										 availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(),
											 deviceExtensions.end());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	// check if all required extensions are supported, if not, return false
	return requiredExtensions.empty();
}

bool vk_Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);
	bool swapChainAdequate = false;
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(
		device);  // check if this physical device supports swap chain
	return indices.isComplete() &&
		   checkDeviceExtensionSupport(device, deviceExtensions) &&
		   !swapChainSupport.presentModes.empty();
}

QueueFamilyIndices vk_Device::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
											 nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
											 queueFamilies.data());

	for (int i = 0; i < queueFamilyCount; i++) {
		std::cout << "Queue family " << i << ": " << queueFamilies[i].queueCount
				  << " queues" << std::endl;
	}

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
											 &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

vk_Device::SwapChainSupportDetails vk_Device::querySwapChainSupport(
	VkPhysicalDevice device) {
	vk_Device::SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
											  &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
										 nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
											 details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
											  &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

void vk_Device::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
											  indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount =
		static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(
		deviceExtensions.size());  // enable device extensions, e.g., swapchain
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0,
					 &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0,
					 &presentQueue);
}

void vk_Device::destroyLogicalDevice() {
	std::cout << "Destroying logical device..." << std::endl;
	if (logicalDevice != VK_NULL_HANDLE) {
		std::cout << "Logical device is not null!" << std::endl;
		vkDestroyDevice(logicalDevice, nullptr);
		std::cout << "Logical device destroyed!" << std::endl;
	}
	vkDestroyDevice(logicalDevice, nullptr);
	std::cout << "Logical device destroyed!" << std::endl;
}
}  // namespace vk