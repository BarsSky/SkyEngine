#pragma once

#include <SkyEngine/vk_sky_device.hpp>

struct FrameBufferAttachments
{
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFormat format;
};

typedef struct _SwapChainBuffers {
	VkImage image;
	VkImageView view;
} SwapChainBuffer;

struct Attachments
{
    FrameBufferAttachments position, normal, albedo;
    int32_t width;
    int32_t height;
};

enum  RenderType{
    USUAL = 0,
    USE_TRANSPARENT_OBJECT = 1,
    USE_ANOTHER_RENDER_SETTINGS = 1 << 1,
    ALL = USE_TRANSPARENT_OBJECT | USE_ANOTHER_RENDER_SETTINGS
};

struct VulkanSwapChain
{
    VulkanDevice *vDevice = nullptr;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;

    VkExtent2D swapChainExtent;
    //
    void createAttacment(VkFormat format, VkImageUsageFlags usage, FrameBufferAttachments *attachment);
    Attachments attachments;
    void clearAttachment(FrameBufferAttachments *attach);
    void createGBufferAttachments();
    void resizeAttachments();
    std::vector<VkDescriptorSet *> attachDescriptors;
    std::vector<VkDescriptorSet *> attachTransparents;
    void addDescriptorAttach(VkDescriptorSet *descriptor);
    void addDescriptorTransperent(VkDescriptorSet *descriptor);
    //
    std::vector<VkFramebuffer> swapChainFramebuffers;

    std::vector<VkImageView> swapChainImageViews;

    VkFormat swapChainImageFormat;

    VkColorSpaceKHR swapChainColorSpace;

    std::vector<VkImage> swapChainImages;
	std::vector<SwapChainBuffer> buffers;


    uint32_t mipLevels = 1;

    uint32_t imageCount;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    uint32_t queueNodeIndex = UINT32_MAX;

    VulkanSwapChain();
    ~VulkanSwapChain();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

    void cleanupSwapChain();
    void createSwapChain();
    void recreateFrameBuffer();

    std::vector<VkAttachmentReference> set_color_attachments_reference();

    void create(int *width, int *height, bool vsync = false, bool fullscreen = false);

    void setRenderType(RenderType flag);
private:
    void createFramebuffersSW();
    void createRenderPass();
    void createImageViews();
    //
    //
    void createColorResources();
    void createDepthResources();
    void searchSurfaceFormat();
    VkSurfaceFormatKHR surfaceFormat;
    RenderType render_type = RenderType::USUAL;
};