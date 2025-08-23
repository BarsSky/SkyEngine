#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <cstring>

#include <SkyEngine/vk_sky_swapchain.hpp>


enum class TextureType : unsigned int
{
    SIMPLE,
    PARTICLE
};

struct Texture
{
    Texture();
    ~Texture();
/**
 *  @brief basis vulkan variables
 */
    VkQueue graphicsQueue;
    VulkanDevice *vDevice;
    VulkanSwapChain *swapChain;
/**
 * @brief
 */
    uint32_t width, height, depth = 1;
    uint32_t layerCount;
    uint32_t mipLevels = 1;
/**
 *
 */
    uint8_t * texture_data = nullptr;
/**
 * @brief
 */
    VkDescriptorImageInfo descriptor;
    VkImage textureImage;
    VkImageLayout imageLayout;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    VkImageViewType viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

    enma::Buffer buffer;
    TextureType type;
    std::string path;

/**
 * @brief
 */
//    ktxResult loadKTXFile(std::string filename, ktxTexture **target);
/**
 * @brief
 */
    void loadTexture(std::string _path, VulkanDevice *_device, VulkanSwapChain *vkSwapChain, TextureType _type = TextureType::SIMPLE, VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D);
/**
 * @brief
 */
    void prepareTexture(VulkanDevice *device, VulkanSwapChain *vkSwapChain, uint32_t width,uint32_t  height, uint32_t depth, uint32_t byte_count);
/**
 * @brief
 */
    void createTextureImage();
/**
 * @brief
 */
    void destroy();
/**
 * @brief
 */
    void updateDescriptor();
/**
 * @brief
 */
    virtual void destroy_local() { return; };

private:
/**
 * @brief
 */
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
/**
 * @brief
 */
    void createTextureImageView();
/**
 * @brief
 */
    void createTextureSampler();
/**
 * @brief
 */
    void createParticleTextureSampler();
/**
 * @brief
 */
    void createHeightMapSampler();
/**
 * @brief
 */
    void createArraySampler();
};

struct Texture2DKTX : public Texture
{
    bool need_local_destroy = false;
    void loadFromFile(std::string filename,
                      VkFormat format,
                      VulkanDevice *device,
                      VkQueue copyQueue,
                      VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
                      VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                      bool forceLinear = false);

    void fromBuffer(void *buffer,
                    VkDeviceSize bufferSize,
                    VkFormat format,
                    uint32_t texWidth,
                    uint32_t texHeight,
                    VulkanDevice *device,
                    VkQueue copyQueue,
                    VkFilter filter = VK_FILTER_LINEAR,
                    VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
                    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

//protected:
    void destroy_local() override;
};

/**
 * @brief HeightMap create struct
 */

struct HeightMap
{
private:
    uint16_t *heightdata;
    uint32_t dim;
    uint32_t scale;

public:
    HeightMap(std::string filename, uint32_t patchsize);;

    ~HeightMap()
    {
        delete[] heightdata;
    }

    float getHeight(uint32_t x, uint32_t y);
};

class Texture2DArray : public Texture
{
public:
    void loadFromFile(
            std::string        filename,
            VkFormat           format,
            VulkanDevice *device,
            VkQueue            copyQueue,
            VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};

class TextureCubeMap : public Texture
{
public:
    void loadFromFile(
            std::string        filename,
            VkFormat           format,
            VulkanDevice *device,
            VkQueue            copyQueue,
            VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};



