#pragma once

#include <SkyEngine/config/config.h>
#include <memory>
#ifndef QT_LIB_ENABLE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <algorithm>
#include <cstring>
#include <random>
#include <string>
#include <vector>

#include <SkyEngine/vk_sky_buffer.hpp>
#include <SkyEngine/vk_sky_object.hpp>

/**
 * @brief Vertex struct enum class
 */
enum class VertexComponent : std::uint8_t {
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

  auto operator==(const Vertex &other) const -> bool {
    return pos == other.pos && normal == other.normal && uv == other.uv &&
           color == other.color && joint0 == other.joint0 &&
           weight0 == other.weight0 && tangent == other.tangent;
  }

  static VkVertexInputBindingDescription vertexInputBindingDescription;
  static std::vector<VkVertexInputAttributeDescription>
      vertexInputAttributeDescriptions;
  static VkPipelineVertexInputStateCreateInfo
      pipelineVertexInputStateCreateInfo;
  /**
   * @brief Returns a vertex input binding description for the given binding
   * index.
   * @param binding The binding index.
   * @return A VkVertexInputBindingDescription structure describing the vertex
   * input binding.
   */
  static auto inputBindingDescription(uint32_t binding)
      -> VkVertexInputBindingDescription;
  /**
   * @brief Returns a vertex input attribute description for the given binding,
   * location, and component.
   * @param binding The binding index.
   * @param location The location index.
   * @param component The vertex component type.
   * @return A VkVertexInputAttributeDescription structure describing the vertex
   * attribute.
   */
  static auto inputAttributeDescription(uint32_t binding, uint32_t location,
                                        VertexComponent component)
      -> VkVertexInputAttributeDescription;
  /**
   * @brief Returns a vector of vertex input attribute descriptions for the
   * given binding and components.
   * @param binding The binding index.
   * @param components A vector of VertexComponent types.
   * @return A vector of VkVertexInputAttributeDescription structures describing
   * the vertex attributes.
   */
  static auto
  inputAttributeDescriptions(uint32_t binding,
                             std::vector<VertexComponent> components)
      -> std::vector<VkVertexInputAttributeDescription>;

  /** @brief Returns the default pipeline vertex input state create info
   * structure for the requested vertex components
   * @param components A vector of VertexComponent types.
   * @return A pointer to a VkPipelineVertexInputStateCreateInfo structure.
   */
  static auto
  getPipelineVertexInputState(std::vector<VertexComponent> components)
      -> VkPipelineVertexInputStateCreateInfo *;
};

