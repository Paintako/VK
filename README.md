# VK
## Resource managment
Writing code in RAII format to perform automatic resource management 

## Repository structure
We're simply spliting vulkan objects to different files, including:
1. Instance
2. Surface
3. Devices
4. RenderPass
5. Pipeline
6. CommandBuffers & CommandPools

And using App to run our application

## Vulkan overview
### Insance & Devices
Overall initialization pseudo code:
vkCreateInstance() → vkEnumeratePhysicalDevices() → vkCreateDevice()

The VkDevice is your main handle and it represents a logical connection - i.e. 'I am running Vulkan on this GPU'. 

## Reference
[Render doc: Vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)