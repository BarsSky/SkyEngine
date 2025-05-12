#include <cstring>
#include <SkyEngine/config/config.h>
#include <SkyEngine/vk_sky_device.hpp>
#include "tools.hpp"
#include "extension/tinygltf/stb_image.h"
#include "window_impl.h"

#ifdef GLFW_LIB_ENABLE
void VulkanDevice::shouldCloseCallback(GLFWwindow *window) {
    auto app = reinterpret_cast<VulkanDevice *>(glfwGetWindowUserPointer(window));
    app->ready_to_close = true;
}

void VulkanDevice::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<VulkanDevice *>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}
#endif
VkCommandBuffer VulkanDevice::beginSingleTimeCommands(VkCommandBufferLevel level, VkCommandPool pool, bool begin)
{
    VkCommandBufferAllocateInfo allocInfo = initializers::commandBufferAllocateInfo(pool, level, 1);

    VkCommandBuffer commandBuffer;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer));

    if (begin)
    {
        VkCommandBufferBeginInfo beginInfo = initializers::commandBufferBeginInfo();
        VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    }

    return commandBuffer;
}

VkCommandBuffer VulkanDevice::beginSingleTimeCommands(VkCommandBufferLevel level, bool begin)
{
    return beginSingleTimeCommands(level, commandPool, begin);
}

void VulkanDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, queue);
}

void VulkanDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free)
{
    if (commandBuffer == VK_NULL_HANDLE)
        return;

    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo = initializers::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkFenceCreateInfo fenceInfo = initializers::fenceCreateInfo(VK_FLAGS_NONE);
    VkFence fence;
    VK_CHECK_RESULT(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence));
    // Submit to the queue
    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK_RESULT(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));
    vkDestroyFence(logicalDevice, fence, nullptr);
    if (free)
    {
        vkFreeCommandBuffers(logicalDevice, pool, 1, &commandBuffer);
    }
}

void VulkanDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue, bool free)
{
    return endSingleTimeCommands(commandBuffer, queue, commandPool, free);
}

VulkanDevice::VulkanDevice()
{
    u_ptr_window = std::make_unique<Window_Impl>();
}

bool VulkanDevice::QueueFamilyProperties()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount > 0);
    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    return true;
}

VulkanDevice::~VulkanDevice()
{
}

// void VulkanDevice::createSurface() {
//     //Делаем обертку для подключения библиотеки QT
// #ifdef QT_LIB_ENABLE
//     VkResult err = VK_SUCCESS;
////    auto window = crossWindow.getWindow();
//
////    surface = q_instance.surfaceForWindow(window);
////TODO: create for windows too
//
// #if defined(VK_USE_PLATFORM_WAYLAND_KHR)
//    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
//	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
//	surfaceCreateInfo.display = display;
//	surfaceCreateInfo.surface = window;
//	err = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
// #elif defined(VK_USE_PLATFORM_XCB_KHR)
//    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
//	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
//	surfaceCreateInfo.connection = crossWindow.m_connection;//reinterpret_cast<xcb_connection_t*>(wid);
//	surfaceCreateInfo.window = crossWindow.m_window;
//	err = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
//    if(err != VK_SUCCESS)
//        throw std::runtime_error("failed to create window surface!" + std::to_string(err));
// #elif defined(VK_USE_PLATFORM_WIN32_KHR)
//    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
//    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
// #endif
// #else
//    if (glfwCreateWindowSurface(instance, crossWindow.getWindow(), nullptr, &surface) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create window surface!");
//    }
// #endif
//}

uint32_t
VulkanDevice::getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound) const
{
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                if (memTypeFound)
                {
                    *memTypeFound = true;
                }
                return i;
            }
        }
        typeBits >>= 1;
    }

    if (memTypeFound)
    {
        *memTypeFound = false;
        return 0;
    }
    else
    {
        throw std::runtime_error("Could not find a matching memory type");
    }
}

void VulkanDevice::pickPhysicalDevice()
{
    VkResult err;
    // Physical device
    uint32_t gpuCount = 0;
    // Get number of available physical devices
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
    if (gpuCount == 0)
    {
        tools::exitFatal("No device with Vulkan support found", -1);
        return;
    }
    // Enumerate devices
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
    if (err)
    {
        tools::exitFatal("Could not enumerate physical devices : \n" + tools::errorString(err), err);
        return;
    }

    // GPU selection

    // Select physical device to be used for the Vulkan example
    // Defaults to the first device unless specified by command line
    uint32_t selectedDevice = 0;

    for (const auto &device : physicalDevices)
    {
        if (isDeviceSuitable(device))
        {
            physicalDevice = device;
            msaaSamples = getMaxUsableSampleCount();
            break;
        }
    }

    // physicalDevice = physicalDevices[selectedDevice];
    // msaaSamples = getMaxUsableSampleCount();

    QueueFamilyProperties();
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return extensionsSupported && swapChainAdequate; // && supportedFeatures.samplerAnisotropy;
}

SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

uint32_t VulkanDevice::getQueueFamilyIndex(VkQueueFlags queueFlags) const
{
    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if ((queueFlags & VK_QUEUE_COMPUTE_BIT) == queueFlags)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                return i;
            }
        }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics and compute
    if ((queueFlags & VK_QUEUE_TRANSFER_BIT) == queueFlags)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
            {
                return i;
            }
        }
    }

    // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
    {
        if ((queueFamilyProperties[i].queueFlags & queueFlags) == queueFlags)
        {
            return i;
        }
    }

    throw std::runtime_error("Could not find a matching queue family index");
}

VkSampleCountFlagBits VulkanDevice::getMaxUsableSampleCount()
{
    // VkPhysicalDeviceProperties physicalDeviceProperties;

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
    {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT)
    {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT)
    {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT)
    {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT)
    {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT)
    {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VulkanDevice::getEnabledFeatures()
{
    if (supportedFeatures.fragmentStoresAndAtomics)
        enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;

    if (supportedFeatures.wideLines)
        enabledFeatures.wideLines = VK_TRUE;

    if (supportedFeatures.samplerAnisotropy)
        enabledFeatures.samplerAnisotropy = VK_TRUE;

    if (supportedFeatures.tessellationShader)
        enabledFeatures.tessellationShader = VK_TRUE;

    if (supportedFeatures.fillModeNonSolid)
        enabledFeatures.fillModeNonSolid = VK_TRUE;

    if (supportedFeatures.pipelineStatisticsQuery)
        enabledFeatures.pipelineStatisticsQuery = VK_TRUE;

    if (supportedFeatures.textureCompressionBC)
        enabledFeatures.textureCompressionBC = VK_TRUE;

    if (supportedFeatures.textureCompressionASTC_LDR)
        enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;

    if (supportedFeatures.textureCompressionETC2)
        enabledFeatures.textureCompressionETC2 = VK_TRUE;

    if (supportedFeatures.depthClamp)
        enabledFeatures.depthClamp = VK_TRUE;

    if (supportedFeatures.multiViewport)
        enabledFeatures.multiViewport = VK_TRUE;
}

void VulkanDevice::clearQueryPool()
{
    if (queryPool != VK_NULL_HANDLE)
    {
        vkDestroyQueryPool(logicalDevice, queryPool, nullptr);
        vkDestroyBuffer(logicalDevice, queryResult.buffer, nullptr);
        vkFreeMemory(logicalDevice, queryResult.memory, nullptr);
    }
}

VkResult
VulkanDevice::createLogicalDevice(std::vector<const char *> enabledExtensions, void *pNextChain, bool useSwapChain,
                                  VkQueueFlags requestedQueueTypes)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

    // Get queue family indices for the requested queue family types
    // Note that the indices may overlap depending on the implementation

    const float defaultQueuePriority(0.0f);

    // Graphics queue
    if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
    {
        queueFamilyIndices.graphics = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamilyIndices.graphics;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    }
    else
    {
        queueFamilyIndices.graphics = 0;
    }

    // Dedicated compute queue
    if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
    {
        queueFamilyIndices.compute = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
        if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
        {
            // If compute family index differs, we need an additional queue create info for the compute queue
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queueFamilyIndices.compute;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    }
    else
    {
        // Else we use the same queue
        queueFamilyIndices.compute = queueFamilyIndices.graphics;
    }

    // Dedicated transfer queue
    if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
    {
        queueFamilyIndices.transfer = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
        if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) &&
            (queueFamilyIndices.transfer != queueFamilyIndices.compute))
        {
            // If transfer family index differs, we need an additional queue create info for the transfer queue
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queueFamilyIndices.transfer;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    }
    else
    {
        // Else we use the same queue
        queueFamilyIndices.transfer = queueFamilyIndices.graphics;
    }

    getEnabledFeatures();

    std::vector<const char *> deviceExtensions(enabledExtensions);
    if (useSwapChain)
    {
        deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &supportedFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

#ifdef VULKAN_VALIDATION_LAYERS
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    enabledFeatures = supportedFeatures;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }
    commandPool = createCommandPool(queueFamilyIndices.graphics);

    return VK_SUCCESS;
}

VkCommandPool VulkanDevice::createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
{
    // QueueFamilyIndices queueFamilyIndices = vDevice.findQueueFamilies(vDevice.getPhysicalDevice());

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = createFlags;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool cmdPool;

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &cmdPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics command pool!");
    }
    return cmdPool;
}