namespace std {
template <> struct hash<Vertex> {
  auto operator()(Vertex const &vertex) const -> size_t {
    return ((hash<glm::vec3>()(vertex.pos) ^
             (hash<glm::vec4>()(vertex.color) ^
              (hash<glm::vec3>()(vertex.normal)) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.uv) << 1);
  }
};
} // namespace std

/**
 * @brief base struct for buffers what send data to shaders
 */
struct uniformBuffer {
  // Set common fields
  glm::uvec4 unique_id{0, 0, 0, 0};
};

/**
 * @brief uniform object buffer
 */
struct UniformBufferObject : public uniformBuffer {
  glm::mat4 model{};
  glm::mat4 view{};
  glm::mat4 proj{};
  glm::vec4 viewPos{};
  glm::vec4 lightPositon{};
};

/**
 * @brief buffer for line object
 */
struct UniformBufferLine : public uniformBuffer {
  glm::mat4 view{};
  glm::mat4 proj{};
  glm::vec4 viewPos{};
};

/**
 * @brief buffer for 2d objects
 *
 */
struct UniformBuffer2D : public uniformBuffer {
  glm::mat4 projection{};
  glm::mat4 model{};
  glm::mat4 view{};
  glm::vec4 viewPos{};
  glm::vec2 viewPortDim{};
};

/**
 * @brief buffer for sky box
 *
 */
struct UniformBufferSkyBox {
  glm::mat4 proj{};
  glm::mat4 view{};
  glm::mat4 model{};
};

/**
 * @brief uniform particle buffer
 */
struct UniformBufferParticle : public uniformBuffer {
  static constexpr float DEFAULT_POINT_SIZE = 10.0F;
  glm::mat4 projection{};
  glm::mat4 modelView{};
  glm::mat4 model{};
  glm::vec2 viewportDim{};
  float pointSize = DEFAULT_POINT_SIZE;
};

/**
 * @brief uniform shadows buffer
 */
struct UniformBufferShadows : public uniformBuffer {};

/**
 * @brief uniform tesselation buffer
 */
struct UniformBufferTessellation : public uniformBuffer {
  glm::mat4 projection{};
  glm::mat4 modelview{};
  glm::vec4 lightPos{};
  std::vector<glm::vec4> frustumPlanes;
  float displacementFactor = 0.0F;
  float tessellationFactor = 0.0F; ///< Tessellation factor for the patch
  glm::vec2 viewportDim{};
  // Desired size of tessellated quad patch edge
  float tessellatedEdgeSize = 0.0F;
};

/**
 * @brief buffer for transparent objects
 *
 */
struct UniformBufferTransparent : public uniformBuffer {
  glm::mat4 projection{};
  glm::mat4 modelview{};
  glm::mat4 model{};
  glm::vec4 lightPositon{};
};

/**
 * @brief buffer for particle gpu
 */
struct UniformBufferParticleGPU : public uniformBuffer {
  static constexpr int32_t DEFAULT_PARTICLE_COUNT = 256;
  float deltaT{};        //		Frame delta time
  float destX{};         //		x position of the attractor
  float destY{};         //		y position of the attractor
  float destZ{};         //		z position of the attractor
  float estLifetime = 1; //  time to live
  int32_t particleCount = DEFAULT_PARTICLE_COUNT;
};

/**
 * @brief buffer for particle gpu position
 */

constexpr std::size_t EMITTER_POSITION_ALIGNMENT = 16;

struct UniformBufferParticleGPUPosition : public uniformBuffer {
  glm::mat4 projection;
  glm::mat4 modelview;
  glm::mat4 model;
  glm::vec2 viewportDim;
  alignas(EMITTER_POSITION_ALIGNMENT) glm::vec3 emiter_position;
};

/**
 * @brief buffer for animate models
 */

struct UniformBufferAnimate : public uniformBuffer {
  glm::mat4 projection{};
  glm::mat4 view{};
  glm::vec4 lightPos{};
};

/**
 * @brief struct for send manage data to shader
 */
struct managePushConstant {
  glm::vec2 mousePos{0, 0};        ///< Mouse position vec2 for fragment shader
  uint32_t selected_unique_ID = 0; ///< For encapsulation model like gltf model
  uint32_t depth_array_value =
      DEPTH_ARRAY_SCALE; ///< Set depth array value for check on z coord
};

/**
 * @brief Particle enum class
 */
enum class ParticleComponent : std::uint8_t {
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
struct Particle {
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
  static std::vector<VkVertexInputAttributeDescription>
      vertexInputAttributeDescriptions;
  static VkPipelineVertexInputStateCreateInfo
      pipelineVertexInputStateCreateInfo;

  /**
   * @brief Returns a vertex input binding description for the given binding
   * index.
   * @param binding The binding index.
   * @return A VkVertexInputBindingDescription structure describing the vertex
   * input binding.
   * @details The binding description specifies the binding index, stride, and
   * input rate for the vertex input binding.
   * @note This function is static and can be called without an instance of the
   * Particle class.
   * @see VkVertexInputBindingDescription
   * @see VkPipelineVertexInputStateCreateInfo
   */
  static auto inputBindingDescription(uint32_t binding)
      -> VkVertexInputBindingDescription;
  /**
   * @brief Returns a vertex input attribute description for the given binding,
   * location, and component.
   * @param binding The binding index.
   * @param location The location index.
   * @param component The particle component type.
   * @return A VkVertexInputAttributeDescription structure describing the vertex
   * attribute.
   * @details The attribute description specifies the binding index, location,
   * format, and offset for the vertex input attribute.
   * @note This function is static and can be called without an instance of the
   * Particle class.
   * @see VkVertexInputAttributeDescription
   * @see VkPipelineVertexInputStateCreateInfo
   * @see ParticleComponent
   * @note The function is designed to work with the ParticleComponent enum
   * to determine the format and offset of the vertex attribute based on the
   * component type.
   * @note The function assumes that the binding index and location are
   * provided correctly and that the component type corresponds to a valid
   * vertex attribute.
   */
  static auto inputAttributeDescription(uint32_t binding, uint32_t location,
                                        ParticleComponent component)
      -> VkVertexInputAttributeDescription;

