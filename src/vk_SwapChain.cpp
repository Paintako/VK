#include "vk_SwapChain.hpp"

#include <algorithm>  // Necessary for std::clamp
#include <cstdint>	  // Necessary for uint32_t
#include <limits>	  // Necessary for std::numeric_limits

namespace vk {
vk_SwapChain::vk_SwapChain(vk_Device &device, vk_Instance &instance)
	: device{device}, instance{instance} {
	std::cout << "Creating Vulkan SwapChain..." << std::endl;
	logicalDevice = device.getLogicalDevice();
	physicalDevice = device.getPhysicalDevice();
	surface = instance.getSurface();
	createSwapChain();
	createSwapChainImageViews();
	std::cout << "Vulkan SwapChain created!" << std::endl;
}

void vk_SwapChain::createSwapChain() {
	vk_Device::SwapChainSupportDetails swapChainSupport =
		device.querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat =
		chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode =
		chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// Set minImageCount to minImageCount + 1 for double/triple buffering, and
	// make sure it doesn't exceed maxImageCount
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;  // Always 1 for 2D images
	createInfo.imageUsage =
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // Specify usage as color
											  // attachment

	QueueFamilyIndices indices = device.findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
									 indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount,
							swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void vk_SwapChain::createSwapChainImageViews() {
	swapChainImageViews.resize(swapChainImages.size());
	std::cout << "Creating " << swapChainImages.size() << " image views..."
			  << std::endl;
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask =
			VK_IMAGE_ASPECT_COLOR_BIT;	// Specify color aspect
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;	 // No mipmaps
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &createInfo, nullptr,
							  &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
		std::cout << "Created image view " << i << std::endl;
	}
}


VkSurfaceFormatKHR vk_SwapChain::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR> &availableFormats) {
	for (const auto &availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	// return the first available format if the preferred one is not found
	return availableFormats[0];
}

VkPresentModeKHR vk_SwapChain::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR> &availablePresentModes) {
	for (const auto &availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vk_SwapChain::chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR &capabilities) {
	if (capabilities.currentExtent.width !=
		std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(instance.getWindow_reference().getWindow(),
							   &width, &height);

		VkExtent2D actualExtent = {static_cast<uint32_t>(width),
								   static_cast<uint32_t>(height)};

		actualExtent.width =
			std::clamp(actualExtent.width, capabilities.minImageExtent.width,
					   capabilities.maxImageExtent.width);
		actualExtent.height =
			std::clamp(actualExtent.height, capabilities.minImageExtent.height,
					   capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void vk_SwapChain::destroySwapChainImageViews() {
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	swapChainImageViews.clear();
}

void vk_SwapChain::destroySwapChain() {
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}
}  // namespace vk