#include "app.hpp"

#include <chrono>

namespace vk {
App::App() {
	// Constructor implementation
	std::cout << "App constructor called" << std::endl;
	initVulkan();

	vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
				{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};

	indices = {0, 1, 2, 2, 3, 0};

	vertex.createVertexBuffer(vertices);
	vertex.createIndexBuffer(indices);

	uniformBuffer.createUniformBuffers(MAX_FRAMES_IN_FLIGHT);
	descriptor.createDescriptorSets(uniformBuffer.getUniformBuffers(),
									uniformBuffer.getDescriptorSetLayout(),
									image.getTextureImageView(),
									image.getTextureSampler());
}

App::~App() {
	// Destructor implementation
	std::cout << "App destructor called" << std::endl;
	cleanup();
	std::cout << "App destructor finished" << std::endl;
}
void App::run() {
	mainLoop();
	// cleanup();
}

static void framebufferResizeCallback(GLFWwindow *window,
									  int width,
									  int height) {
	std::cout << "Framebuffer resized!" << std::endl;
	void *ptr = glfwGetWindowUserPointer(window);
	if (!ptr) {
		std::cerr << "Window user pointer is null!" << std::endl;
		return;
	}
	auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
	std::cout << "Framebuffer resized callback called!" << std::endl;
	std::cout << app->framebufferResized << std::endl;
}

void App::initVulkan() {
	// Vulkan initialization code
	std::cout << "Vulkan initialized!" << std::endl;
	glfwSetWindowUserPointer(window.getWindow(), this);
	glfwSetFramebufferSizeCallback(window.getWindow(),
								   framebufferResizeCallback);
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
	// Calculate fps
	static auto start_time = std::chrono::high_resolution_clock::now();
	static uint16_t frame_count = 0;  // up to 65535 frames per second

	// Update the uniform buffer
	uniformBuffer.updateUniformBuffer(currentFrame,
									  swapChain.getSwapChainExtent());

	// Wait for the fence to be signaled, which indicates that the GPU has
	// finished rendering the previous frame
	vkWaitForFences(device.getLogicalDevice(), 1,
					&sync_object.getInFlightFence(currentFrame), VK_TRUE,
					UINT64_MAX);

	// Acquire the next image from the swap chain
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		device.getLogicalDevice(), swapChain.getSwapChain(), UINT64_MAX,
		sync_object.getImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE,
		&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		std::cout << "Swap chain out of date!" << std::endl;
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Reset the fence to the unsignaled state
	vkResetFences(device.getLogicalDevice(), 1,
				  &sync_object.getInFlightFence(currentFrame));

	// Reset the command buffer before recording
	vkResetCommandBuffer(commandBuffers.getCommandBuffer(currentFrame), 0);
	VkCommandBuffer commandBuffer =
		commandBuffers.getCommandBuffer(currentFrame);
	// Record the command buffer
	commandBuffers.recordCommandBuffer(
		commandBuffers.getCommandBuffer(currentFrame),
		renderPass.getRenderPass(), graphicsPipeline.getGraphicsPipeline(),
		frameBuffer.getswapChainFrameBuffer()[imageIndex],
		swapChain.getSwapChainExtent(), vertex.getVertexBuffer(),
		vertex.getIndexBuffer(), vertices, indices,
		pipelineLayout.getPipelineLayout(),
		descriptor.getDescriptorSets()[currentFrame]);

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

	result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
		framebufferResized) {
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			std::cout << "Swap chain out of date!" << std::endl;
		} else if (result == VK_SUBOPTIMAL_KHR) {
			// The swap chain is suboptimal, but we can still use it
			std::cout << "Swap chain suboptimal!" << std::endl;
		} else {
			std::cout << "Framebuffer resized!" << std::endl;
		}
		framebufferResized = false;
		recreateSwapChain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
	currentFrame = (currentFrame + 1) % max_frames_in_flight;

	// Calculate fps
	frame_count++;
	auto end_time = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time)
			.count() >= 1) {
		std::cout << "FPS: " << frame_count << std::endl;
		frame_count = 0;
		start_time = end_time;
	}
}

void App::recreateSwapChain() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(window.getWindow(), &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.getWindow(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device.getLogicalDevice());

	cleanupSwapChain();

	swapChain.createSwapChain();
	swapChain.createSwapChainImageViews();
	frameBuffer.createFrameBuffer(swapChain.getSwapChainImageViews());
}

void App::cleanupSwapChain() {
	// Cleanup all the resources related to the swap chain
	frameBuffer.cleanupFrameBuffer();
	swapChain.destroySwapChainImageViews();
	swapChain.destroySwapChain();
}

void App::cleanup() {
	cleanupSwapChain();

	graphicsPipeline.cleanupGraphicsPipeline();
	pipelineLayout.cleanupPipelineLayout();
	renderPass.cleanupRenderPass();

	uniformBuffer.cleanupBuffers();
	vertex.cleanupBuffers();

	image.cleanup();

	descriptor.cleanupDescriptorPool();
	uniformBuffer.cleanupDescriptorSetLayout();

	sync_object.cleanupSyncObjects();

	commandBuffers.cleanupCommandPool();

	device.destroyLogicalDevice();

	vkDestroySurfaceKHR(instance.getInstance(), instance.getSurface(), nullptr);
	vkDestroyInstance(instance.getInstance(), nullptr);

	glfwDestroyWindow(window.getWindow());

	glfwTerminate();
	std::cout << "App cleanup finished!" << std::endl;
}
}  // namespace vk