  static auto
  inputAttributeDescriptions(uint32_t binding,
                             std::vector<ParticleComponent> components)
      -> std::vector<VkVertexInputAttributeDescription>;

  /** @brief Returns the default pipeline vertex input state create info
   * structure for the requested vertex components */
  static auto
  getPipelineVertexInputState(std::vector<ParticleComponent> components)
      -> VkPipelineVertexInputStateCreateInfo *;
};

constexpr std::size_t PARTICLE_ALIGNMENT = 16;
/**
 * @brief ParticleGPU struct for GPU particle system
 * @details This struct is used to represent a particle in the GPU particle
 * system. It contains position, velocity, texture coordinates for the gradient
 * ramp map, random position, estimated lifetime, and actual lifetime.
 * @note The struct is aligned to PARTICLE_ALIGNMENT for optimal memory access.
 */
struct ParticleGPU {
  alignas(PARTICLE_ALIGNMENT) glm::vec3 pos;         ///< Particle position
  alignas(PARTICLE_ALIGNMENT) glm::vec3 vel;         ///< Particle velocity
  alignas(PARTICLE_ALIGNMENT) glm::vec3 gradientPos; ///< Texture
                                                     /// coordinates for
                                                     /// the gradient ramp
                                                     /// map
  alignas(PARTICLE_ALIGNMENT) glm::vec3 randomPos;
  float estLifetime;
  float lifeTime;
};

struct Vert {
  int count{};
  enma::Buffer buffer;
  Vert() = default;
};

struct Ind {
  int count{};
  enma::Buffer buffer;
};

/**
 * @brief model type struct enum class
 */
enum class model_type : uint8_t { from_obj_file, from_ktx_file };

/**
 * @brief Model class
 */
struct LIBSKYENGINE_EXPORT Model : public Object {
public:
  UniformBufferObject
      object_ubo{}; /// TODO: сделать приватным и ограничить вносимые измнения
  Vertex vertex{};

  uint32_t *_buffer{}, *indeces{}, _shape_num{};
  model_type type;

  glm::vec3 currentPos;

  // Copy constructor
  Model(const Model &other) = delete;
  // Copy assignment operator
  auto operator=(const Model &other) -> Model & = delete;
  // Move constructor
  Model(Model &&other) noexcept = delete;
  // Move assignment operator
  auto operator=(Model &&other) noexcept -> Model & = delete;

  Model(uint32_t *buffer, uint32_t *_ind, uint32_t shape_num,
        model_type _type = model_type::from_obj_file);

  void initialization() override;

  void updateMapped() override;

  explicit Model(std::string _path,
                 model_type _type = model_type::from_obj_file);

  void draw(VkCommandBuffer model_buffer) override;

  ~Model() override;

  auto getOrintationMat(glm::vec3 vec, glm::vec3 start_orientation)
      -> glm::mat4;

  auto getRotationVector(glm::vec3 point_to, glm::vec3 point_from,
                         bool counterclockwise, glm::vec3 rotate_axis)
      -> glm::vec3;

  auto get_descriptor_image(size_t tex_idx) -> VkDescriptorImageInfo * override;

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

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void bindBuffers(VkCommandBuffer commandBuffer);

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  /**
   *
   */
  VkDeviceSize getBufferSize() override;
  ;

  uint32_t getTexturesSize() override;
  ;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;
  ;

  void update(float frame_time) override;
  ;

  void prepare() override;
  ;

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createUniformBuffer() override;

  void createAdditinalBuffer() override;

  void LoadModelFromFile(std::string path);

  void GenerateModelFromFile(std::string path);

  void GenerateModelFromBuffer(uint32_t *buffer, uint32_t *_ind, uint32_t size,
                               uint32_t shape_num);

  void createIndexBuffer();

  void createVertexBuffer();

private:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::string path;

  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

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

struct LIBSKYENGINE_EXPORT Partical_Model_CPU : public Object {
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

  // API for particle
  /**
   *
   * @param _count
   */
  void set_count(unsigned int _count);
  ;

  /**
   *
   * @param _radius
   */
  void set_radius(float _radius);
  ;

  /**
   *
   * @param position
   * TODO: create glm::vec4 for change direction
   */
  void set_position(glm::vec3 position);
  ;

  // Поовернуть в заданном направлении
  void set_direction(glm::vec3 direction);

  /**
   *
   * @param value
   */
  void set_alpha(float value);

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

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void destroy() override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  VkDeviceSize getBufferSize() override;

