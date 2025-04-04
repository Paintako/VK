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

### SwapChain & ImageView
#### SwapChain
General purpose of the SwapChain is to **synchronize the presentation of images with the refresh rate of the screen**.
The SwapChain is essentially a queue of images that are waiting to be presented to the screen.
1. Check SwapChain support (during physical device selection, some physical device might not support...)
    * We query physical device and get attributes from this swap chain
    * e.g., 
        * VkSurfaceCapabilitiesKHR (capabilities of physical device and given surface (e.g., windows/linux))
        * VkSurfaceFormatKHR
        * VkPresentModeKHR
2. Choose detail settings from 1. for SwapChain creation
    * Surface format (color depth)
    * Presentation mode (conditions for "swapping" images to the screen, e.g., FIFO)
    * Swap extent (resolution of images in swap chain)
        * We could set `imageUsage` bit field specifies what kind of operations 
            we'll use the images in the swap chain for.
3. Use a vector to handle `VkImage`s which were created via SwapChain creation

#### ImageView
To use any VkImage, including those in the SwapChain, in the render pipeline we have to create a VkImageView object. 
An image view is quite literally a view into an image. 
It describes how to access the image and which part of the image to access, for example if it should be treated as a 2D texture depth texture without any mipmapping levels.

## Reference
[Render doc: Vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)