#pragma once

#include <SkyEngine/config/config.h>

#ifndef QT_LIB_ENABLE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <string>
#include <algorithm>
#include <utility>
#include <vector>
#include <random>
#include <cstring>

#include <SkyEngine/vk_sky_object.hpp>
#include <SkyEngine/vk_sky_buffer.hpp>


/**
 * @brief Vertex struct enum class
 */
enum class VertexComponent {
  Position,
  Normal,
  UV,
  Color,
  Tangent,
  Joint0,
  Weight0
};

/**
 * @brief Vertex struct class
 */

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec4 color;
  glm::vec4 joint0;
  glm::vec4 weight0;
  glm::vec4 tangent;

  bool operator==(const Vertex &other) const {
    return pos == other.pos &&
           normal == other.normal &&
           uv == other.uv &&
           color == other.color &&
           joint0 == other.joint0 &&
           weight0 == other.weight0 &&
           tangent == other.tangent;
  }

  static VkVertexInputBindingDescription vertexInputBindingDescription;
  static std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
  static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;

  static VkVertexInputBindingDescription inputBindingDescription(uint32_t binding);

  //    dolphin = reinterpret_cast<Model2D*>(createObject(pipelineObject(ePipelineObjectType::OBJECT_2D,"")));
  ////    dolphin->load_textures_paths({MODELS_DIRECTORY + "/particle_fire.ktx"});
  //    dolphin->load_object_shaders({"../shaders/model2d.vert.spv", "../shaders/model2d.frag.spv"});
  static VkVertexInputAttributeDescription
  inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);

  static std::vector<VkVertexInputAttributeDescription>
  inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components);

  /** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
  static VkPipelineVertexInputStateCreateInfo *
  getPipelineVertexInputState(const std::vector<VertexComponent> components);
};

namespace std {
  template<>
  struct hash<Vertex> {
    size_t operator()(Vertex const &vertex) const {
      return ((hash<glm::vec3>()(vertex.pos) ^
               (hash<glm::vec4>()(vertex.color) ^
                (hash<glm::vec3>()(vertex.normal)) << 1)) >>
                                                          1) ^
             (hash<glm::vec2>()(vertex.uv) << 1);
    }
  };
}

class uniformBuffer {

};

/**
 * @brief uniform object buffer
 */
struct UniformBufferObject : public uniformBuffer {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec4 viewPos;
  glm::vec4 lightPositon;
};

/**
 *
 */
struct UniformBufferLine : public uniformBuffer {
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec4 viewPos;
};
/**
 * @brief
 *
 */
struct UniformBuffer2D : public uniformBuffer {
  glm::mat4 projection;
  glm::mat4 modelView;
  glm::mat4 view;
  glm::vec4 viewPos;
};
/**
 * @brief
 *
 */
struct UniformBufferSkyBox {
  glm::mat4 proj;
  glm::mat4 view;
  glm::mat4 model;
};
/**
 * @brief uniform particle buffer
 */
struct UniformBufferParticle : public uniformBuffer {
  glm::mat4 projection{};
  glm::mat4 modelView{};
  glm::mat4 model{};
  glm::vec2 viewportDim{};
  float pointSize = 10.f;
};
/**
 * @brief uniform shadows buffer
 */
struct UniformBufferShadows : public uniformBuffer {
};
/**
 * @brief uniform tesselation buffer
 */
struct UniformBufferTessellation : public uniformBuffer {
  glm::mat4 projection{};
  glm::mat4 modelview{};
  glm::vec4 lightPos = glm::vec4(-48.0f, -40.0f, 46.0f, 0.0f);
  glm::vec4 frustumPlanes[6]{};
  float displacementFactor = 32.0f;
  float tessellationFactor = 0.75f;
  glm::vec2 viewportDim{};
  // Desired size of tessellated quad patch edge
  float tessellatedEdgeSize = 20.0f;
};
/**
 * @brief
 *
 */