  uint32_t getTexturesSize() override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;
  ;

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

struct LIBSKYENGINE_EXPORT Partical_Model_GPU final : public Object {
  struct {
    VkPipelineVertexInputStateCreateInfo inputState;
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
  } vertices;

  UniformBufferParticleGPU ubo_gpu_particl;
  UniformBufferParticleGPUPosition ubo_pos_particle;
  //  enma::Buffer storageBuffer;

  ParticleGPU particle{};

  Partical_Model_GPU();

  ~Partical_Model_GPU() override = default;

  /**
   * @brief change count of emitted particles
   * @param _count
   */
  void set_count(unsigned int _count);
  ;

  /**
   * @brief function set radius of emitted particles
   * @param _radius
   */
  void set_radius(float _radius);
  ;

  /**
   * @brief function for set new position of emitted particles center
   * @param position
   */
  void set_position(glm::vec3 position);
  ;

  /**
   * @brief function for rotate to direction
   * @param direction
   */
  void set_direction(glm::vec3 direction);

  /**
   * @brief function set alpha value for emitted particles
   * @param value
   */
  void set_alpha(float value);

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

  void prepareCompute();

  void buildComputeCommandBuffer();

  void setDescriptorLayout() override;

  void prepareUniformBuffers();

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

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

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

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

#define PARTICLE_COUNT 1024 * 4
// Subpass Object

struct LIBSKYENGINE_EXPORT Transparent_Model final : public Model {
  UniformBufferTransparent trn_ubo{};

  Transparent_Model(std::string _path);

  void updateMapped() override;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void draw(VkCommandBuffer _buffer) override;

  VkDeviceSize getBufferSize() override;

  void createAdditinalBuffer() override;
};

struct LIBSKYENGINE_EXPORT Terrain_Model final : public Model {
  UniformBufferTessellation tesselation_ubo;

  explicit Terrain_Model(std::string _path,
                         model_type _type = model_type::from_ktx_file);
  ;

  VkPipelineTessellationStateCreateInfo tesselationState{};
  std::vector<VkDynamicState> dynamicStateEnables;

  void updateMapped() override;

  void preparePipeline() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  VkDeviceSize getBufferSize() override;
  ;

  void setDescriptorLayout() override;

  void draw(VkCommandBuffer _buffer) override;

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void createAdditinalBuffer() override;

private:
  bool tesselation = false;

  struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
  };

  // heightmap — массив float размером width*height

  void buildSphereWithHeightmap(int width, int height, float R, float hScale,
                                const std::vector<float> &heightmap,
                                std::vector<Vertex> &vertices,
                                std::vector<uint32_t> &indices) {
    vertices.clear();
    indices.clear();
    vertices.reserve(width * height);
    indices.reserve((width - 1) * (height - 1) * 6);

    // 1. Вершины с UV
    for (int j = 0; j < height; ++j) {
      float v = float(j) / (height - 1);
      float θ = v * M_PI;

      for (int i = 0; i < width; ++i) {
        float u = float(i) / (width - 1);
        float φ = u * 2.0f * M_PI;

        float x = sin(θ) * cos(φ);
        float y = cos(θ);
        float z = sin(θ) * sin(φ);

        float h = heightmap[j * width + i] * hScale;
        glm::vec3 pos = glm::vec3(x, y, z) * (R + h);
        vertices.push_back({pos, glm::vec3(0), glm::vec2(u, v)});
      }
    }

    // 2. Нормали
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        int iL = std::max(i - 1, 0), iR = std::min(i + 1, width - 1);
        int jD = std::max(j - 1, 0), jU = std::min(j + 1, height - 1);

        glm::vec3 pL = vertices[j * width + iL].pos;
        glm::vec3 pR = vertices[j * width + iR].pos;
        glm::vec3 pD = vertices[jD * width + i].pos;
        glm::vec3 pU = vertices[jU * width + i].pos;

        glm::vec3 dx = pR - pL;
        glm::vec3 dz = pU - pD;
        glm::vec3 n = glm::normalize(glm::cross(dz, dx));

        vertices[j * width + i].normal = n;
      }
    }

