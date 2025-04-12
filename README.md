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

### Pipeline
#### 1. Create shader module
##### GLSL
Using `GLSL` to write `shading language`, programs written in it have a `main` function that is invoked for every `object`.
`GLSL` uses global variables to handle input and output.
##### Vertex shader
Vertex shader processes `each incoming vertex`. 

**Input**: It takes it's attributes, such as `world position`, `color`, `normal` and `texture coordinates`.
**Output**: Clip coordinates and attributes need to passed on to fragment shader (such as color, texture coordinates).

Note:
A clip coordinate is a `four dimensional vector` from the `vertex shader` that is subsequently **turned into a normalized device coordinate** by **dividing the whole vector by its last component**. These normalized device coordinates are homogeneous coordinates that **map the framebuffer to a [-1, 1] by [-1, 1] coordinate system**.

4D vector in clip space:
```glsl
gl_Position = MVP * vec4(position, 1.0);
```
This gives you a 4D vector
```ini
gl_Position = (x, y, z, w)
```
The `w` stands for `homogenous divide` (`w` often ends up being related to the distance between the camera and the vertex.)
It lets us represent perspective projection (like how things get smaller in the distance).
It also allows for translation and more flexible transformations using matrix multiplication.

Imagine you take a photo with a perspective camera. Distant objects appear smaller.

Without dividing by w, all your objects would look like they're in orthographic view (no depth shrinking).
With w, you get perspective — objects farther from the camera appear smaller.

```glsl
#version 450

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
```
* positions[3]: an array that holds 3 `2D postions` --> a triangle
* gl_VertexIndex: built-in vairable automatically set by the `GPU`
    * Indicates witch `vertex` is being processed
* gl_Position: variable that outputs the final postion in `clip space`

The word "The main function is invoked for every vertex" means:
`The vertex shader is run once per vertex by GPU`
* For each vetex you're drawing, the GPU calls the `main()` function in the vertex shader
* It's like loop, but done by `SIMD`(in parallel) on GPU

So in Pseudo-code it looks like this
```c
// Pseudo-code of what GPU does internally
for (int i = 0; i < 3; i++) {
    gl_VertexIndex = i;
    main(); // Run vertex shader for vertex i
}
```

When drawing in `Vulkan`, we need to specify how much vertices to draw
```vulkan
vkCmdDraw(commandBuffer, 3, ...);  // In Vulkan
// or
glDrawArrays(GL_TRIANGLES, 0, 3);  // In OpenGL
```
So the GPU:
* Runs the vertex shader exactly 3 times (because you told it to draw 3 vertices).
* Each time, it sets gl_VertexIndex to 0, 1, and 2, and runs the main() function in parallel.

##### Fragment shader
The triangle that is formed by the positions from the `vertex shader` fills an area on the screen with **fragments**. 
The fragment shader is invoked on these fragments to **produce a color and depth for the framebuffer** (or  framebuffers).

```glsl
#version 450

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
```

The `main` function is called for **every fragment**.
Unlike vertex shader, there's no built-in vairable like `gl_Position`(it's build per vetex) to output put a color to current fragment.
You have to specify your own **output** for each **frambuffer** where the `layout(location=0)` specifies the **index of the framebuffer**.
The `outColor` variable (custom) is a curstom variable, which is the **output variable** for this fragment shader, it's linked to a **framebuffer attachment** via `layout(location=x)` 

Note: 
**framebuffer attachment** is a special data buffer that is **attached to a framebuffer** in Vulkan, which holds datas such as:
* Color data (color ouput of the framgent shader)
* Depth data (determine how clos a pixel is to the camera)
* Stencil data (for performing operations like masking areas during rendering)

So if there's 3 framebuffers, we can specify different framebuffer to put framebuffer attachment.
```glsl
layout(location = 0) out vec4 outColor0; // put outColor0 in framebuffer0 (location = 0)
layout(location = 1) out vec4 outColor1; // put outColor1 in framebuffer1 (location = 1)
layout(location = 2) out vec4 outColor2; // put outColor2 in framebuffer2 (location = 2)
```
#### 2. Set Fixed stages
* Dynamic state
* Vertex input
* Input assembly
* Viewports and scissors
* Rasterizer
* Multisampling
* Color blending
* Pipeline layout
* RenderPass

##### Pipeline layout
You can use `uniform` values in shaders, which are `globals` similar to dynamic state variables that **can be changed at drawing time** to alter(改變) the behavior of your shaders without having to recreate them. 
They are commonly used to pass the **transformation matrix** to the vertex shader, or to **create texture samplers in the fragment shader**.

These uniform values need to be specified during pipeline creation by creating a VkPipelineLayout object.

##### RenderPass
We need to tell Vulkan about the framebuffer attachments that will be used while rendering.
We need to specify 
* how many color and depth buffers there will be
* how many samples to use for each of them
* how their contents should be handled throughout the rendering operations. 
All of this information is wrapped in a render pass object.

```vulkan
VkAttachmentDescription colorAttachment{};
colorAttachment.format = swapChainImageFormat; // specifies the colorAttachmemt format is same as swapChain
colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
```
* The `VkAttachmentDescription` should fit the framebuffer format, in the case of swapChain framebuffers, we pass the swapChain format to match it.
* Textures and framebuffers are represented by `VKImage` with certial `pixel formats`, layouts of diffetent pixels may differ based on what you're trying to do with an image.
    * The `initialLayout` specifies which layout the image will have before the **render pass begins**. 
    * The `finalLayout` specifies the layout to automatically transition to when the **render pass finishes**.

##### Subpass


## Reference
[Render doc: Vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)
[Coordinates in shader](https://ithelp.ithome.com.tw/articles/10245073)
[homogeneous coorindates](https://flyhead.medium.com/%E7%82%BA%E4%BB%80%E9%BA%BC%E9%9C%80%E8%A6%81%E9%BD%8A%E6%AC%A1%E5%BA%A7%E6%A8%99-homogeneous-coordinate-bd86356f67b1)