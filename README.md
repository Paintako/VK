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

### Framebuffer
Framebuffer: A framebuffer object references **all of the VkImageView objects that represent the attachments**. 
Relation between VkImage, VkImageView, framebuffer, framebuffer attachments.

* VkImage: represents an **image resource** in Vulkan, used for **texture**, **render target**, **depth buffer**.
* VkImageView: **wrapper around a `VkImage`**, describes how the `VkImage` should be access during rendering. Provides a view of `VkImage` for different usage such as `texture sampling` or `render output`.
* FrameBuffer: Contains sets of `attachments(附件)`, specified the images to be used in `renderpass`
* Attahments: Reference to `VkImageview` used in a `framebuffer` during renderpass

* VkImage = 圖片本體，表示實際的 image 資料（也就是 GPU 記憶體中的圖像），例如顏色圖層、深度圖層、texture 等。
* VkImageView = 看圖方式（你要看哪一層、哪個格式），表示「如何解讀」某個 VkImage。
* RenderPass = 定義需要幾個 attachment（顏色/深度...）
* Attachments
    * 是 RenderPass 規格中所定義的 attachment slot（比如 color0, depth），而在實際使用時，這些 slot 會對應到具體的 VkImageView。
    * 也就是說：attachment 是一個抽象概念（類似「第0個 color 輸出目標」），實際對應的圖像由 Framebuffer 提供的 ImageView 決定。
* Framebuffer
    * 是一個容器，用來把 renderpass 需要的 attachment slots（color, depth 等）「具體綁定」到某些 VkImageView 上。
    * 就像是在告訴 Vulkan：這個 renderpass 的 color0 用這張圖、depth 用這張圖。

### CommandBuffer
To record a `CommandBuffer`, we need to:
1. Set `VkCommandBufferBeginInfo`, which tells driver which kind of commandbuffer it is (primiary/secondary)
2. `vkCmdBeginRenderPass`
3. `vkCmdBindPipeline`
4. draw calls
5. `vkCmdEndRenderPass`
6. `vkEndCommandBuffer`

### Synchornization
Outline of a frame during rendering & presentation
1. Acquire an image from the swap chain
2. Execute commands that draw onto the acquired image
3. Present that image to the screen for presentation, returning it to the swapchain

Each of these events is set in motion using a single function call, but are **all executed asynchronously**. 

#### Semaphores
A semaphore is used to **add order between queue operations**. 
Examples of queues are the graphics queue and the presentation queue. 
Semaphores are used both to order work inside the same queue and between different queues.

```c
VkCommandBuffer A, B = ... // record command buffers
VkSemaphore S = ... // create a semaphore

// enqueue A, signal S when done - starts executing immediately
vkQueueSubmit(work: A, signal: S, wait: None)

// enqueue B, wait on S to start
vkQueueSubmit(work: B, signal: None, wait: S)
```

Note that `vkQueueSubmit` returns **immediately**, the waiting happens on **GPU**.

#### Fences
To make the `GPU` wait --> `semaphore`
Make CPU(host) wait --> `Fence`

For example: taking a screenshot.
Say we have already done the necessary work on the GPU. Now need to transfer the image from the GPU over to the host and then save the memory to a file. 
We have command buffer A which executes the transfer and fence F. 
We submit command buffer A with fence F, then immediately tell the host to wait for F to signal.
This causes the host to block until command buffer A finishes execution. Thus we are safe to let the host save the file to disk, as the memory transfer has completed.

```c
VkCommandBuffer A = ... // record command buffer with the transfer
VkFence F = ... // create the fence

// enqueue A, start work immediately, signal F when done
vkQueueSubmit(work: A, fence: F)

vkWaitForFence(F) // blocks execution until A has finished executing

save_screenshot_to_disk() // can't run until the transfer has finished
```