    // 3. Индексы
    for (int j = 0; j < height - 1; ++j) {
      for (int i = 0; i < width - 1; ++i) {
        uint32_t cur = j * width + i;
        uint32_t next = (j + 1) * width + i;

        indices.push_back(cur);
        indices.push_back(next);
        indices.push_back(next + 1);

        indices.push_back(cur);
        indices.push_back(next + 1);
        indices.push_back(cur + 1);
      }
    }
  }
};

struct LIBSKYENGINE_EXPORT GLTF_Model : public Object {
  UniformBufferObject gltf_ubo{};

  managePushConstant manage_constant;

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

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void update(float frame_time) override;

  void prepare() override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void draw(VkCommandBuffer _buffer) override;

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override {}

  void createRenderPass(VkFormat format) override {}

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

  void releaseBarrier(VkCommandBuffer _buffer) override;

  void readShaderData() override;

  /**
   * @brief function return id number of selected object in gltf model
   * if 0 not selected any object
   * @return id number of selected object in gltf model
   */
  uint32_t idSelected() const;

  class GLTF_CImpl;

private:
  void prepareInputs();

  void prepareCompute();

  void prepareBuildComputeBuffer();

  void prepareUniformBuffers();

  glm::vec2 mouse_vec2_;
  uint32_t selectedId = 0;

protected:
  std::unique_ptr<GLTF_CImpl> u_ptr_model;
};

struct LIBSKYENGINE_EXPORT GLTF_Model_Animate final : public Object {
  UniformBufferAnimate gltf_animate_ubo;

  struct DescriptorSetLayouts {
    VkDescriptorSetLayout matrices;
    VkDescriptorSetLayout textures;
    VkDescriptorSetLayout jointMatrices;
  } descriptorSetLayouts{};

  explicit GLTF_Model_Animate(std::string object_path);

  ~GLTF_Model_Animate() override;

  void initialization() override;

  void destroy() override;

  void update(float frame_time) override;

  void prepare() override;

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

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

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void createAdditinalBuffer() override;

  class AnimGLTF_Model_Impl;

private:
  std::string obj_path;

protected:
  std::unique_ptr<AnimGLTF_Model_Impl> u_ptr_model;
};

struct LIBSKYENGINE_EXPORT GLTF_SkyBox final : public GLTF_Model {
#define FB_DIM 256
#define FB_COLOR_FORMAT VK_FORMAT_R8G8B8A8_UNORM

  UniformBufferSkyBox skybox_ubo{};

  explicit GLTF_SkyBox(std::string object_path);

  void updateMapped() override;

  void destroy() override;

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void setDescriptorLayout() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  VkDeviceSize getBufferSize() override;

  void draw(VkCommandBuffer _buffer) override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void preparePipeline() override;

  void createUniformBuffer() override;

  void bloomRender();

  void prepareOffscreenFramebuffer(FrameBuffer *frameBuf, VkFormat colorFormat,
                                   VkFormat depthFormat);

  void createAdditinalBuffer() override;
};

// Max. number of chars the text overlay buffer can hold
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048

// TODO: Make universal TEXT OBJECT as PARENT for ANOTHER like OVERLAY OR TEXT
// ON BOARD
struct LIBSKYENGINE_EXPORT TextOverlay : public Object {

  UniformBuffer2D text_ubo;

  struct PushConstBlock {
    glm::vec2 scale = {0, 0};
    glm::vec2 translate = {0, 0};
  } pushConstBlock;

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
  // VkDescriptorPool descriptorPool{};
  // VkDescriptorSetLayout descriptorSetLayout{};
  VkCommandPool commandPool{};

  // Pointer to mapped vertex buffer
  glm::vec4 *mapped = nullptr;
  uint32_t numLetters{};

public:
  enum TextAlign { alignLeft, alignCenter, alignRight };

  bool visible = true;

  std::vector<VkCommandBuffer> cmdBuffers;

  TextOverlay();

  ~TextOverlay() override;

  // Prepare all vulkan resources required to render the font
  // The text overlay uses separate resources for descriptors (pool, sets,
  // layouts), pipelines and command buffers
  void prepareResources();

  // Prepare a separate pipeline for the font rendering decoupled from the main
  // application
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

  void addPlateText(std::string text, float x, float y, TextAlign align);

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

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

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

struct LIBSKYENGINE_EXPORT Model3D final : public Object {
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

  explicit Model3D(
      const std::vector<Vertex> &_vertices = std::vector<Vertex>(),
      const std::vector<uint32_t> &_indices = std::vector<uint32_t>(),
      uint8_t *_texture_data = nullptr);

  void initialization() override;