struct UniformBufferTransparent : public uniformBuffer {
  glm::mat4 projection;
  glm::mat4 modelview;
  glm::mat4 model;
  glm::vec4 lightPositon;
};

struct UniformBufferParticleGPU : public uniformBuffer {
  float deltaT{};              //		Frame delta time
  float destX{};              //		x position of the attractor
  float destY{};              //		y position of the attractor
  float destZ{};              //		z position of the attractor
  float estLifetime = 1;          //  time to live
  int32_t particleCount = 256;
};

struct UniformBufferParticleGPUPosition : public uniformBuffer {
  glm::mat4 projection;
  glm::mat4 modelview;
  glm::mat4 model;
  glm::vec2 viewportDim;
  alignas(16) glm::vec3 emiter_position;
};


struct UniformBufferAnimate : public uniformBuffer {
  glm::mat4 projection;
  glm::mat4 view;
  glm::vec4 lightPos = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);
};

struct UniformBufferCompute : public uniformBuffer {
  glm::vec2 position;
  glm::uint id;
};

/**
 * @brief all buffers struct
 */
//struct uniformsBuffers {
//    // fill this struct with all uni buffers for all type of objects
//    UniformBufferObject object;
//    UniformBufferSkyBox skybox;
//    UniformBufferParticle part_object;
//    UniformBufferParticle shadow_object;
//    UniformBufferTessellation tesselation_object;
//    UniformBufferTransparent transparent_object;
//};
/**
 * @brief Particle enum class
 */
enum class ParticleComponent {
  Position,
  Color,
  Alpha,
  Size,
  Rotation,
  Type
};

/**
 * @brief Particle struct create particle objects
 */
struct PM_IO_VULKAN_EXPORT Particle {
  glm::vec4 pos;
  glm::vec4 color;
  float alpha;
  float size;
  float rotation;
  uint32_t type;
  // Attributes not used in shader
  glm::vec4 vel;
  float rotationSpeed;

  static VkVertexInputBindingDescription vertexInputBindingDescription;
  static std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
  static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;

  static VkVertexInputBindingDescription inputBindingDescription(uint32_t binding);

  static VkVertexInputAttributeDescription
  inputAttributeDescription(uint32_t binding, uint32_t location, ParticleComponent component);

  static std::vector<VkVertexInputAttributeDescription>
  inputAttributeDescriptions(uint32_t binding, const std::vector<ParticleComponent> components);

  /** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
  static VkPipelineVertexInputStateCreateInfo *
  getPipelineVertexInputState(const std::vector<ParticleComponent> components);
};

// SSBO particle declaration
//#pragma pack (push,1)
struct ParticleGPU {
  alignas(16) glm::vec3 pos;                // Particle position
  alignas(16) glm::vec3 vel;               // Particle velocity
  alignas(16) glm::vec3 gradientPos;        // Texture coordinates for the gradient ramp map
  alignas(16) glm::vec3 randomPos;
  float estLifetime;
  float lifeTime;
};
//#pragma pack(pop)
struct Vert {
  int count;
  enma::Buffer buffer;
};

struct Ind {
  int count;
  enma::Buffer buffer;
};
/**
 * @brief model type struct enum class
 */
enum class model_type : uint8_t {
  FROMOBJ,
  FROMKTX
};

/**
 * @brief Model class
 */
//struct BufferModel : public Object {
//	Vertex vertex;
//
//	BufferModel(uint32_t *buffer) : Object() {
//	}
//
//private:
//	std::vector<Vertex> vertices;
//	std::vector<uint32_t> indices;
//};

struct PM_IO_VULKAN_EXPORT Model : public Object {

  UniformBufferObject object_ubo{};

  Vertex vertex{};

  uint32_t *_buffer{}, *indeces{}, _shape_num{};
  model_type type;

  glm::vec3 currentPos;

  Model(uint32_t *buffer, uint32_t *_ind, uint32_t shape_num,
        model_type _type = model_type::FROMOBJ);

