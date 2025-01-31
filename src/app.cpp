#include "app.hpp"

// std
#include <array>
#include <iostream>
#include <random>
#include <stdexcept>

namespace vk {
App::App()
	: WIDTH(800),
	  HEIGHT(600),
	  title("Vulkan Application"),
	  window(WIDTH, HEIGHT, title) {
	loadModel();
	createPipelineLayout();
	createPipeline();
	createCommandBuffers();
}

App::~App() {
	// Destructor
	vkDestroyPipelineLayout(devices.device(), pipelineLayout, nullptr);
}

void App::run() {
	while (!window.shouldClose()) {
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(devices.device());
}

void App::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;	// Optional
	pipelineLayoutInfo.pSetLayouts =
		nullptr;  // SetLayouts are for descriptor sets, used to pass data to
				  // shaders such as uniform data
	pipelineLayoutInfo.pushConstantRangeCount =
		0;	// Push constants are another
			// way to pass data to shaders, but are limited in size
	pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional

	if (vkCreatePipelineLayout(devices.device(), &pipelineLayoutInfo, nullptr,
							   &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void App::createPipeline() {
	auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(
		swapChain.width(),
		swapChain.height());  // Pipeline width and height must match swap chain
							  // configuration
	pipelineConfig.renderPass =
		swapChain.getRenderPass();	// renderpass describes the framebuffer
									// attachments that will be used while
									// rendering, for example color, depth, and
									// stencil attachments in a frame buffer
	pipelineConfig.pipelineLayout = pipelineLayout;

	pipeline =
		std::make_unique<Pipeline>(devices, "shaders/shader.vert.spv",
								   "shaders/shader.frag.spv", pipelineConfig);
}

void App::createCommandBuffers() {
	commandBuffers.resize(swapChain.imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool =
		devices.getCommandPool();  // Reduce the number of
								   // command buffer allocations through the
								   // command pool
	allocInfo.level =
		VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // Can be submitted to a queue for
										  // execution, but cannot be called
										  // from other command buffers
	allocInfo.commandBufferCount = static_cast<uint32_t>(
		commandBuffers.size());	 // Equal to the number of swap chain images

	if (vkAllocateCommandBuffers(devices.device(), &allocInfo,
								 commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	// Record command buffers
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo
			beginInfo{};  // Always required when recording command buffers
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error(
				"failed to begin recording command buffer!");
		}

		// Begin render pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain.getRenderPass();
		renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

		// Specify clear values (initial) for the color and depth attachments
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount =
			static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
							 VK_SUBPASS_CONTENTS_INLINE);

		pipeline->bind(commandBuffers[i]);
		model->bind(commandBuffers[i]);
		model->draw(commandBuffers[i]);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
};

void App::drawFrame() {
	uint32_t imageIndex;
	auto result = swapChain.acquireNextImage(&imageIndex);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex],
											&imageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to submit command buffer!");
	}
};


void App::loadModel() {
	// Load model data
	// Random vertices for now
	std::vector<Model::Vertex> vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	};
	// std::vector<Model::Vertex> vertices{};

	// std::random_device rd;
	// std::mt19937 gen(rd());
	// std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	// for (size_t i = 0; i < 3; i++) {
	// 	for (size_t j = 0; j < 3; j++) {
	// 		float x = dist(gen);
	// 		float y = dist(gen);
	// 		vertices.push_back({{x, y}});
	// 		std::cout << x << ", " << y << std::endl;
	// 	}
	// }

	model = std::make_unique<Model>(devices, vertices);
}
}  // namespace vk