  void updateMapped() override;

  ~Model3D() override = default;

  void generateQuad(std::vector<Vertex> _vertices,
                    std::vector<uint32_t> _indices);

  void destroy() override;

  void updateTexture(uint8_t *data);

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void preparePipeline() override;

  void draw(VkCommandBuffer _buffer) override;

  uint32_t getTexturesSize() override;
  ;

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

struct LIBSKYENGINE_EXPORT Model2D final : public Object {
  UniformBuffer2D model2d_ubo{};

  TextOverlay *text;

  /**
   * For geometry shader
   */
  struct PushConstantTo {
    float line_thick = 1.0;
    int segments = 16;
    bool dash = false;
  } push_constants;

  struct PushConstBlock {
    glm::vec2 scale;
    glm::vec2 translate;
  } pushConstBlock;

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

  explicit Model2D(
      const std::vector<Vertex> &_vertices = std::vector<Vertex>(),
      const std::vector<uint32_t> &_indices = std::vector<uint32_t>(),
      uint8_t *_texture_data = nullptr);

  void initialization() override;

  void updateMapped() override;

  ~Model2D() override { delete text; }

  void modelVertexUpdate(const std::vector<Vertex> &data);
  ////////////// Text Overlay function BLOCK BEGIN ////////////////
  /**
   * @brief  Update and base settings for begin to draw text
   */
  void plateTextUpdate();

  /**
   * @brief add string to plate function
   * @param str
   * @param x
   * @param y
   */
  void addTextToPlate(const std::string &str, float x, float y);

  /**
   * @brief End text update
   */
  void plateTextEnd();

  ///////////// Text Overlay function BLOCK END //////////////
  /**
   *
   * @param _vertices
   * @param _indices
   */
  void generateQuad(std::vector<Vertex> _vertices,
                    std::vector<uint32_t> _indices);

  void destroy() override;

  void updateTexture(uint8_t *data);

  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void preparePipeline() override;

  void draw(VkCommandBuffer _buffer) override;

  uint32_t getTexturesSize();
  ;

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

struct LIBSKYENGINE_EXPORT Line : public Object {
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
  void loadTexture(
      VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) override;

  void preparePipeline() override;

  void draw(VkCommandBuffer _buffer) override;

  uint32_t getTexturesSize() override;
  ;

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

/**
 * @brief ui form draw classes
 * save form data like - position of draw start on screen
 */
struct LIBSKYENGINE_EXPORT UIForm {
  enum AlignFormRule { AlignNext, AlignDown, AlignBackGround };

  explicit UIForm(Object *obj_ptr = nullptr, UIForm *ptr = nullptr)
      : object(obj_ptr) {
    if (ptr) {
      // screen_start_draw_position = ptr->screen_start_draw_position;
      // bounding_rect = ptr->bounding_rect;
      parent = ptr;
    }
  }

  // glm::uvec2 screen_start_draw_position = {0, 0};
  // glm::uvec4 bounding_rect = {0, 0, 0, 0};
  ///< area of draw form increase then add new form elements
  ///< add new element to current form
  void Add(UIForm *form) {
    child_list.emplace_back(form); ///< add form to
    form->parent = this;
  }

  UIForm *getForm() { return this; }

  uint32_t getWidth() { return width; }

  uint32_t getHeight() { return height; }

  uint32_t getDrawXPos() const { return start_x + anchors.x; }

  uint32_t getDrawYPos() const { return start_y + anchors.y; }

  uint32_t getXPos() const { return start_x; }

  uint32_t getYPos() const { return start_y; }

  uint32_t getNextXPos() const { return next_form_x; }

  uint32_t getNextYPos() const { return next_form_y; }

  void set_draw_str_point(uint32_t x, uint32_t y) {
    start_x = x; //+ anchors.x;
    start_y = y; //+ anchors.y;
  }

  Object *getObject() const { return object; }

  UIForm *getParent() const { return parent; }

  void make_update() {
    if (parent)
      parent->make_update();
    update();
  }

  void setAlignRule(const AlignFormRule _rule) { rule = _rule; }

  glm::uvec2 getShapeRect() const { return shape_rect; }

  void changeAnchors(const glm::uvec4 anchor_) { anchors = anchor_; }

private:
  std::vector<UIForm *> child_list;
  Object *object;
  AlignFormRule rule = AlignNext;
  glm::uvec2 shape_rect{};
  glm::uvec4 anchors = {10, 10, 10, 10};