  void initialization() override;

  void updateMapped() override;

  explicit Model(std::string _path, model_type _type = model_type::FROMOBJ);;

  void draw(VkCommandBuffer model_buffer) override;

  ~Model() override;

  glm::mat4 getOrintationMat(glm::vec3 vec, glm::vec3 start_orientation);

  glm::vec3 getRotationVector(glm::vec3 point_to,
                              glm::vec3 point_from,
                              bool counterclockwise,
                              glm::vec3 rotate_axis);

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  //
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  /**
   * @brief
   */
  void destroy() override;

  /**
   *
   */
  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void set_vertices(Vertex *vert, size_t size);

  void set_indecies(uint32_t *ind, size_t size);

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void bindBuffers(VkCommandBuffer commandBuffer);

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  /**
   *
   */
  VkDeviceSize getBufferSize() override;;

  uint32_t getTexturesSize() override;;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;;

  void update(float frame_time) override;;

  void prepare() override;;

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  void createAdditinalBuffer() override;

  void LoadModelFromFile(std::string path);

  void GenerateModelFromFile(std::string path);

  void GenerateModelFromBuffer(uint32_t *buffer, uint32_t *_ind, uint32_t size, uint32_t shape_num);

  void createIndexBuffer();

  void createVertexBuffer();

private:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::string path = "";

  bool buffersBound = false;
  Vert vert;
  Ind ind;
};
///\Defined properties

/**
 * @brief Partical_Model_CPU class
 */
#define PARTICLE_TYPE_FLAME 0
#define PARTICLE_TYPE_SMOKE 1
#ifdef _MSC_VER
#define M_PI 3.141592265358979323864
#endif

struct PM_IO_VULKAN_EXPORT Partical_Model_CPU : public Object {

  UniformBufferParticle particl_ubo;

  Particle particle{};

  Partical_Model_CPU();

  void initialization() override;

  void updateMapped() override;

  ~Partical_Model_CPU() override = default;

  void draw(VkCommandBuffer _buffer) override;

  /**
   * @brief particle model properties
   */
  // TODO: Сформировать внешние функции задания параметров
  unsigned int particle_count = 512;
  float radius = 8.0f;
  float alpha = 0.0f;

  //API for particle
  /**
   *
   * @param _count
   */
  void set_count(unsigned int _count) {
    particle_count = _count;
  };

  /**
   *
   * @param _radius
   */
  void set_radius(float _radius) {
    radius = _radius;
  };

  /**
   *
   * @param position
   * TODO: create glm::vec4 for change direction
   */
  void set_position(glm::vec3 position) {
    emitterPos = position;
  };

  //Поовернуть в заданном направлении
  void set_direction(glm::vec3 direction) {
    //err guard
    if (direction == glm::vec3(0, 0, 0))
      return;

    direction = glm::normalize(direction);
    if (lastDirection == direction)//if nothing changed
      return;

    minVel = minVel_base * direction;
    maxVel = maxVel_base * direction;
    lastDirection = direction;
  }

  /**
   *
   * @param value
   */
  void set_alpha(float value) {
    alpha = value;
  }

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  // Предопределяем тип буфера
  VkDeviceSize bufferSize = sizeof(UniformBufferParticle);

  std::default_random_engine rndEngine;

  std::vector<Particle> particleBuffer;
  glm::vec3 emitterPos = glm::vec3(0.0f, -radius + 2.0f, 0.0f);

  float width = 3.f;
  glm::vec3 minVel = glm::vec3(0.5f, 0.5f, 0.5f);
  glm::vec3 minVel_base = glm::vec3(0.5f, 0.5f, 0.5f);
  glm::vec3 maxVel = glm::vec3(17.0f, 17.0f, 17.0f);
  glm::vec3 maxVel_base = glm::vec3(17.0f, 17.0f, 17.0f);
  glm::vec3 lastDirection = glm::vec3(0, 0, 0);

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  /**
   * @brief
   */
  void destroy() override;

