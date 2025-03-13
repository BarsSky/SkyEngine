#pragma once

#include <SkyEngine/config/config.h>

#include <vulkan/vulkan.h>

#ifdef QT_LIB_ENABLE

// #include <QWidget>
// #include <QGuiApplication>

#ifdef VK_USE_PLATFORM_XCB_KHR

#include <xcb/xcb.h>
#include <QX11Info>

#endif
#else
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <set>
//

#include <SkyEngine/export_import_magick.h>
#include <SkyEngine/qt_plugin/vkwidget.h>
#include <SkyEngine/vk_sky_buffer.hpp>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class PM_IO_VULKAN_EXPORT VulkanDevice
{
public:
    SwapChainSupportDetails swapChainSupport;

    // #ifdef QT_LIB_ENABLE
    //     QVulkanInstance q_instance;
    // #endif
    VkInstance instance;

    VkQueue queue;
    //
    VkRenderPass renderPass;

    //
    VkAllocationCallbacks *g_Allocator = nullptr;
    // Qeuery pool
    VkQueryPool queryPool = VK_NULL_HANDLE;
    /** @brief Physical device representation */
    VkPhysicalDevice physicalDevice;
    /** @brief Logical device representation (application's view of the device) */
    VkDevice logicalDevice;
    /** @brief Properties of the physical device including limits that the application can check against */
    VkPhysicalDeviceProperties physicalDeviceProperties;
    /** @brief Features of the physical device that an application can use to check if a feature is supported */
    VkPhysicalDeviceFeatures features;
    /** @brief Features that have been enabled for use on the physical device */
    VkPhysicalDeviceFeatures enabledFeatures;
    /** @brief Memory types and heaps of the physical device */
    VkPhysicalDeviceMemoryProperties memoryProperties;
    /** @brief Queue family properties of the physical device */
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    /** @brief List of extensions supported by the device */
    std::vector<std::string> supportedExtensions;
    /** @brief Default command pool for the graphics queue family index */
    VkCommandPool commandPool = VK_NULL_HANDLE;
    /** @brief Set to true when the debug marker extension is detected */
    bool enableDebugMarkers = false;
    /** @brief Contains queue family indices */

    // for window
    // TODO: Вынести на другие механизмы определения размеров окна
    // Унифицировать из
    //    int getWidth() const;
    //    int getHeight() const;
    int *Width();
    int *Height();
    bool qiut();
    void set_quit(bool flag);

#ifdef GLFW_LIB_ENABLE
  GLFWwindow * get_glfw_window_ptr();
#endif

#ifdef __linux__
    xcb_connection_t *get_connection() const;
#endif
#ifdef QT_LIB_ENABLE
    VkWidget *init_widget(QWidget *parent);
    // bool framebufferResized = false;
#endif
    operator VkDevice() const
    {
        return logicalDevice;
    };

    VkPhysicalDevice getPhysicalDevice()
    {
        return physicalDevice;
    }

    explicit VulkanDevice();

    ~VulkanDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    // QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);

    VkSampleCountFlagBits getMaxUsableSampleCount();

    void pickPhysicalDevice();

    bool QueueFamilyProperties();

    VkCommandBuffer beginSingleTimeCommands(VkCommandBufferLevel level, VkCommandPool pool, bool begin);

    VkCommandBuffer beginSingleTimeCommands(VkCommandBufferLevel level, bool begin = true);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free = true);

    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true);

    VkResult
    createLogicalDevice(std::vector<const char *> enabledExtensions, void *pNextChain, bool useSwapChain = true,
                        VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

    VkFormat
    findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkSurfaceKHR surface;

    void createSurface();

    bool beClosed();

    void windowFrameBuffer();

    void createRenderPass();

    void setupQueryResultBuffer();

    void getQueryPoolResult();

    void getEnabledFeatures();

    void clearQueryPool();

    VkCommandPool createCommandPool(uint32_t queueFamilyIndex,
                                    VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VkPhysicalDeviceFeatures supportedFeatures;

    VkFormat findDepthFormat();

    void initWindow(std::string app_name
#ifdef QT_LIB_ENABLE
                    ,
                    QWidget *widget = nullptr
#endif
    );

    uint32_t getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);

    uint32_t getQueueFamilyIndex(VkQueueFlags queueFlags) const;

    VkResult
    createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, enma::Buffer *buffer,
                 void *data = nullptr); // VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    VkResult
    createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceSize size,
                 VkDeviceMemory *memory, void *data = nullptr);

    VkResult
    createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, enma::Buffer *buffer, VkDeviceSize size,
                 VkDeviceMemory *memory, void *data = nullptr);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth = 1);

    uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr) const;

    bool framebufferResized = false;

    bool ready_to_close = false;

    // Pipeline statistic
    struct
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
    } queryResult;

    struct
    {
        uint32_t graphics;
        uint32_t compute;
        uint32_t transfer;
    } queueFamilyIndices;

    struct
    {
        VkSemaphore presentComplete;
        VkSemaphore renderComplete;
    } semaphores;

    uint64_t pipelineStats[2] = {0};

    class Window_Impl;

private:
    std::unique_ptr<Window_Impl> u_ptr_window;

#ifdef GLFW_LIB_ENABLE
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    static void shouldCloseCallback(GLFWwindow *window);
#endif
};