  // Корректируем следующую точку отрисовки
  void increase(UIForm *form) {
    if (form->rule == AlignNext) {
      next_form_x =
          start_x + width +
          anchors.x; // ширина должна браться согласно общей ширине формы
      next_form_y = start_y;
    }

    if (form->rule == AlignDown) {
      next_form_x = start_x;
      next_form_y =
          start_y + height +
          anchors.y; // высота должна браться согласно общей высоте формы
    }

    if (form->rule == AlignBackGround) {
      next_form_x = start_x;
      next_form_y =
          start_y; // высота должна браться согласно общей высоте формы
    }
    form->set_draw_str_point(next_form_x, next_form_y);
    // на каждую форму в списке должен произойти рекурсивный расчет ширины
  };
  // Необходимо считать общие параметры для формы включая всех элементов формы
  glm::uvec2 meas_size(const UIForm *form = nullptr, uint32_t w_ = 0,
                       uint32_t h_ = 0) const {
    glm::uvec2 size_form{};
    if (!form) {
      form = this;
    }

    size_form =
        glm::uvec2(form->start_x + form->width, form->start_y + form->height);

    if (size_form.x < w_)
      size_form.x = w_;
    if (size_form.y < h_)
      size_form.y = h_;

    for (const auto frm : form->child_list) {
      size_form = meas_size(frm, size_form.x, size_form.y);
    }

    return size_form;
  }

  void clear() {}

  void resize_form(UIForm *frm) {
    frm->shape_rect = meas_size(frm) + glm::uvec2(0, anchors.w);
  }

protected:
  UIForm *parent = nullptr; ///< previous form ptr if nullptr current form first
  uint32_t width = 0;       ///< form width
  uint32_t height = 0;      ///< form height
  uint32_t start_x = 0;     ///< start draw x coord
  uint32_t start_y = 0;     ///< start draw y coord
  uint32_t next_form_x = 0; ///< start draw x coord for form
  uint32_t next_form_y = 0; ///< start draw y coord for form
  ///< make update for every size change
  ///

  void update() {
    if (!parent)
      resize_form(this);
    if (!child_list.empty()) {
      clear();
      for (auto frm = child_list.begin(); frm != child_list.end(); ++frm) {
        /// Чтобы правильно расположить новое поле надо расчитать
        /// полную ширину высоту предыдущей формы с ее наследниками
        if (frm == child_list.begin()) {
          increase(*frm);
        } else {
          auto prev = frm;
          --prev;

          auto rect_sh = meas_size((*prev));
          // get previous position
          if ((*frm)->rule == AlignNext) {
            next_form_x = rect_sh.x; //+ width; // ширина должна браться
                                     // согласно общей ширине формы
            next_form_y = (*prev)->getYPos();
          }
          if ((*frm)->rule == AlignDown) {
            next_form_x = (*prev)->getXPos();
            next_form_y =
                rect_sh.y + anchors.y; //+ height; // высота должна браться
                                       // согласно общей высоте формы
          }
          if ((*frm)->rule == AlignBackGround) {
            next_form_x = (*prev)->getParent()->getNextXPos();
            next_form_y = (*prev)->getParent()->getNextYPos();
          }
          (*frm)->set_draw_str_point(next_form_x, next_form_y);
        }
        resize_form((*frm));
      }
    }
  }

  void setWidth(uint32_t _width) { width = _width + anchors.x + anchors.z; }

  void setHeight(uint32_t _height) { height = _height + anchors.y + anchors.w; }

  void removeChild(const UIForm *child) {
    auto iter_child = std::find(child_list.begin(), child_list.end(), child);
    if (iter_child != child_list.end())
      child_list.erase(iter_child);
  }
};

struct LIBSKYENGINE_EXPORT TextForm : public UIForm, Object {
  UniformBuffer2D text_ubo;

  /// @brief
  /// @param position
  /// @param ptr
  TextForm();

  /// @brief
  ~TextForm() override;

  void init(UIForm *ptr = nullptr);

  glm::vec2 shift_position; ///< shift position  text on
  uint32_t text_height = 0; ///< calculate parameter then add some text, take
                            ///< max height of font symbol
  // For draw Text use TextOverlay
  // For draw Plate use Model2d
  struct PushConstantBlock {
    glm::vec2 scale = {0, 0};
    glm::vec2 translate = {0, 0};
  } pushConstBlock;