  /**
   *
   */

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  VkDeviceSize getBufferSize() override;

  uint32_t getTexturesSize() override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;;

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
    //
    void *mappedMemory;
    // Size of the particle buffer in bytes
    size_t size;
  } particles{};

  float rnd(float range);

  void prepareParticles();

  void updateParticles(float frameTimer);

  void transitionParticle(Particle *part);

  void initParticle(Particle *part, glm::vec3 emit_pos);

  void createAdditinalBuffer() override;
};

// Resources for the compute part of the example

struct PM_IO_VULKAN_EXPORT Partical_Model_GPU : public Object {
  struct {
    VkPipelineVertexInputStateCreateInfo inputState;
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
  } vertices;

  UniformBufferParticleGPU ubo_gpu_particl;
  UniformBufferParticleGPUPosition ubo_pos_particle;
  enma::Buffer storageBuffer;

  ParticleGPU particle{};

  Partical_Model_GPU();

  ~Partical_Model_GPU() override = default;

  void set_count(unsigned int _count) {
    particle_count = _count;
  };

  /**
   *
   * @param _radius
   */
  void set_radius(float _radius) {
    radius = _radius;
  };

  /**
   *
   * @param position
   * TODO: create glm::vec4 for change direction
   */
  void set_position(glm::vec3 position) {
    this->ubo_pos_particle.emiter_position= position;
  };

  //Поовернуть в заданном направлении
  void set_direction(glm::vec3 direction) {
    //err guard
    //direction *= 10;
    this->ubo_gpu_particl.destX = direction.x;
    this->ubo_gpu_particl.destY = direction.y;
    this->ubo_gpu_particl.destZ = direction.z;
  }

  /**
   *
   * @param value
   */
  void set_alpha(float value) {
    alpha = value;
  }

  // Resources for the compute part of the example

  VkDeviceSize bufferSize = sizeof(UniformBufferParticle);

  std::default_random_engine rndEngine;
  glm::vec3 emitterPos = glm::vec3(0.0f, -radius + 2.0f, 0.0f);

  float width = 3.f;
  glm::vec3 minVel = glm::vec3(0.5f, 0.5f, 0.5f);
  glm::vec3 minVel_base = glm::vec3(0.5f, 0.5f, 0.5f);
  glm::vec3 maxVel = glm::vec3(17.0f, 17.0f, 17.0f);
  glm::vec3 maxVel_base = glm::vec3(17.0f, 17.0f, 17.0f);
  glm::vec3 lastDirection = glm::vec3(0, 0, 0);

  unsigned int particle_count = 512;
  float radius = 8.0f;
  float alpha = 0.0f;

  void draw(VkCommandBuffer _buffer) override;

  void initialization() override;

  void updateMapped() override;

  void preparePipeline() override;

  void prepareCompute() const;

  void buildComputeCommandBuffer() const;

  void setDescriptorLayout() override;

  void prepareUniformBuffers();

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D)  override;

  void prepareParticles();

  void createDescriptorPool() override;

  void createDescriptorSets() override;

  void createAdditinalBuffer() override;

  void acquireBarrier(VkCommandBuffer _buffer) override;

  void releaseBarrier(VkCommandBuffer _buffer) override;

  void createSemaphore();

  VkDeviceSize getBufferSize() override;

  uint32_t getTexturesSize() override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  void destroy() override;

  void additionalDestroy() override;

  void clearComputeBlock() override;
};

#define PARTICLE_COUNT 1024*4
// Subpass Object

struct PM_IO_VULKAN_EXPORT Transparent_Model : public Model {

  UniformBufferTransparent trn_ubo{};

  Transparent_Model(std::string _path);

  void updateMapped() override;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void draw(VkCommandBuffer _buffer) override;

  VkDeviceSize getBufferSize() override;

  void createAdditinalBuffer() override;
};

struct PM_IO_VULKAN_EXPORT Terrian_Model : public Model {


  UniformBufferTessellation tesselation_ubo;