#### Real case
1. `vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);`
這段會向 swapChain 要一張 image, 並且 `imageAvailableSemaphore` 是一個 semaphore, 還沒跟 swapChain 要到圖片前他會 wait 住, 直到要到之後才會 set
2. record draw calls into a commandBuffer
3. 準備 submit commandBuffer
    * `VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};`
    * `VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};`
        * `waitSemaphores`: 我要在開始執行前要等哪些 semaphore, 那這邊要等到要到 image 後才開始執行
        * `waitStages`: 當 `waitSepmaphores` 尚未被 set 前, gpu 最多能執行到的 stage
```c
Vertex Input → Vertex Shader → Fragment Shader → Color Attachment Output
```
這裡就算還沒等到 `imageAvailableSemaphore`, GPU 也會把工作做到 Fragment shader 然後卡在這裡, 直到 `imageAvailableSemaphore` 被 set 他才會繼續往下工作
4. vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence)
    * inFlightFence 是一個 opntional 的 parameter, 可以透過這個 Fence 來告訴 CPU 該 command buffer 可以被安全的使用
    * CPU 會等這個 Fence 被 set 之後才會繼續的錄製 & submit

#### Subpass
Vulkan 在 `Subpass` 間，會自動對 attachment 做 **image layout transition**
`RenderPasss` 本身就是一個 `Subpass`, 所以他的開始結束自然也有 transition
e.g., `VK_IMAGE_LAYOUT_UNDEFINED` → `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`

```bash
transition           transition              transition
          subpass...             subpass2...
```

但這個有一個問題，這個 transtion 假設是發生在 `graphic pipeline` 的 top(TOP_OF_PIPE)，而並不是在真正取得 image 後 
(透過 vkAcquireNextImageKHR 並等待 imageAvailableSemaphore, 還沒拿到之前其實 GPU 早就可以提前工作了)
所以可能長這樣:
```bash
[預設 layout transition 發生了]（Vulkan 以為 image 準備好了）
              ↓
[你才透過 imageAvailableSemaphore 確認 image 真正可用]
```
這樣可能導致 undefined behavior

解法是:
1. 
```cpp
waitStages[] = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
```
這種做法 pipeline stage (layout transtion) 會等到 `imageAvailableSemaphore` signal，也就是說 GPU 在等到 signal set 前不會做任何工作 

2. 
```cpp
waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
```
這種做法是讓 layout transition 晚點做

### Frame in flights
Flow:
1. CPU 準備 commands --> Submit to driver --> GPU
2. GPU Parallel processing, 但是 GPU 通常會比 CPU 慢一個 frame
3. 當上一個 frame 跑完之後 (GPU queue 空了) 之後才開始準備下一個 frame 指令的話, 那在等待GPU完成時 CPU 就會空轉
4. 為了避免 CPU 空轉 --> frame in flights --> 需要為每一張 Frame 準備自己的:
    * command buffer
    * semaphore (for GPU sync)
    * fence (for C <--> G sync)

## Vertex buffers
`Shader`: codes that runs on GPU
`Vertex shader`: Takes chunks of input points and figure out where they should appear on screen
`Fragment shader`: Takes the output of vertex shader (varying) and colors the surface

步驟:
1. 準備 vertex data from CPU (not from hardcoded vertex shader), 設定:     
    a. `VkVertexInputBindingDescription` **資料來源(從哪個 vertex buffer拿)**
    b. `VkVertexInputAttributeDescription` **指定 shader 端的接收端 (in) 以及資料從哪個 binding 拿(from 1.)**
2. create buffer, 設定:
    a. create buffer object, 其中包含 buffer 屬性, size 等等
    b. get memory, 設定 heap 屬性並且詢問 phyisical device 是否有指定屬性的 heap
    c. 若有 --> map vertex buffer

### Prepare vertex data
```cpp
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};
```

