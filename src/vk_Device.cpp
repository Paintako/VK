#include "vk_Device.hpp"

namespace vk {
vk_Device::vk_Device(VkInstance instance) : instance(instance) {
	std::cout << "Creating Vulkan Device..." << std::endl;
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

bool vk_Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.isComplete();
}

QueueFamilyIndices vk_Device::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
											 nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
											 queueFamilies.data());

	int i = 0;
	// Find a queue family that supports graphics operations
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void vk_Device::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0,
					 &graphicsQueue);
}
}  // namespace vk