  explicit Terrian_Model(std::string _path,
                         model_type _type = model_type::FROMKTX);;

  VkPipelineTessellationStateCreateInfo tesselationState{};
  std::vector<VkDynamicState> dynamicStateEnables;

  void updateMapped() override;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  VkDeviceSize getBufferSize() override;;

  void setDescriptorLayout() override;

  void draw(VkCommandBuffer _buffer) override;

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void createAdditinalBuffer() override;
};

struct PM_IO_VULKAN_EXPORT GLTF_Model : public Object {
  UniformBufferObject gltf_ubo{};

  std::string obj_path;

  explicit GLTF_Model(std::string object_path);

  ~GLTF_Model() override;

  void initialization() override;

  void updateMapped() override;

  void updateUBO(UniformBufferObject *data, size_t mesh_id);

  Vertex vertex{};

  void destroy() override;

  uint32_t getNodesSize() const;

  uint32_t getLinearNodesSize() const;

  uint32_t getMaterialsSize() const;

  uint32_t getSkinSize() const;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void draw(VkCommandBuffer _buffer) override;

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override {
  }

  void createRenderPass(VkFormat format) override {
  }

  void createUniformBuffer() override;

  void set_visible_node(size_t index, bool flag);

  VkDeviceSize getBufferSize() override;

  // return texture descriptors
  uint32_t getTexturesSize() override;

  // change to current texture on all gltf scene
  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void createAdditinalBuffer() override;

  class GLTF_CImpl;

protected:
  std::unique_ptr<GLTF_CImpl> u_ptr_model;
};

struct PM_IO_VULKAN_EXPORT GLTF_Model_Animate : public Object {

  UniformBufferAnimate gltf_animate_ubo;
  struct DescriptorSetLayouts {
    VkDescriptorSetLayout matrices;
    VkDescriptorSetLayout textures;
    VkDescriptorSetLayout jointMatrices;
  } descriptorSetLayouts{};

//	struct desc_buff {
//		enma::Buffer buffer;
//		VkDescriptorSet descriptor;
//	};

  explicit GLTF_Model_Animate(std::string object_path);

  ~GLTF_Model_Animate() override;

  void initialization() override;

  void destroy() override;

  void update(float frame_time) override;

  void prepare() override;

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override {}

  void createUniformBuffer() override;

  VkDeviceSize getBufferSize() override;

  // return texture descriptors
  uint32_t getTexturesSize() override;

  // change to current texture on all gltf scene
  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void createDescriptorPool() override;

  void setDescriptorLayout() override;

  void createDescriptorSets() override;

  void preparePipeline() override;

  void updateMapped() override;

  void createRenderPass(VkFormat format) override;

  void draw(VkCommandBuffer _buffer) override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void createAdditinalBuffer() override;

  class AnimGLTF_Model_Impl;

private:
  std::string obj_path;
protected:
  std::unique_ptr<AnimGLTF_Model_Impl> u_ptr_model;
};

struct PM_IO_VULKAN_EXPORT GLTF_SkyBox : public GLTF_Model {
#define FB_DIM 256
#define FB_COLOR_FORMAT VK_FORMAT_R8G8B8A8_UNORM

  UniformBufferSkyBox skybox_ubo{};

  explicit GLTF_SkyBox(std::string object_path);

  void updateMapped() override;

  void destroy() override;

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void setDescriptorLayout() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  VkDeviceSize getBufferSize() override;

  void draw(VkCommandBuffer _buffer) override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void preparePipeline() override;

  void createUniformBuffer() override;

  void bloomRender();

  void prepareOffscreenFramebuffer(FrameBuffer *frameBuf, VkFormat colorFormat, VkFormat depthFormat);

  void createAdditinalBuffer() override;
};

