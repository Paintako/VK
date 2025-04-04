#pragma once

#include "vk_Device.hpp"

namespace vk {
class vk_SwapChain {
public:
	vk_SwapChain(vk_Device &device, vk_Instance &instance);
	~vk_SwapChain() = default;

	vk_SwapChain(const vk_SwapChain &) = delete;
	vk_SwapChain &operator=(const vk_SwapChain &) = delete;

	void createSwapChain();

	void destroySwapChainImageViews();
	void destroySwapChain();

	VkSwapchainKHR &getSwapChain() { return swapChain; }
	VkFormat &getSwapChainImageFormat() { return swapChainImageFormat; }
	VkExtent2D &getSwapChainExtent() { return swapChainExtent; }

private:
	vk_Instance &instance;
	vk_Device &device;

	VkDevice logicalDevice;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;

	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	void createSwapChainImageViews();

	/* Helper functions */
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
};
}  // namespace vk