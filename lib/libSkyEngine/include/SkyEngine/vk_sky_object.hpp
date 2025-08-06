#pragma once

#include <SkyEngine/config/config.h>
#include <array>
#include <memory>
#ifndef QT_LIB_ENABLE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <utility>
#include <SkyEngine/vk_sky_texture.hpp>
#include <SkyEngine/export_import_magick.h>

#define DEPTH_ARRAY_SCALE 4096 // TODO: make variable for change check depth

struct LIBSKYENGINE_EXPORT pipeline_parameters {
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

const int MAX_FRAMES_IN_FLIGHT = 1;
/**
 * @brief
 */
struct LIBSKYENGINE_EXPORT viBuffer {
  VkBuffer vert;
  VkBuffer ind;
};

enum class ObjectFlags {
  GLTF_DESCRITOR = 0x00000001
};

enum class ObjectRenderFlags {
  STDOBJECT = 0,
  TRNOBJECT = 1,
  CMPTOBJECT = 2,
  ANTOBJECT = 1 << 1,
  ALL = TRNOBJECT | ANTOBJECT
};

/**
 * @brief base struct for all object with virtual functions
 */
struct LIBSKYENGINE_EXPORT Object {
  explicit Object();

  /**
   * @brief Sets the engine dependencies for the object.
   * @param device Pointer to VulkanDevice.
   * @param swapChain Pointer to VulkanSwapChain.
   */
  void setEngineDepends(VulkanDevice *device, VulkanSwapChain *swapChain);

  /**
   * @brief Sets the shaders for the object.
   */
  void setObjectShaders();;

  /**
   * @brief Loads shader paths for the object.
   * @param paths Vector of shader file paths.
   */
  void load_object_shaders(std::vector<std::string> paths);

  /**
   * @brief Draws the object using the provided command buffer.
   * @param _buffer Vulkan command buffer.
   */
  void object_draw(VkCommandBuffer _buffer) {
    if (!is_object_visible)
      return;
    /// TODO: Add draw description set for base manage functions
    drawObjectBase(_buffer);
    draw(_buffer);
  }

  /**
   * @brief Creates the descriptor pool for the object.
   */
  void objectCreateDescriptorPool() {
    ///TODO: Add universal function for create base pool descriptor
    createObjectBasePool();
    createDescriptorPool();
    allocateDescriptorPool();
  };

  /**
   * @brief Creates the descriptor sets for the object.
   */
  void objectCreateDescriptorSets() {
    ////TODO: Add universal function for create base descriptor set
    createObjectBaseDescriptor();
    createDescriptorSets();
  };
  /**
 * @brief Sets the descriptor layout for the object.
 */
  void objectSetDescriptorLayout() {
    ////TODO: Add universal function for create base descriptor layout
    setObjectBaseLayout();
    setDescriptorLayout();
  };
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
  auto getShaderStages() const -> std::vector<VkPipelineShaderStageCreateInfo>;

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
  auto getShaderModules() const -> std::vector<VkShaderModule>;

  /**
   * @brief Get the pipeline object
   *
   * @return VkPipelineLayout
   */
  auto get_pipeline_layout() -> VkPipelineLayout;

  /**
   * @brief Get the descriptor set layout object
   *
   * @return VkDescriptorSetLayout
   */
  virtual auto get_descriptor_set_layout() -> VkDescriptorSetLayout;

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
  auto get_descriptor_set() -> VkDescriptorSet;

  /**
    *@brief set visible property
    *
    */
  void setVisibleProperty(bool flag);

  /**
   * @brief Destroy the Object object
   *
   */
  virtual ~Object();

  void cleanObjectSwapChain();

  /**
   * @brief Get the Buffer Size object
   *
   * @return VkDeviceSize
   */
  virtual auto getBufferSize() -> VkDeviceSize = 0;

  /**
   * @brief Get the Textures object
   *
   * @return std::vector<Texture*>
   */
  virtual auto getTexturesSize() -> uint32_t = 0;

  /**
   * @brief Get the descriptor object
   *
   * @param tex_idx
   * @return VkDescriptorImageInfo*
   */
  virtual auto get_descriptor_image(size_t tex_idx) -> VkDescriptorImageInfo * = 0;

  /**
   * @brief Get the Buffer object
   *
   * @return viBuffer
   */
  virtual auto getBuffer() -> viBuffer * = 0;

  /**
   * @brief Get the Indices object
   *
   * @return std::vector<uint32_t>
   */
  virtual auto getIndices() -> std::vector<uint32_t> * = 0;

  /**
   * @brief
   *
   * @param _device
   * @param vkSwapChain
   * @param type
   */
  virtual void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) = 0;

  /**
   * @brief Loads texture paths for the model.
   * @param paths Vector of texture file paths.
   */
  void load_textures_paths(std::vector<std::string> paths);;

  /**
   * @brief Destroys the object and releases resources.
   */
  void object_destroy();
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
  virtual void createPipelineCache();

  /**
 * @brief create additinal compute command buffer if needed for object
 */
  void createAllBuffers() {
    //TODO: Don't forgot Clear buffer
    createUniqueBuffers();
    createUniformBuffer();
  }
  /**
   * @brief create additinal compute command buffer if needed for object
   */
  virtual void createAdditinalBuffer() = 0;

  /**
   *  @brief make barrier
   */
  virtual void acquireBarrier(VkCommandBuffer _buffer) {
  };

  /**
   * @brief release barier
   */
  virtual void releaseBarrier(VkCommandBuffer _buffer) {
  };

  /**
   * @brief additional destroy for compute
   */
  virtual void additionalDestroy() {
  };

  /**
   * @brief if create compute shader for model
   */
  virtual void clearComputeBlock();