// Max. number of chars the text overlay buffer can hold
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048
//TODO: Make universal TEXT OBJECT as PARENT for ANOTHER like OVERLAY OR TEXT ON BOARD
struct PM_IO_VULKAN_EXPORT TextOverlay : public Object {
private:
  uint32_t *frameBufferWidth;
  uint32_t *frameBufferHeight;
  float scale = 1.0;

  VkBuffer buffer{};
  VkSampler sampler{};
  VkImage image{};
  VkImageView view{};
  VkDeviceMemory imageMemory{};
  VkDeviceMemory memory{};
  VkDescriptorPool descriptorPool{};
  VkDescriptorSetLayout descriptorSetLayout{};
  VkCommandPool commandPool{};

  // Pointer to mapped vertex buffer
  glm::vec4 *mapped = nullptr;
  uint32_t numLetters{};

public:
  enum TextAlign {
    alignLeft,
    alignCenter,
    alignRight
  };

  bool visible = true;

  std::vector<VkCommandBuffer> cmdBuffers;

  TextOverlay();

  ~TextOverlay() override;

  // Prepare all vulkan resources required to render the font
  // The text overlay uses separate resources for descriptors (pool, sets, layouts), pipelines and command buffers
  void prepareResources();

  // Prepare a separate pipeline for the font rendering decoupled from the main application
  void preparePipeline() override;

  // Prepare a separate render pass for rendering the text as an overlay
  void prepareRenderPass();

  void createFramebuffers(VulkanSwapChain *vkSwapChain);

  void updateScale(float nScale);

  void updateFrameSize(uint32_t *width, uint32_t *height);

  // Map buffer
  void beginTextUpdate();

  // Add text to the current buffer
  // todo: drop shadow? color attribute?
  void addText(std::string text, float x, float y, TextAlign align);

  // Unmap buffer and update command buffers
  void endTextUpdate();

  void draw(VkCommandBuffer _buffer) override;

  void initialization() override;

  VkDeviceSize getBufferSize() override;

  uint32_t getTexturesSize() override;

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void loadTexture(VkImageViewType type) override;

  void destroy() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;

  void setDescriptorLayout() override;

  void createRenderPass(VkFormat format) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void createUniformBuffer() override;

  void updateMapped() override;

  void createAdditinalBuffer() override;
};

// // Vertex layout for this example
// struct Plane_Vertex
// {
//     float pos[3];
//     float uv[2];
//     float normal[3];
// };
struct PM_IO_VULKAN_EXPORT Model3D : public Object {
  UniformBufferObject model3d_ubo{};
  Vertex vertex{};
  enma::Buffer vertexBuffer;
  enma::Buffer indexBuffer;
  uint32_t indexCount{};
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  std::vector<Vertex> buff_vertices;
  std::vector<uint32_t> buff_indices;
  // For generated texture

  uint8_t *texture_data{};
  struct ImageInfo {
    uint32_t texture_width = 320;
    uint32_t texture_height = 256;
    uint32_t texture_depth = 4;
    uint32_t texture_byte_count = 4;
  } image;

  explicit Model3D(const std::vector<Vertex> &_vertices = std::vector<Vertex>(),
                   const std::vector<uint32_t> &_indices = std::vector<uint32_t>(),
                   uint8_t *_texture_data = nullptr);

  void initialization() override;

  void updateMapped() override;

  ~Model3D() override = default;

  void generateQuad(std::vector<Vertex> _vertices, std::vector<uint32_t> _indices);

  void destroy() override;

  void updateTexture(uint8_t *data);

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void preparePipeline() override;

  void draw(VkCommandBuffer _buffer) override;

  uint32_t getTexturesSize() override;;

  std::vector<uint32_t> *getIndices() override;

  void prepare() override {};

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  viBuffer *getBuffer() override;

  void update(float frame_time) override {}

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  VkDeviceSize getBufferSize() override;

  void createAdditinalBuffer() override;
};

struct PM_IO_VULKAN_EXPORT Model2D : public Object {
  UniformBuffer2D model2d_ubo{};
  Vertex vertex{};
  enma::Buffer vertexBuffer;
  enma::Buffer indexBuffer;
  uint32_t indexCount{};
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  VkDeviceSize bufferSize = sizeof(UniformBuffer2D);

