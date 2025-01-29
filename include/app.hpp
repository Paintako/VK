#pragma once

#include "devices.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

// std
#include <memory>
#include <vector>

namespace vk {
class App {
public:
	App();
	~App();

	// Not copyable or movable
	App(const App &) = delete;
	void operator=(const App &) = delete;

	void run();

private:
	/* Helper functions */
	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffers();
	void drawFrame();

	/* Variables & attributes */
	const int WIDTH = 800;
	const int HEIGHT = 600;
	std::string title = "Vulkan Application";

	/* Vulkan Objects */
	Window window{WIDTH, HEIGHT, title};
	Device devices{window};
	SwapChain swapChain{devices, window.getExtent()};
	std::unique_ptr<Pipeline>
		pipeline;  // Unique pointer to pipeline, to avoid memory leaks
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
};
}  // namespace vk