VkFormat VulkanDevice::findDepthFormat()
{
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat VulkanDevice::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                           VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

bool VulkanDevice::beClosed()
{
    return u_ptr_window->crossWindow.shouldClosed();
}

// void VulkanDevice::windowFrameBuffer() {
//     int width = 0, height = 0;
// #ifdef QT_LIB_ENABLE
//     width = *crossWindow.Width();
//     height = *crossWindow.Height();
//     while (width == 0 || height == 0) {
//         width = *crossWindow.Width();
//         height = *crossWindow.Height();
//         //TODO: add event wait loop
////        QEvent
//    }
// #else
//    glfwGetFramebufferSize(crossWindow.getWindow(), &width, &height);
//    while (width == 0 || height == 0) {
//        glfwGetFramebufferSize(crossWindow.getWindow(), &width, &height);
//        glfwWaitEvents();
//    }
// #endif
//}

uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    // VkPhysicalDeviceMemoryProperties memProperties;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanDevice::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        depth};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer, queue);
}

uint32_t VulkanDevice::getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
    // VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        typeBits >>= 1;
    }
    return 0;
}

VkResult VulkanDevice::createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, enma::Buffer *buffer,
                                    VkDeviceSize size, VkDeviceMemory *memory, void *data)
{
    // Create the buffer handle
    VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo(usage, size);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer->buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    // Create the memory backing up the buffer handle
    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
    vkGetBufferMemoryRequirements(logicalDevice, buffer->buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, properties);
    if (vkAllocateMemory(logicalDevice, &memAlloc, nullptr, memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate memory!");
    }

    if (data != nullptr)
    {
        void *mapped;
        if (vkMapMemory(logicalDevice, *memory, 0, size, 0, &mapped) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to map memory!");
        }
        memcpy(mapped, data, size);
        vkUnmapMemory(logicalDevice, *memory);
    }

    if (vkBindBufferMemory(logicalDevice, buffer->buffer, *memory, 0))
    {
        throw std::runtime_error("failed to bind buffer memory!");
    }

    return VK_SUCCESS;
}

VkResult VulkanDevice::createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer,
                                    VkDeviceSize size, VkDeviceMemory *memory, void *data)
{
    // Create the buffer handle
    VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo(usage, size);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    // Create the memory backing up the buffer handle
    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
    vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, properties);
    if (vkAllocateMemory(logicalDevice, &memAlloc, nullptr, memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate memory!");
    }

    if (data != nullptr)
    {
        void *mapped;
        if (vkMapMemory(logicalDevice, *memory, 0, size, 0, &mapped) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to map memory!");
        }
        memcpy(mapped, data, size);
        vkUnmapMemory(logicalDevice, *memory);
    }

    if (vkBindBufferMemory(logicalDevice, *buffer, *memory, 0))
    {
        throw std::runtime_error("failed to bind buffer memory!");
    }

    return VK_SUCCESS;
}

VkResult VulkanDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                    enma::Buffer *buffer, void *data)
{
    buffer->device = logicalDevice;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer->buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer->buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    VkMemoryAllocateFlagsInfoKHR allocFlagsInfo{};
    if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
    {
        allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
        allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        allocInfo.pNext = &allocFlagsInfo;
    }

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &buffer->memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    buffer->alignment = memRequirements.alignment;
    buffer->size = size;
    buffer->usageFlags = usage;
    buffer->memoryPropertyFlags = properties;

    if (data != nullptr)
    {
        buffer->map();
        memcpy(buffer->mapped, data, size);
        if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
            buffer->flush();

        buffer->unmap();
    }

    buffer->setupDescriptor();
    buffer->bind();
    return VK_SUCCESS;
}

