//
// Created by f_f on 04.12.24.
//

#ifndef PK_DISPLAY_BASEOBJECT_H
#define PK_DISPLAY_BASEOBJECT_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <SkyEngine/vk_sky_device.hpp>
#include "vk_sky_swapchain.hpp"
#include "texture.hpp"

struct pipeline_parameters
{
    /**
     * @brief Vulkan graphics pipeline parameters
     *
     */
    VkPipelineDynamicStateCreateInfo *dynamicState;
    VkPipelineColorBlendAttachmentState *colorBlendAttachment;
    VkPipelineDepthStencilStateCreateInfo *depthStencil;
    VkPipelineInputAssemblyStateCreateInfo *inputAssembly;
    VkPipelineViewportStateCreateInfo *viewportState;
    VkPipelineRasterizationStateCreateInfo *rasterizer;
    VkPipelineMultisampleStateCreateInfo *multisampling;
    VkPipelineColorBlendStateCreateInfo *colorBlending;
    VkPipelineVertexInputStateCreateInfo *vertexInputInfo;
    VkPipelineTessellationStateCreateInfo *tesselationState;
};

struct viBuffer
{
    VkBuffer vert;
    VkBuffer ind;
};

enum class ObjectFlags
{
    GLTF_DESCRITOR = 0x00000001
};

enum class ObjectRenderFlags
{
    STDOBJECT = 0,
    TRNOBJECT = 1,
    ANTOBJECT = 1 << 1,
    ALL = TRNOBJECT | ANTOBJECT
};

struct Object
{
    Object() = default;
    /**
     *
     */
    void setEngineDepends(VulkanDevice *device, VulkanSwapChain *swapChain);
    /**
     * @brief Set the Object Sheaders object
     *
     */
    void setObjectShaders();
    ;
    /**
     * @brief load shaders paths of object
     *
     */
    void load_object_shaders(std::vector<std::string> paths);
    ;
    /**
     * @brief Initialization function for models
     *
     */
    virtual void initialization() = 0;
    /**
     * @brief Get the Shader Stages object
     *
     * @return std::vector<VkPipelineShaderStageCreateInfo>
     */
    std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() const;
    /**
     * @brief
     *
     */
    void destroyShaderModules();
    /**
     * @brief Get the Shader Modules object
     *
     * @return std::vector<VkShaderModule>
     */
    std::vector<VkShaderModule> getShaderModules() const;
    /**
     * @brief Get the pipeline object
     *
     * @return VkPipelineLayout
     */
    VkPipelineLayout get_pipeline_layout();
    /**
     * @brief Get the descriptor set layout object
     *
     * @return VkDescriptorSetLayout
     */
    virtual VkDescriptorSetLayout get_descriptor_set_layout();
    /**
     * @brief Get the pipeline object
     *
     * @return VkPipeline
     */
    // VkPipeline get_pipeline()
    // {
    //     return pipeline;
    // }
    /**
     * @brief Get the descriptor set object
     *
     * @return VkDescriptorSet
     */
    VkDescriptorSet get_descriptor_set();
    /**
     * @brief Destroy the Object object
     *
     */
    virtual ~Object();
    ;