  std::vector<Vertex> buff_vertices;
  std::vector<uint32_t> buff_indices;
  // For generated texture

  uint8_t *texture_data{};
  struct ImageInfo {
    uint32_t texture_width = 320;
    uint32_t texture_height = 256;
    uint32_t texture_depth = 4;
    uint32_t texture_byte_count = 4;
  } image;

  explicit Model2D(const std::vector<Vertex> &_vertices = std::vector<Vertex>(),
                   const std::vector<uint32_t> &_indices = std::vector<uint32_t>(),
                   uint8_t *_texture_data = nullptr);

  void initialization() override;

  void updateMapped() override;

  ~Model2D() override = default;

  void generateQuad(std::vector<Vertex> _vertices, std::vector<uint32_t> _indices);

  void destroy() override;

  void updateTexture(uint8_t *data);

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void preparePipeline() override;

  void draw(VkCommandBuffer _buffer) override;

  uint32_t getTexturesSize();;

  std::vector<uint32_t> *getIndices() override;

  void prepare() override {};

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  viBuffer *getBuffer() override;

  void update(float frame_time) override {}

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  VkDeviceSize getBufferSize() override;

  void createAdditinalBuffer() override;
};

struct PM_IO_VULKAN_EXPORT Line : public Object {

  UniformBufferLine line_ubo{};

  struct PushConstantTo {
    float line_thick = 1.0;
    int segments = 16;
    bool dash = false;
  } push_constants;

  Vertex vertex{};
  enma::Buffer vertexBuffer;
  enma::Buffer indexBuffer;
  uint32_t indexCount{};
  std::vector<uint32_t> indices;
  float line_thick = 1.f;
  glm::vec4 color = {0.0f, 0.0f, 0.0f, 1.0f};

  VkDeviceSize bufferSize = sizeof(UniformBufferObject);
  std::vector<Vertex> buff_vertices;

  explicit Line(std::vector<Vertex> _vertices = std::vector<Vertex>());

  void initialization() override;

  void updateMapped() override;

  ~Line() override = default;

  void generateQuad(std::vector<Vertex> vertices);

  void destroy() override;

  void updateLinePoints(const std::vector<Vertex> &new_points);

  // TODO: add texture load on line
  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void preparePipeline() override;

  void draw(VkCommandBuffer _buffer) override;

  uint32_t getTexturesSize() override;;

  std::vector<uint32_t> *getIndices() override;

  void prepare() override {
  };

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  viBuffer *getBuffer() override;

  void update(float frame_time) override {
  }

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  VkDeviceSize getBufferSize() override;

  void createAdditinalBuffer() override;
};

struct PickObject : public Object {

  UniformBufferCompute ubo_compute;
  enma::Buffer inputBuffer, outputBuffer;
  VkDeviceMemory inputBufferMemory;
  VkDeviceMemory outputBufferMemory;

  size_t dataSize = 0;


  PickObject();

  ~PickObject() override = default;


  void draw(VkCommandBuffer _buffer) override;

  void initialization() override;

  void updateMapped() override;

  void preparePipeline() override;

  void prepareCompute() const;

  void buildComputeCommandBuffer() const;

  void setDescriptorLayout() override;

  void prepareUniformBuffers();

  void loadTexture(VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D)  override;

  void prepareBuffers();

  void createDescriptorPool() override;

  void createDescriptorSets() override;

  void createAdditinalBuffer() override;

  void acquireBarrier(VkCommandBuffer _buffer) override;

  void releaseBarrier(VkCommandBuffer _buffer) override;

  void createSemaphore();

  VkDeviceSize getBufferSize() override;

  uint32_t getTexturesSize() override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void setObjectInfo(pipeline_parameters *_parameters, VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  void destroy() override;

  void additionalDestroy() override;

  void clearComputeBlock() override;
};