// Setup pool and buffer for storing pipeline statistics results
void VulkanDevice::setupQueryResultBuffer()
{
    uint32_t bufSize = 2 * sizeof(uint64_t);

    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
    VkBufferCreateInfo bufferCreateInfo =
        initializers::bufferCreateInfo(
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            bufSize);

    // Results are saved in a host visible buffer for easy access by the application
    if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &queryResult.buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create buffer for qeuery result");
    }
    vkGetBufferMemoryRequirements(logicalDevice, queryResult.buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &queryResult.memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't allocate memory for qeuery result");
    }
    if (vkBindBufferMemory(logicalDevice, queryResult.buffer, queryResult.memory, 0))
    {
        throw std::runtime_error("Can't bind buffer memory for qeuery result");
    }

    // Create query pool
    if (supportedFeatures.pipelineStatisticsQuery)
    {
        VkQueryPoolCreateInfo queryPoolInfo = {};
        queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolInfo.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
        queryPoolInfo.pipelineStatistics =
            VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;
        queryPoolInfo.queryCount = 2;
        if (vkCreateQueryPool(logicalDevice, &queryPoolInfo, NULL, &queryPool))
        {
            throw std::runtime_error("Can't create query pool");
        }
    }
}

// Retrieves the results of the pipeline statistics query submitted to the command buffer
void VulkanDevice::getQueryPoolResult()
{
    // We use vkGetQueryResults to copy the results into a host visible buffer
    vkGetQueryPoolResults(
        logicalDevice,
        queryPool,
        0,
        1,
        sizeof(pipelineStats),
        pipelineStats,
        sizeof(uint64_t),
        VK_QUERY_RESULT_64_BIT);
}

void VulkanDevice::createSurface()
{
    // Делаем обертку для подключения библиотеки QT
#ifdef QT_LIB_ENABLE
    VkResult err = VK_SUCCESS;
    //    auto window = crossWindow.getWindow();

    //    surface = q_instance.surfaceForWindow(window);
    // TODO: create for windows too

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.display = display;
    surfaceCreateInfo.surface = window;
    err = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.connection = u_ptr_window->crossWindow.m_connection; // reinterpret_cast<xcb_connection_t*>(wid);
    surfaceCreateInfo.window = u_ptr_window->crossWindow.m_window;
    err = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
    if (err != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!" + std::to_string(err));
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = (HINSTANCE)u_ptr_window->crossWindow.windowInstance;
    surfaceCreateInfo.hwnd = (HWND)u_ptr_window->crossWindow.m_window;
    err = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#endif
#else
#ifdef GLFW_LIB_ENABLE
    if (glfwCreateWindowSurface(instance, u_ptr_window->crossWindow.getWindow(), nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
#endif
#endif
}

void VulkanDevice::windowFrameBuffer()
{
    int width = 0, height = 0;
    width = *Width();
    height = *Height();
    while (width == 0 || height == 0)
    {
        width = *Width();
        height = *Height();
        // TODO: add event wait loop
        //        QEvent
    }
}

void VulkanDevice::initWindow(std::string app_name
#ifdef QT_LIB_ENABLE
                              ,QWidget *widget)
{
    u_ptr_window->crossWindow.initWindow(app_name, this, widget);
#endif
#ifdef GLFW_LIB_ENABLE
        )
    {
        u_ptr_window->crossWindow.setResizeCallFunc(VulkanDevice::framebufferResizeCallback);
        u_ptr_window->crossWindow.setCloseCallFunc(VulkanDevice::shouldCloseCallback);
        u_ptr_window->crossWindow.initWindow(app_name, this);
#endif
    }

    bool VulkanDevice::qiut()
    {
        return u_ptr_window->crossWindow.m_quit;
    }

    void VulkanDevice::set_quit(bool flag)
    {
        u_ptr_window->crossWindow.m_quit = flag;
    }

    int *VulkanDevice::Width() const {
        return u_ptr_window->crossWindow.Width();
    }

    int *VulkanDevice::Height() const {
        return u_ptr_window->crossWindow.Height();
    }

    uint32_t *VulkanDevice::uWidth() const {
            return u_ptr_window->crossWindow.uWidth();
        }

    uint32_t *VulkanDevice::uHeight() const {
            return u_ptr_window->crossWindow.uHeight();
        }

GLFWwindow *VulkanDevice::get_glfw_window_ptr() {
  return u_ptr_window->crossWindow.getWindow();
}

#ifdef __linux__
    xcb_connection_t *VulkanDevice::get_connection() const
    {
        return u_ptr_window->crossWindow.m_connection;
    }
#endif
#ifdef QT_LIB_ENABLE
    VkWidget *VulkanDevice::init_widget(QWidget * parent)
    {
        return u_ptr_window->crossWindow.initWidget(parent);
    }
#endif
#ifdef QT_LIB_ENABLE

#include "moc_device.cpp"

#endif