    void cleanObjectSwapChain();
    /**
     * @brief Get the Buffer Size object
     *
     * @return VkDeviceSize
     */
    virtual VkDeviceSize getBufferSize() = 0;
    /**
     * @brief Get the Textures object
     *
     * @return std::vector<Texture*>
     */
    virtual uint32_t getTexturesSize() = 0;
    /**
     * @brief Get the descriptor object
     *
     * @param tex_idx
     * @return VkDescriptorImageInfo*
     */
    virtual VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) = 0;
    /**
     * @brief Get the Buffer object
     *
     * @return viBuffer
     */
    virtual viBuffer *getBuffer() = 0;
    /**
     * @brief Get the Indices object
     *
     * @return std::vector<uint32_t>
     */
    virtual std::vector<uint32_t> *getIndices() = 0;
    /**
     * @brief
     *
     * @param _device
     * @param vkSwapChain
     * @param type
     */
    virtual void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) = 0;
    /**
     * @brief load textures for model
     *
     * @param path
     * @param ...
     */
    void load_textures_paths(std::vector<std::string> paths);
    ;
    /**
     * @brief
     *
     */
    virtual void destroy() = 0;
    /**
     * @brief Set the Object Info object
     *
     * @param vertexInputInfo
     * @param depthStencil
     * @param colorBlendAttachment
     * @param inputAssembly
     */
    virtual void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) = 0;
    /**
     * @brief
     *
     */
    virtual void preparePipeline() = 0;
    /**
     * @brief
     *
     * @param frame_time
     */
    virtual void update(float frame_time) = 0;
    /**
     * @brief call only for particle system
     */
    virtual void prepare() = 0;
    /**
     * @brief call specific object draw
     *
     * @param _buffer
     */
    virtual void draw(VkCommandBuffer _buffer) = 0;
    /**
     * @brief Set the Descriptor Layout object
     *
     */
    virtual void setDescriptorLayout() = 0;
    /**
     * @brief Create a Framebuffers object
     *
     */
    virtual void createFramebuffers(VulkanSwapChain *vkSwapChain) = 0;
    /**
     * @brief Create a Render Pass object
     *
     */
    virtual void createRenderPass(VkFormat format) = 0;
    /**
     * @brief Create a Descriptor Sets object
     *
     */
    virtual void createDescriptorSets() = 0;
    /**
     * @brief Create a Descriptor Pool object
     *
     */
    virtual void createDescriptorPool() = 0;
    /**
     * @brief Create a Uniform Buffer object
     *
     */
    virtual void createUniformBuffer() = 0;
    /**
     * @brief requested memcpy for all objects
     */
    virtual void updateMapped() = 0;
    /**
     * @brief create cache if need
     */
    virtual void createPipelineCache()
    {
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        if (vkCreatePipelineCache(vDevice->logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache))
        {
            throw std::runtime_error("failed too create pipeline cache");
        }
    }
    // VK layouts
    VkDescriptorSetLayout descriptorSetLayout{};
    // VkDescriptorSetLayout descriptorSetLayout_textures = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout{};
    // Vk pipeline
    VkPipeline pipeline{};
    VkDescriptorSet descriptor{};

    enma::Buffer uniformObjectBuffer;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    // Vk render
    VkRenderPass renderPass = VK_NULL_HANDLE;
    // //
    std::vector<VkFramebuffer> frameBuffer;
    unsigned int subpass_layout = 0;
    // Bloom
    struct FrameBufferAttachment
    {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    };
    struct FrameBuffer
    {
        VkFramebuffer framebuffer;
        FrameBufferAttachment color, depth;
        VkDescriptorImageInfo descriptor;
    };
    struct OffscreenPass
    {
        int32_t width, height;
        VkRenderPass renderPass;
        VkSampler sampler;
        std::array<FrameBuffer, 2> framebuffers;
    } offscreenPass{};
    //
    VkFormat imageFormat;

    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;

    VkImage colorImage = VK_NULL_HANDLE;
    VkDeviceMemory colorImageMemory = VK_NULL_HANDLE;
    VkImageView colorImageView = VK_NULL_HANDLE;

    VkPipelineCache pipelineCache;

    VkExtent2D *swapChainExtent{};
    VkSampler sampler{};
    // согласно количеству объектов на поток
    std::vector<VkCommandBuffer> cmdBuffer;
    ObjectRenderFlags render_flags = ObjectRenderFlags::STDOBJECT;

    /////////// Методы управления характеристиками объекта ////////////
    glm::vec3 obj_position = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 position();

    glm::vec3 rotation_axis();

    float rotation_speed();

    void set_rotation_angle(float *angle);

    void set_track(float *x, float *y, float *z);

    void set_rotate_axis(float *x, float *y, float *z);

    void set_rotate_track(float *x, float *y, float *z, float *w);

private:
    /**
     * @brief
     *
     * @param filename
     * @param stage
     * @return VkPipelineShaderStageCreateInfo
     */
    VkPipelineShaderStageCreateInfo LoadShader(const std::string &filename, VkShaderStageFlagBits stage);
    std::vector<VkShaderModule> shaderModules;
    /**
     *  Характеристсики любого объекта
     */
    float *x{}, *y{}, *z{};
    float *omega_x{}, *omega_y{}, *omega_z{}, *omega_w{};

protected:
    std::vector<VkPipelineShaderStageCreateInfo> shadersStages;
    VulkanDevice *vDevice{};
    VulkanSwapChain *vSwapChain{};
    std::vector<Texture *> textures;
    std::vector<std::string> shaders_paths;
    std::vector<std::string> textures_paths;
    ObjectFlags object_flags;
    viBuffer trn_buff{};
};

#endif //PK_DISPLAY_BASEOBJECT_H