  //////////////>@brief COMPUTE BLOCK
  /**
   * @brief get compute buffer
   * @return
   */
  auto getComputeBuffer() const -> VkCommandBuffer *;

  /**
   * @brief get compute queue
   * @return
   */
  auto getComputeQueue() const -> VkQueue;

  /**
   * @brief get compute semaphore
   * @return
   */
  auto getComputeSemaphore() -> VkSemaphore *;

  /**
   * @brief get graphic semaphore
   * @return VkSemaphore
   */
  auto getGraphicSemaphore() const -> VkSemaphore *;

  /**
 *  @brief get data from shader
 */
  virtual void readShaderData();


  // VK layouts
  VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout pickDescriptorSetLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout computeDescriptorSetLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout descriptorSetLayoutImages = VK_NULL_HANDLE;
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
  struct FrameBufferAttachment {
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
  };

  struct FrameBuffer {
    VkFramebuffer framebuffer;
    FrameBufferAttachment color, depth;
    VkDescriptorImageInfo descriptor;
  };

  struct OffscreenPass {
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

  VkPipelineCache pipelineCache = VK_NULL_HANDLE;

  VkExtent2D *swapChainExtent{};
  VkSampler sampler{};

  // согласно количеству объектов на поток
  std::vector<VkCommandBuffer> cmdBuffer;
  ObjectRenderFlags render_flags = ObjectRenderFlags::STDOBJECT;

  void set_mouse_ptr(glm::vec2 * ptr_point) {
    mouse_position = ptr_point;
  }
  //FIXME: DEPRECATED FUNCTION
  void set_screen_ptr(glm::vec2 * ptr_point) {
    screen_size = *ptr_point;
  }

  /////////// Методы управления характеристиками объекта ////////////
  glm::vec3 obj_position = glm::vec3(0.0f, 0.0f, 0.0f);

  auto position() -> glm::vec3;

  auto rotation_axis() -> glm::vec3;

  auto rotation_speed() -> float;

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
  auto LoadShader(const std::string &filename, VkShaderStageFlagBits stage) -> VkPipelineShaderStageCreateInfo;

  std::vector<VkShaderModule> shaderModules;
  /**
   *  @brief Определяем все общие буфферы для объектов
   */
  void createUniqueBuffers();

  /**
   * @brief Clear unique buffers
   */
  void clearUniqueBuffers();

  /**
   *
   */
  void drawObjectBase(VkCommandBuffer _buffer);
  /**
   *
   */
  void createObjectBaseDescriptor();
  /**
   *
   */
  void createObjectBasePool();

  /**
   *
   */
  void allocateDescriptorPool();
  /**
   *
   */
  void setObjectBaseLayout();
  /**
   *  Характеристсики любого объекта
   */
  float *x{}, *y{}, *z{};
  float *omega_x{}, *omega_y{}, *omega_z{}, *omega_w{};

  VkDescriptorSet pickDescriptor{};

  struct ComputeInst;

protected:
  std::vector<VkPipelineShaderStageCreateInfo> shadersStages;
  VulkanDevice *vDevice{};
  VulkanSwapChain *vSwapChain{};
  std::vector<Texture *> textures;
  std::vector<std::string> shaders_paths;
  std::vector<std::string> textures_paths;
  ObjectFlags object_flags;
  viBuffer trn_buff{};
  //COMPUTE BLOCK
  std::unique_ptr<ComputeInst> u_ptr_compute;
  // DRAW BLOCK
  bool is_object_visible = true;
  glm::vec2 *mouse_position = nullptr;
  glm::vec2 screen_size;
  // Pick Object BLOCK
  enma::Buffer pickObjectBuffer;
  /**
   * Vulkan vectors for pool descriptors layouts
   */
  std::vector<VkDescriptorPoolSize> vkPoolSizes{};
  uint32_t poolDrawSize = 0;
  // TODO: make as function for increase layout counter then add new layout in vector automaticaly
  std::vector<VkDescriptorSetLayout> vkDescriptorLayouts{};// compare all layouts in one vector for pipline layout
};

struct Object::ComputeInst {
  VulkanDevice *vDevice{};
  uint32_t queueFamilyIndex{};
  // Used to check if compute and graphics queue families differ and require additional barriers
  enma::Buffer storageBuffer; // (Shader) storage buffer object containing the particles
  enma::Buffer uniformBuffer; // Uniform buffer object containing particle system parameters
  enma::Buffer hitBuffer; // hit buffer object for take data from compute shader
  VkQueue queue{}; // Separate queue for compute commands (queue family may differ from the one used for graphics)
  VkCommandPool commandPool{}; // Use a separate command pool (queue family may differ from the one used for graphics)
  VkCommandBuffer commandBuffer = VK_NULL_HANDLE; // Command buffer storing the dispatch commands and barriers
  VkSemaphore compute{}; // Execution dependency between compute & graphic submission
  VkSemaphore graphic{}; // Execution dependency between compute & graphic submission
  VkDescriptorSetLayout descriptorSetLayout{}; // Compute shader binding layout
  VkDescriptorSet descriptorSet{}; // Compute shader bindings
  VkPipelineLayout pipelineLayout{}; // Layout of the compute pipeline
  VkPipeline pipeline{}; // Compute pipeline for updating particle positions

  void set_device(VulkanDevice *vDev) {
    vDevice = vDev;
  }

  void destroy() {
    storageBuffer.destroy();
    hitBuffer.destroy();
    uniformBuffer.destroy();
    vkDestroyPipelineLayout(vDevice->logicalDevice, pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(vDevice->logicalDevice, descriptorSetLayout, nullptr);
    vkDestroyPipeline(vDevice->logicalDevice, pipeline, nullptr);
    vkFreeCommandBuffers(vDevice->logicalDevice, commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(vDevice->logicalDevice, commandPool, nullptr);
  }
};
