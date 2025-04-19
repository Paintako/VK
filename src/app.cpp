#include "app.hpp"

namespace vk {
App::App() {
	// Constructor implementation
	std::cout << "App constructor called" << std::endl;
}

App::~App() {
	// Destructor implementation
	std::cout << "App destructor called" << std::endl;
	cleanup();
	std::cout << "App destructor finished" << std::endl;
}
void App::run() {
	initVulkan();
	mainLoop();
	// cleanup();
}
void App::initVulkan() {
	// Vulkan initialization code
	std::cout << "Vulkan initialized!" << std::endl;
}
void App::mainLoop() {
	// Main loop code
	while (!glfwWindowShouldClose(window.getWindow())) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(device.getLogicalDevice());
	std::cout << "Main loop finished!" << std::endl;
}

void App::drawFrame() {
	// Drawing code
	std::cout << "Drawing frame..." << std::endl;

	// Wait for the fence to be signaled, which indicates that the GPU has
	// finished rendering the previous frame
	vkWaitForFences(device.getLogicalDevice(), 1,
					&sync_object.getInFlightFence(currentFrame), VK_TRUE,
					UINT64_MAX);

	// Reset the fence to the unsignaled state
	vkResetFences(device.getLogicalDevice(), 1,
				  &sync_object.getInFlightFence(currentFrame));

	// Acquire the next image from the swap chain
	uint32_t imageIndex;
	vkAcquireNextImageKHR(device.getLogicalDevice(), swapChain.getSwapChain(),
						  UINT64_MAX,
						  sync_object.getImageAvailableSemaphore(currentFrame),
						  VK_NULL_HANDLE, &imageIndex);

	// Reset the command buffer before recording
	vkResetCommandBuffer(commandBuffers.getCommandBuffer(currentFrame), 0);
	VkCommandBuffer commandBuffer =
		commandBuffers.getCommandBuffer(currentFrame);
	// Record the command buffer
	commandBuffers.recordCommandBuffer(
		commandBuffers.getCommandBuffer(currentFrame),
		renderPass.getRenderPass(), graphicsPipeline.getGraphicsPipeline(),
		frameBuffer.getswapChainFrameBuffer()[imageIndex],
		swapChain.getSwapChainExtent());

	// Submit the command buffer to the graphics queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {
		sync_object.getImageAvailableSemaphore(currentFrame)};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers.getCommandBuffer(currentFrame);

	VkSemaphore signalSemaphores[] = {
		sync_object.getRenderFinishedSemaphore(currentFrame)};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo,
					  sync_object.getInFlightFence(currentFrame)) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Present the image to the swap chain
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapChain.getSwapChain()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);
	std::cout << "Frame drawn!" << std::endl;
	currentFrame = (currentFrame + 1) % max_frames_in_flight;
}

void App::cleanup() {
	std::cout << "Cleaning up..." << std::endl;
	sync_object.cleanupSyncObjects();
	std::cout << "Sync objects destroyed!" << std::endl;
	commandBuffers.cleanupCommandPool();
	std::cout << "Command pool destroyed!" << std::endl;
	frameBuffer.cleanupFrameBuffer();
	std::cout << "Frame buffer destroyed!" << std::endl;
	graphicsPipeline.cleanupGraphicsPipeline();
	std::cout << "Graphics pipeline destroyed!" << std::endl;
	pipelineLayout.cleanupPipelineLayout();
	std::cout << "Pipeline layout destroyed!" << std::endl;
	renderPass.cleanupRenderPass();
	std::cout << "Render pass destroyed!" << std::endl;
	swapChain.destroySwapChainImageViews();
	std::cout << "Swap chain image views destroyed!" << std::endl;
	swapChain.destroySwapChain();
	std::cout << "Swap chain destroyed!" << std::endl;
	device.destroyLogicalDevice();
	std::cout << "Logical device destroyed!" << std::endl;
	vkDestroySurfaceKHR(instance.getInstance(), instance.getSurface(), nullptr);
	std::cout << "Surface destroyed!" << std::endl;
	vkDestroyInstance(instance.getInstance(), nullptr);
	std::cout << "Instance destroyed!" << std::endl;
	glfwDestroyWindow(window.getWindow());
	std::cout << "Window destroyed!" << std::endl;
	glfwTerminate();
	std::cout << "GLFW terminated!" << std::endl;
	std::cout << "App cleanup finished!" << std::endl;
}
}  // namespace vk