GLM conveniently provides us with C++ types that exactly match the vector types used in the shader language.
```cpp
const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};
```
### Binding descriptions
Vulkan 中的 binding:
在 Vulkan 的 vertex input 階段，我們會提供一個或多個 vertex buffer（頂點緩衝區），而每個 buffer 對應一個 binding slot
```cpp
VkVertexInputBindingDescription bindingDescription{};
bindingDescription.binding = 0; // binding index
bindingDescription.stride = sizeof(Vertex); // 每個 vertex 的大小
bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // 每個頂點取一次
```
* 我們要從 binding 0（也就是 vertex buffer 的第一個綁定點）讀資料
* 每筆資料大小為 sizeof(Vertex)
* 每個頂點讀一次資料（而不是 per-instance）

因為一個 shader 可能需要從多個 vertex buffer 拿資料, e.g.,
* binding 0: 頂點位置
* binding 1: 法向量
* binding 2: 顏色
...
這些可能來自不同的 buffer，就要指定對應的 binding index

而每一個 binding 可以有多種不同的 `attribute`, 如 position, color, normal 都放在一個 struct 裡，對應同一個 buffer。
每個 attribute 都要指定：
* 哪個 binding（哪個 buffer）
* shader 的 location（對應 layout(location = x)）
* 資料格式（float2、float3）
* 偏移量（offset from vertex start）

對應到 shader
```glsl
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
```

相對應的 attribute description
```cpp
attributeDescriptions[0].location = 0; // 對應 inPosition
attributeDescriptions[0].binding = 0;  // 從 binding 0 抓
attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
attributeDescriptions[0].offset = offsetof(Vertex, pos);

attributeDescriptions[1].location = 1; // 對應 inColor
attributeDescriptions[1].binding = 0;  // 一樣從 binding 0 抓
attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
attributeDescriptions[1].offset = offsetof(Vertex, color);
```

### Allcocate buffer & Mapping
設定完如何拿 vertex from vector 後，接著該開始 create buffer 以及把 vector map 過去
```cpp
VkBufferCreateInfo bufferInfo{};
bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
bufferInfo.size = sizeof(vertices[0]) * vertices.size();

bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
```

設定大小/usage/sharingMode 等等
有了 buffer object 之後，要去設定這塊 buffer 的 memory
```cpp
VkMemoryRequirements memRequirements;
vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
```

這會得到這塊 buffer object 的 memory 屬性(buffer or image) & size
有了屬性之後去 query physicial device 有沒有我們要的 attribute
```cpp
VkMemoryAllocateInfo allocInfo{};
allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
allocInfo.allocationSize = memRequirements.size;
allocInfo.memoryTypeIndex =
    findMemoryType(memRequirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    physicalDevice);
```

這裡要 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 以及 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 的目的在於我們要把 CPU 端的 vector data 寫入 GPU memory
VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 的目的在於我們不需要額外做 flush 的操作 (driver 可能不會買上進行 copy 動作, COHERENT 就不用額外 handle, 沒這個屬性的 heap 就要手動 map)
    * Call vkFlushMappedMemoryRanges after writing to the mapped memory
    * Call vkInvalidateMappedMemoryRanges before reading from the mapped memory

## Staging buffers
使用 `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` 是為了從 host memcpy 過去給 GPU, 也就是可以:
```cpp
vkMapMemory(...)
memcpy(...)
vkUnmapMemory(...)
```

但這裡的 heap 對 GPU 來說並不見得是最快可 access 的 memory, 可以用 copyBuffer 的方式把 data 從 CPU visiable copy 到 GPU visiable only 的 heap 上


## Reference
[Render doc: Vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)
[Coordinates in shader](https://ithelp.ithome.com.tw/articles/10245073)
[homogeneous coorindates](https://flyhead.medium.com/%E7%82%BA%E4%BB%80%E9%BA%BC%E9%9C%80%E8%A6%81%E9%BD%8A%E6%AC%A1%E5%BA%A7%E6%A8%99-homogeneous-coordinate-bd86356f67b1)