  enum TextAlign { alignLeft, alignCenter, alignRight };

  ///@brief Prepare a separate pipeline for the font rendering decoupled from
  /// the main application
  void preparePipeline() override;

  /// @brief
  /// @param nScale
  void updateScale(float nScale);

  /// @brief
  /// @param width
  /// @param height
  void updateFrameSize(uint32_t *width, uint32_t *height);

  /// @brief Map buffer
  void beginTextUpdate();

  /// @brief
  /// @param text
  /// @param align
  void addText(std::string text, TextAlign align);

  /// @brief Unmap buffer and update command buffers
  void endTextUpdate();

  /// @brief
  /// @param _buffer
  void draw(VkCommandBuffer _buffer) override;

  /// @brief
  void initialization() override;

  /// @brief
  /// @return
  VkDeviceSize getBufferSize() override;

  /// @brief
  /// @return
  uint32_t getTexturesSize() override;

  /// @brief
  /// @param tex_idx
  /// @return
  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  /// @param vkSwapChain
  void createFramebuffers(VulkanSwapChain *vkSwapChain) override {};

  /// @brief
  /// @return
  viBuffer *getBuffer() override;

  /// @brief
  /// @return
  std::vector<uint32_t> *getIndices() override;

  /// @brief
  /// @param type
  void loadTexture(VkImageViewType type) override;

  /// @brief
  void destroy() override;

  /// @brief
  /// @param _parameters
  /// @param pipelineInfo
  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  /// @brief
  void setDescriptorLayout() override;

  /// @brief
  void createDescriptorSets() override;

  /// @brief
  void createDescriptorPool() override;

  /// @brief
  void createUniformBuffer() override;

  /// @brief
  void updateMapped() override;

  void update(float frame_time) override;

  void prepare() override;

  void createRenderPass(VkFormat format) override;

  void createAdditinalBuffer() override;

  void readShaderData() override;

  /// @brief
  uint32_t *frameBufferWidth;
  uint32_t *frameBufferHeight;
  float scale = 1.0;

  uint32_t selectedId = 0;

  managePushConstant manage_constant;

private:
  VkBuffer buffer{};
  VkSampler sampler{};
  VkImage image{};
  VkImageView view{};
  VkDeviceMemory imageMemory{};
  VkDeviceMemory memory{};
  VkCommandPool commandPool{};
  // Pointer to mapped vertex buffer
  glm::vec4 *mapped = nullptr;
  uint32_t numLetters{};

public:
  bool visible = true;
};

struct LIBSKYENGINE_EXPORT ShapeForm : public UIForm, Object {
  ShapeForm();

  void init(UIForm *ptr = nullptr);

  void recreate_vertices();

  void initialization() override;

  VkDeviceSize getBufferSize() override;

  uint32_t getTexturesSize() override;

  VkDescriptorImageInfo *get_descriptor_image(size_t tex_idx) override;

  viBuffer *getBuffer() override;

  std::vector<uint32_t> *getIndices() override;

  void loadTexture(VkImageViewType type) override;

  void destroy() override;

  void setObjectInfo(pipeline_parameters *_parameters,
                     VkGraphicsPipelineCreateInfo *pipelineInfo) override;

  void preparePipeline() override;

  void update(float frame_time) override;

  void prepare() override;

  void draw(VkCommandBuffer _buffer) override;

  void setDescriptorLayout() override;

  void createFramebuffers(VulkanSwapChain *vkSwapChain) override;

  void createRenderPass(VkFormat format) override;

  void createDescriptorSets() override;

  void createDescriptorPool() override;

  void createUniformBuffer() override;

  void updateMapped() override;

  void createAdditinalBuffer() override;

  void updateTexture(uint8_t *data);

  struct PushConstBlock {
    glm::vec2 scale;
    glm::vec2 translate;
  } pushConstBlock{};

  uint8_t *texture_data{};

  struct ImageInfo {
    uint32_t texture_width = 320;
    uint32_t texture_height = 256;
    uint32_t texture_depth = 4;
    uint32_t texture_byte_count = 4;
  } image;

  Vertex vertex{};
  enma::Buffer vertexBuffer;
  enma::Buffer indexBuffer;
  uint32_t indexCount{};
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  VkDeviceSize bufferSize = sizeof(UniformBuffer2D);

  UniformBuffer2D model2d_ubo{};
};
