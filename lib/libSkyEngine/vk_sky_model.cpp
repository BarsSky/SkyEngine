#include "vk_sky_model.hpp"
#include "animodel_impl.h"
#include "extension/stb/stb_font_consolas_24_latin1.inl"
#include "gltf_model_impl.h"
#include "vk_sky_objcamera.hpp"
#include <unordered_map>
#include <utility>

#define TINYOBJLOADER_IMPLEMENTATION

#include <glm/gtx/quaternion.hpp>

#include "extension/tinyobjloader/tiny_obj_loader.h"
#include "tools.hpp"
#include "vk_sky_model.hpp"

// For font overlay
stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS]{};

VkVertexInputBindingDescription Vertex::vertexInputBindingDescription;
std::vector<VkVertexInputAttributeDescription>
    Vertex::vertexInputAttributeDescriptions;
VkPipelineVertexInputStateCreateInfo Vertex::pipelineVertexInputStateCreateInfo;

VkVertexInputBindingDescription
Vertex::inputBindingDescription(uint32_t binding) {
  return VkVertexInputBindingDescription(
      {binding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX});
}

VkVertexInputAttributeDescription
Vertex::inputAttributeDescription(uint32_t binding, uint32_t location,
                                  VertexComponent component) {
  switch (component) {
  case VertexComponent::Position:
    return VkVertexInputAttributeDescription(
        {location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)});
  case VertexComponent::Normal:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32_SFLOAT,
                                              offsetof(Vertex, normal)});
  case VertexComponent::UV:
    return VkVertexInputAttributeDescription(
        {location, binding, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
  case VertexComponent::Color:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32A32_SFLOAT,
                                              offsetof(Vertex, color)});
  case VertexComponent::Tangent:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32A32_SFLOAT,
                                              offsetof(Vertex, tangent)});
  case VertexComponent::Joint0:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32A32_SFLOAT,
                                              offsetof(Vertex, joint0)});
  case VertexComponent::Weight0:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32A32_SFLOAT,
                                              offsetof(Vertex, weight0)});
  default:
    return VkVertexInputAttributeDescription({});
  }
}

std::vector<VkVertexInputAttributeDescription>
Vertex::inputAttributeDescriptions(
    uint32_t binding, const std::vector<VertexComponent> components) {
  std::vector<VkVertexInputAttributeDescription> result;
  uint32_t location = 0;
  for (VertexComponent component : components) {
    result.push_back(
        Vertex::inputAttributeDescription(binding, location, component));
    location++;
  }
  return result;
}

/** @brief Returns the default pipeline vertex input state create info structure
 * for the requested vertex components */
VkPipelineVertexInputStateCreateInfo *Vertex::getPipelineVertexInputState(
    const std::vector<VertexComponent> components) {
  vertexInputBindingDescription = Vertex::inputBindingDescription(0);
  Vertex::vertexInputAttributeDescriptions =
      Vertex::inputAttributeDescriptions(0, components);
  pipelineVertexInputStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions =
      &Vertex::vertexInputBindingDescription;
  pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(Vertex::vertexInputAttributeDescriptions.size());
  pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions =
      Vertex::vertexInputAttributeDescriptions.data();
  return &pipelineVertexInputStateCreateInfo;
}

// Particle

VkVertexInputBindingDescription Particle::vertexInputBindingDescription;
std::vector<VkVertexInputAttributeDescription>
    Particle::vertexInputAttributeDescriptions;
VkPipelineVertexInputStateCreateInfo
    Particle::pipelineVertexInputStateCreateInfo;

VkVertexInputBindingDescription
Particle::inputBindingDescription(uint32_t binding) {
  return VkVertexInputBindingDescription(
      {binding, sizeof(Particle), VK_VERTEX_INPUT_RATE_VERTEX});
}

VkVertexInputAttributeDescription
Particle::inputAttributeDescription(uint32_t binding, uint32_t location,
                                    ParticleComponent component) {
  switch (component) {
  case ParticleComponent::Position:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32A32_SFLOAT,
                                              offsetof(Particle, pos)});
  case ParticleComponent::Color:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32G32B32A32_SFLOAT,
                                              offsetof(Particle, color)});
  case ParticleComponent::Alpha:
    return VkVertexInputAttributeDescription(
        {location, binding, VK_FORMAT_R32_SFLOAT, offsetof(Particle, alpha)});
  case ParticleComponent::Size:
    return VkVertexInputAttributeDescription(
        {location, binding, VK_FORMAT_R32_SFLOAT, offsetof(Particle, size)});
  case ParticleComponent::Rotation:
    return VkVertexInputAttributeDescription({location, binding,
                                              VK_FORMAT_R32_SFLOAT,
                                              offsetof(Particle, rotation)});
  case ParticleComponent::Type:
    return VkVertexInputAttributeDescription(
        {location, binding, VK_FORMAT_R32_SINT, offsetof(Particle, type)});
  default:
    return VkVertexInputAttributeDescription({});
  }
}

std::vector<VkVertexInputAttributeDescription>
Particle::inputAttributeDescriptions(
    uint32_t binding, const std::vector<ParticleComponent> components) {
  std::vector<VkVertexInputAttributeDescription> result;
  uint32_t location = 0;
  for (ParticleComponent component : components) {
    result.push_back(
        Particle::inputAttributeDescription(binding, location, component));
    location++;
  }
  return result;
}

/** @brief Returns the default pipeline particle input state create info
 * structure for the requested particle components */
VkPipelineVertexInputStateCreateInfo *Particle::getPipelineVertexInputState(
    const std::vector<ParticleComponent> components) {
  vertexInputBindingDescription = Particle::inputBindingDescription(0);
  Particle::vertexInputAttributeDescriptions =
      Particle::inputAttributeDescriptions(0, components);
  pipelineVertexInputStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions =
      &Particle::vertexInputBindingDescription;
  pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(Particle::vertexInputAttributeDescriptions.size());
  pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions =
      Particle::vertexInputAttributeDescriptions.data();
  return &pipelineVertexInputStateCreateInfo;
}

//* Model

VkDescriptorImageInfo *Model::get_descriptor_image(size_t tex_idx) {
  return &textures.at(tex_idx)->descriptor;
}

void Model::setObjectInfo(pipeline_parameters *_parameters,
                          VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(5);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      vertex.inputAttributeDescription(0, 3, VertexComponent::Color),
      // vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent)
  };

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  _parameters->colorBlendAttachment->blendEnable = VK_FALSE;
  _parameters->depthStencil->depthWriteEnable = VK_TRUE;
  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void Model::bindBuffers(VkCommandBuffer buffer) {
  const VkDeviceSize offsets[1] = {0};
  vkCmdBindVertexBuffers(buffer, 0, 1, &vert.buffer.buffer, offsets);
  vkCmdBindIndexBuffer(buffer, ind.buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
  buffersBound = true;
}

void Model::GenerateModelFromFile(std::string filename) {
#define PATCH_SIZE 64
#define UV_SCALE 1.0f

  const uint32_t vertexCount = PATCH_SIZE * PATCH_SIZE;
  // We use the Vertex definition from the glTF model loader, so we can re-use
  // the vertex input state
  vertices.resize(vertexCount);

  const float wx = 2.0f;
  const float wy = 2.0f;

  for (auto x = 0; x < PATCH_SIZE; x++) {
    for (auto y = 0; y < PATCH_SIZE; y++) {
      uint32_t index = (x + y * PATCH_SIZE);
      vertices[index].pos[0] =
          x * wx + wx / 2.0f - (float)PATCH_SIZE * wx / 2.0f;
      vertices[index].pos[1] = 0.0f;
      vertices[index].pos[2] =
          y * wy + wy / 2.0f - (float)PATCH_SIZE * wy / 2.0f;
      vertices[index].uv =
          glm::vec2((float)x / PATCH_SIZE, (float)y / PATCH_SIZE) * UV_SCALE;
    }
  }

  // Calculate normals from height map using a sobel filter

  HeightMap heightMap(filename, PATCH_SIZE);

  for (auto x = 0; x < PATCH_SIZE; x++) {
    for (auto y = 0; y < PATCH_SIZE; y++) {
      // Get height samples centered around current position
      float heights[3][3];
      for (auto hx = -1; hx <= 1; hx++) {
        for (auto hy = -1; hy <= 1; hy++) {
          heights[hx + 1][hy + 1] = heightMap.getHeight(x + hx, y + hy);
        }
      }

      // Calculate the normal
      glm::vec3 normal;
      // Gx sobel filter
      normal.x = heights[0][0] - heights[2][0] + 2.0f * heights[0][1] -
                 2.0f * heights[2][1] + heights[0][2] - heights[2][2];
      // Gy sobel filter
      normal.z = heights[0][0] + 2.0f * heights[1][0] + heights[2][0] -
                 heights[0][2] - 2.0f * heights[1][2] - heights[2][2];
      // Calculate missing up component of the normal using the filtered x and y
      // axis The first value controls the bump strength
      normal.y = 0.25f * sqrt(1.0f - normal.x * normal.x - normal.z * normal.z);

      vertices[x + y * PATCH_SIZE].normal =
          glm::normalize(normal * glm::vec3(2.0f, 1.0f, 2.0f));
      vertices[x + y * PATCH_SIZE].color = {1.0f, 1.0f, 1.0f, 1.0f};
    }
  }

  // Indices
  const uint32_t w = (PATCH_SIZE - 1);
  const uint32_t indexCount = w * w * 4;
  indices.resize(indexCount);
  for (auto x = 0; x < w; x++) {
    for (auto y = 0; y < w; y++) {
      uint32_t index = (x + y * w) * 4;
      indices[index] = (x + y * PATCH_SIZE);
      indices[index + 1] = indices[index] + PATCH_SIZE;
      indices[index + 2] = indices[index + 1] + 1;
      indices[index + 3] = indices[index] + 1;
    }
  }
}

void Model::GenerateModelFromBuffer(uint32_t *buffer, uint32_t *_ind,
                                    uint32_t size, uint32_t shape_num) {
  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  uint32_t highs = size / 3;
  // количество плоскостей
  for (uint32_t i = 0; i < shape_num; i++) {
    // количество вершинных точек внутри плоскости
    for (uint32_t j = 0; j < highs; j += 3) {
      Vertex _vertex{};

      _vertex.pos = {
          buffer[i + j + 0],
          buffer[i + j + 1],
          buffer[i + j + 2],
      };

      // _vertex.uv = {
      // 	attrib.texcoords[2 * index.texcoord_index + 0],
      // 	1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      _vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
      // _vertex.normal = {// 1.0f,1.0f,1.0f};
      // 				  attrib.normals[3 * index.normal_index
      // + 0], 				  attrib.normals[3 * index.normal_index
      // + 1], 				  attrib.normals[3 * index.normal_index
      // + 2]};

      if (uniqueVertices.count(_vertex) == 0) {
        uniqueVertices[_vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(_vertex);
      }
      // формируем общий буфер вершин
      indices.push_back(uniqueVertices[_vertex]);
    }
  }
}

void Model::LoadModelFromFile(std::string path) {
  // Определить какой тип объекта и произвести загрузку

  // загрузка модели
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        path.c_str())) {
    throw std::runtime_error(warn + err);
  }
  // запоминаем уникальные вершины для отображения
  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex _vertex{};

      _vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                     attrib.vertices[3 * index.vertex_index + 1],
                     attrib.vertices[3 * index.vertex_index + 2]};

      _vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      _vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
      _vertex.normal = {// 1.0f,1.0f,1.0f};
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};

      if (uniqueVertices.count(_vertex) == 0) {
        uniqueVertices[_vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(_vertex);
      }
      // формируем общий буфер вершин
      indices.push_back(uniqueVertices[_vertex]);
    }
  }
}

void Model::createIndexBuffer() {
  VkDeviceSize bufferSize;

  bufferSize = sizeof(indices[0]) * indices.size();

  enma::Buffer buffer;
  vDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &buffer);

  buffer.map();
  memcpy(buffer.mapped, indices.data(), (size_t)bufferSize);

  vDevice->createBuffer(bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ind.buffer);

  vDevice->copyBuffer(buffer.buffer, ind.buffer.buffer, bufferSize);
  buffer.destroy();
}

void Model::createVertexBuffer() {
  auto sz = vertices.size();
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  enma::Buffer buffer;

  vDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &buffer);
  buffer.map();
  memcpy(buffer.mapped, vertices.data(), (size_t)bufferSize);

  vDevice->createBuffer(bufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert.buffer);

  vDevice->copyBuffer(buffer.buffer, vert.buffer.buffer, bufferSize);

  buffer.destroy();
}

void Model::loadTexture(VkImageViewType type) {
  for (auto &tx_path : textures_paths) {
    // check file format on .png or ktx
    auto array = tools::split(tx_path, std::string("."));

    if (array.back() == "ktx") {
      // find array word in name
      std::size_t found = array.at(array.size() - 2).find(std::string("array"));
      if (found != std::string::npos) {
        auto *d_texture = new Texture2DArray();
        d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                vDevice->queue);
        textures.emplace_back(d_texture);
      } else {
        auto *d_texture = new Texture2DKTX();
        d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                vDevice->queue);
        textures.emplace_back(d_texture);
      }
    } else {
      textures.emplace_back(new Texture());
      textures.back()->loadTexture(tx_path, vDevice, vSwapChain,
                                   TextureType::SIMPLE, type);
    }
  }
}

void Model::setDescriptorLayout() {
  // Для каждого элемента сверху что хотим передать в шейдеры задаем свое поле
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalLayoutBinding{};
  normalLayoutBinding.binding = 2;
  normalLayoutBinding.descriptorCount = 1;
  normalLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalLayoutBinding.pImmutableSamplers = nullptr;
  normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding,
      samplerLayoutBinding,
      normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Model::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = reference_color_vector.size();
  subpass.pColorAttachments = reference_color_vector.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Model::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void Model::createFramebuffers(VulkanSwapChain *vkSwapChain) {
  std::vector<VkImageView> view_vector;
  // size_t attach_count = 0;
  // // Считаем общее количество attachments
  // if (attachDescriptors.size() > 0 || attachTransparents.size() > 0)
  // {
  // 	attach_count++;
  // 	createGBufferAttachments();
  // }

  // Необходимо формировать с учетом потребности в отрисовке
  vkSwapChain->swapChainFramebuffers.resize(
      vkSwapChain->swapChainImageViews.size());

  for (size_t i = 0; i < vkSwapChain->swapChainImageViews.size(); i++) {
    // view_vector.clear();
    // view_vector.emplace_back(colorImageView);
    // if (attach_count > 0)
    // {
    // 	view_vector.emplace_back(attachments.position.view);
    // 	view_vector.emplace_back(attachments.normal.view);
    // 	view_vector.emplace_back(attachments.albedo.view);
    // }
    // view_vector.emplace_back(depthImageView);
    // view_vector.emplace_back(swapChainImageViews[i]);
    std::array<VkImageView, 3> attachments = {
        vkSwapChain->colorImageView, vkSwapChain->depthImageView,
        vkSwapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vDevice->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = vkSwapChain->swapChainExtent.width;
    framebufferInfo.height = vkSwapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &vkSwapChain->swapChainFramebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

// матрица поворота в заданном направлении
glm::mat4 Model::getOrintationMat(glm::vec3 vec, glm::vec3 start_orientation) {
  // double angle = std::atan2(vec.y,vec.x);
  // glm::mat4 glmrotXY = glm::rotation(angle,glm::tvec3<double>(0.0,0.0,1.0));
  // auto rot_ax = glm::vec3(0,0,1);
  // if((-currentPos.z+vec.z)<0)
  //    rot_ax = glm::vec3(0,0,-1);
  //
  // glm::vec3 rotor = getRotationVector(vec, currentPos, false, rot_ax);
  if (vec == glm::vec3(0))
    return glm::mat4(1);

  vec = glm::normalize(vec);
  glm::quat ror = glm::rotation(start_orientation, vec);
  glm::mat4 Mrot = glm::mat4_cast(ror); // glm::quat(rotor));
  return Mrot;
}

glm::vec3 Model::getRotationVector(glm::vec3 point_to, glm::vec3 point_from,
                                   bool counterclockwise,
                                   glm::vec3 rotate_axis) {
  glm::vec3 lookVector = point_to;
  glm::vec3 up_vector = rotate_axis;
  glm::vec3 cross;
  if (lookVector == point_from)
    return glm::vec3(0);
  glm::vec3 direction = glm::normalize(lookVector - point_from);
  float dot = glm::dot(direction, up_vector);
  if (fabs(dot - (-1.0f)) < 0.000001f) {
    return {0, 0, 1};
  } else if (fabs(dot - (1.0f)) < 0.000001f) {
    auto quat = glm::quat();
    return glm::axis(quat) * glm::angle(quat);
  }
  float dot_angle = acosf(dot); // glm::degress
  if (counterclockwise)
    cross = glm::normalize(glm::cross(direction, up_vector));
  else
    cross = glm::normalize(glm::cross(up_vector, direction));
  auto rotate = cross * dot_angle;
  return rotate;
}

void Model::draw(VkCommandBuffer model_buffer) {
  VkDeviceSize offsets[1] = {0};

  vkCmdBindPipeline(model_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  vkCmdBindDescriptorSets(model_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);

  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(model_buffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(model_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(model_buffer, static_cast<uint32_t>(getIndices()->size()), 1,
                   0, 0, 0);
}

Model::Model(uint32_t *buffer, uint32_t *_ind, uint32_t shape_num,
             model_type _type)
    : Object() {
  _buffer = buffer;
  _shape_num = shape_num;
  type = _type;
}

void Model::initialization() {
  if (vDevice == nullptr)
    return;
  if (path.empty()) {
    if (_buffer != nullptr) {
      GenerateModelFromBuffer(_buffer, indeces, _shape_num * 3, _shape_num);
      createVertexBuffer();
      createIndexBuffer();
    } else {
      return;
    }
  } else if (!path.empty()) {
    switch (type) {
    case model_type::from_obj_file:
      LoadModelFromFile(path);
      break;
    case model_type::from_ktx_file:
      GenerateModelFromFile(path);
      break;
    default:
      LoadModelFromFile(path);
      break;
    }
    createVertexBuffer();
    createIndexBuffer();
  }
}

Model::Model(std::string _path, model_type _type)
    : path(std::move(_path)), Object() {
  type = _type;
}

void Model::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  ind.buffer.destroy();
  vert.buffer.destroy();
}

void Model::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;
  size_t obj_size = getTexturesSize() + 1;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));

  for (size_t k = 1; k < obj_size; k++)
    descriptorWrites.emplace_back(initializers::createVkWriteDescriptorTexture(
        k, get_descriptor_image(static_cast<size_t>(k - 1)),
        get_descriptor_set()));

  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void Model::createDescriptorPool() {
  size_t draw_size = 0;
  draw_size += getTexturesSize();

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount =
      static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * draw_size);
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount =
      static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * draw_size * 2);
  poolDrawSize = MAX_FRAMES_IN_FLIGHT * draw_size * 2;
}

void Model::set_vertices(Vertex *vert, size_t size) {
  for (size_t i = 0; i < size; i++) {
    vertices.emplace_back(vert[i]);
  }
}

void Model::set_indecies(uint32_t *ind, size_t size) {
  for (size_t i = 0; i < size; i++) {
    indices.emplace_back(ind[i]);
  }
}

void Model::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE,
                                                        VK_COMPARE_OP_LESS);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE,
          0); ///*_COUNTER*/
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  // fixme: changed for viewport
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);
  // fixme: changed for viewports
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

VkDeviceSize Model::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferObject));
}

uint32_t Model::getTexturesSize() {
  return static_cast<uint32_t>(textures.size());
}

viBuffer *Model::getBuffer() {
  //        viBuffer buff{};
  trn_buff.vert = vert.buffer.buffer;
  trn_buff.ind = ind.buffer.buffer;
  return &trn_buff;
}

std::vector<uint32_t> *Model::getIndices() { return &indices; }

void Model::update(float frame_time) {}

void Model::prepare() {}

void Model::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &object_ubo, sizeof(object_ubo));
}

void Model::createAdditinalBuffer() {}

void Model::updateUniformBuffer() {
  ObjCamera *cam = ObjCamera::getApp();
  object_ubo.view = cam->matrices.view;
  object_ubo.proj = cam->matrices.perspective;
  object_ubo.model = glm::mat4(1.0f);
  // Get entity for light position
  object_ubo.lightPositon = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);
  object_ubo.viewPos = cam->viewPos;
  object_ubo.unique_id = glm::vec4(10, 1, 0, 0);
}

Model::~Model() = default;

VkDescriptorImageInfo *
Partical_Model_CPU::get_descriptor_image(size_t tex_idx) {
  return &textures.at(tex_idx)->descriptor;
}

void Partical_Model_CPU::loadTexture(VkImageViewType type) {
  for (auto path : textures_paths) {
    Texture2DKTX *d_texture = new Texture2DKTX();
    d_texture->loadFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                            vDevice->queue);
    /**
     * Change sampler spectial for particles
     */
    VkSamplerCreateInfo samplerCreateInfo = initializers::samplerCreateInfo();
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    // Different adress mode
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
    samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0f;
    // both particle textures have the same number of mip maps
    samplerCreateInfo.maxLod = float(d_texture->mipLevels);

    if (vDevice->features.samplerAnisotropy) {
      samplerCreateInfo.maxAnisotropy = 8.0f;
      samplerCreateInfo.anisotropyEnable = VK_TRUE;
    }
    //
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    VK_CHECK_RESULT(vkCreateSampler(vDevice->logicalDevice, &samplerCreateInfo,
                                    nullptr, &d_texture->descriptor.sampler));
    d_texture->need_local_destroy = true;
    textures.emplace_back(d_texture);
  }
}

void Partical_Model_CPU::setObjectInfo(
    pipeline_parameters *_parameters,
    VkGraphicsPipelineCreateInfo *pipelineInfo) {
  particle.vertexInputBindingDescription = particle.inputBindingDescription(0);

  particle.vertexInputAttributeDescriptions.resize(6);
  particle.vertexInputAttributeDescriptions = {
      particle.inputAttributeDescription(0, 0, ParticleComponent::Position),
      particle.inputAttributeDescription(0, 1, ParticleComponent::Color),
      particle.inputAttributeDescription(0, 2, ParticleComponent::Alpha),
      particle.inputAttributeDescription(0, 3, ParticleComponent::Size),
      particle.inputAttributeDescription(0, 4, ParticleComponent::Rotation),
      particle.inputAttributeDescription(0, 5, ParticleComponent::Type),
  };

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(particle.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &particle.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      particle.vertexInputAttributeDescriptions.data();

  // Don t' write to depth buffer
  _parameters->depthStencil->depthWriteEnable = VK_FALSE;

  // Premulitplied alpha
  _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  _parameters->colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  _parameters->colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  //
  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

void Partical_Model_CPU::setDescriptorLayout() {
  // Для каждого элемента сверху что хотим передать в шейдеры задаем свое поле
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalLayoutBinding{};
  normalLayoutBinding.binding = 2;
  normalLayoutBinding.descriptorCount = 1;
  normalLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalLayoutBinding.pImmutableSamplers = nullptr;
  normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding,
      samplerLayoutBinding,
      normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Partical_Model_CPU::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = reference_color_vector.size();
  subpass.pColorAttachments = reference_color_vector.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Partical_Model_CPU::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void Partical_Model_CPU::updateUniformBuffer() {}

void Partical_Model_CPU::createFramebuffers(VulkanSwapChain *vkSwapChain) {
  std::vector<VkImageView> view_vector;
  // size_t attach_count = 0;
  // // Считаем общее количество attachments
  // if (attachDescriptors.size() > 0 || attachTransparents.size() > 0)
  // {
  // 	attach_count++;
  // 	createGBufferAttachments();
  // }

  // Необходимо формировать с учетом потребности в отрисовке
  vkSwapChain->swapChainFramebuffers.resize(
      vkSwapChain->swapChainImageViews.size());

  for (size_t i = 0; i < vkSwapChain->swapChainImageViews.size(); i++) {
    // view_vector.clear();
    // view_vector.emplace_back(colorImageView);
    // if (attach_count > 0)
    // {
    // 	view_vector.emplace_back(attachments.position.view);
    // 	view_vector.emplace_back(attachments.normal.view);
    // 	view_vector.emplace_back(attachments.albedo.view);
    // }
    // view_vector.emplace_back(depthImageView);
    // view_vector.emplace_back(swapChainImageViews[i]);
    std::array<VkImageView, 3> attachments = {
        vkSwapChain->colorImageView, vkSwapChain->depthImageView,
        vkSwapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vDevice->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = vkSwapChain->swapChainExtent.width;
    framebufferInfo.height = vkSwapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &vkSwapChain->swapChainFramebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Partical_Model_CPU::draw(VkCommandBuffer _buffer) {
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);

  VkDeviceSize offsets[1] = {0};
  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);

  // vkCmdBindIndexBuffer(_buffer, getBuffer().ind, 0, VK_INDEX_TYPE_UINT32);
  // vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices().size()), 1, 0,
  // 0, 0);

  vkCmdDraw(_buffer, particle_count, 1, 0, 0);
}

void Partical_Model_CPU::set_count(unsigned int _count) {
  particle_count = _count;
}

void Partical_Model_CPU::set_radius(float _radius) { radius = _radius; }

void Partical_Model_CPU::set_position(glm::vec3 position) {
  emitterPos = position;
}

void Partical_Model_CPU::set_direction(glm::vec3 direction) {
  // err guard
  if (direction == glm::vec3(0, 0, 0))
    return;

  direction = glm::normalize(direction);
  if (lastDirection == direction) // if nothing changed
    return;

  minVel = minVel_base * direction;
  maxVel = maxVel_base * direction;
  lastDirection = direction;
}

void Partical_Model_CPU::set_alpha(float value) { alpha = value; }

Partical_Model_CPU::Partical_Model_CPU() : Object() {}

void Partical_Model_CPU::initialization() {
  rndEngine.seed((unsigned)time(nullptr));
}

void Partical_Model_CPU::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  vkUnmapMemory(vDevice->logicalDevice, particles.memory);
  vkDestroyBuffer(vDevice->logicalDevice, particles.buffer, nullptr);
  vkFreeMemory(vDevice->logicalDevice, particles.memory, nullptr);
}

void Partical_Model_CPU::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;
  size_t obj_size = getTexturesSize() + 1;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));

  for (size_t k = 1; k < obj_size; k++)
    descriptorWrites.emplace_back(initializers::createVkWriteDescriptorTexture(
        k, get_descriptor_image(static_cast<size_t>(k - 1)),
        get_descriptor_set()));

  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void Partical_Model_CPU::createDescriptorPool() {
  size_t draw_size = 0;
  draw_size += getTexturesSize();

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount =
      static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * draw_size);
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount =
      static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * draw_size * 2);
  poolDrawSize += MAX_FRAMES_IN_FLIGHT * draw_size;
}

VkDeviceSize Partical_Model_CPU::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferParticle));
}

uint32_t Partical_Model_CPU::getTexturesSize() {
  return static_cast<uint32_t>(textures.size());
}

viBuffer *Partical_Model_CPU::getBuffer() {
  //        viBuffer buff{};
  trn_buff.vert = particles.buffer;
  trn_buff.ind = VkBuffer();
  return &trn_buff;
}

std::vector<uint32_t> *Partical_Model_CPU::getIndices() {
  return new std::vector<uint32_t>();
}

void Partical_Model_CPU::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE,
          0); ///*_COUNTER*/
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  // fixme: changed for viewport
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples,
                                                       0); //

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void Partical_Model_CPU::update(float frame_time) {
  updateParticles(frame_time);
}

void Partical_Model_CPU::prepare() { prepareParticles(); }

float Partical_Model_CPU::rnd(float range) {
  std::normal_distribution<float> rndDist(range / 1.5, range / 8);
  return rndDist(rndEngine);
}

void Partical_Model_CPU::prepareParticles() {
  particleBuffer.resize(particle_count);
  for (auto &part_buf : particleBuffer) {
    initParticle(&part_buf, emitterPos);
    part_buf.alpha =
        1.0f - (glm::length(part_buf.pos - glm::vec4(emitterPos, 0)) /
                (radius * 2.0f));
  }

  particles.size = particleBuffer.size() * sizeof(Particle);

  if (vDevice->createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &particles.buffer, particles.size,
                            &particles.memory,
                            particleBuffer.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  // Map the memory and store the pointer for reuse
  if (vkMapMemory(vDevice->logicalDevice, particles.memory, 0, particles.size,
                  0, &particles.mappedMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to map memory!");
  }
}

void Partical_Model_CPU::updateParticles(float frameTimer) {
  float particleTimer = 1;
  for (auto &part_buf : particleBuffer) {
    switch (part_buf.type) {
    case PARTICLE_TYPE_FLAME:
      part_buf.pos -= part_buf.vel * particleTimer * 0.15f;
      part_buf.alpha += particleTimer * 0.05f;
      part_buf.size -= particleTimer * 0.05f;
      break;
    case PARTICLE_TYPE_SMOKE:
      part_buf.pos -= part_buf.vel * particleTimer * 0.1f;
      part_buf.alpha += particleTimer * 0.025f;
      part_buf.size += particleTimer * 0.0125f;
      part_buf.color -= particleTimer * 0.05f;
      break;
    }
    part_buf.rotation += particleTimer * part_buf.rotationSpeed;
    // Transition part_buf state
    if (part_buf.alpha > 2.0f) {
      transitionParticle(&part_buf);
    }
  }
  size_t size = particleBuffer.size() * sizeof(Particle);
  memcpy(particles.mappedMemory, particleBuffer.data(), size);
}

void Partical_Model_CPU::transitionParticle(Particle *part) {
  switch (part->type) {
  case PARTICLE_TYPE_FLAME:
    // Flame particles have a chance of turning into smoke
    if (rnd(1.0f) < 0.35f) {
      part->alpha = alpha;
      part->color = glm::vec4(0.25f + rnd(0.25f));
      //
      //                    part->pos.x += 0.5f;
      //                    part->pos.z += 0.5f;
      //                    part->pos.y += 0.5f;
      part->vel = glm::vec4((minVel.x * 2) + rnd(maxVel.x - minVel.x),
                            (minVel.y * 2) + rnd(maxVel.y - minVel.y),
                            (minVel.z * 2) + rnd(maxVel.z - minVel.z), 0.0f);
      part->size = 0.5f + rnd(0.5f);
      part->rotationSpeed = rnd(1.0f) - rnd(1.0f);
      part->type = PARTICLE_TYPE_SMOKE;
      // part->pos += glm::vec4(emitterPos, 0.f);
    } else {
      initParticle(part, emitterPos);
    }
    break;
  case PARTICLE_TYPE_SMOKE:
    // Respawn at end of life
    initParticle(part, emitterPos);
    break;
  }
}

void Partical_Model_CPU::initParticle(Particle *part, glm::vec3 emit_pos) {
  part->vel = glm::vec4(minVel.x + rnd(maxVel.x - minVel.x),
                        minVel.y + rnd(maxVel.y - minVel.y),
                        minVel.z + rnd(maxVel.z - minVel.z),
                        0.0f); /// направление движения частиц
  part->alpha = alpha + rnd(0.5f);
  part->size = 1.0f + rnd(1.f);
  part->color = glm::vec4(1.0f);
  part->type = PARTICLE_TYPE_FLAME;
  part->rotation = rnd(2.0f * float(glm::pi<float>()));
  part->rotationSpeed = rnd(2.0f) - rnd(2.0f);

  // Get random sphere point
  float theta = rnd(2.0f * float(glm::pi<float>()));
  float phi = rnd(float(glm::pi<float>())) - float(glm::pi<float>()) / 2.0f;
  float r = rnd(radius);

  part->pos.x = r * cos(theta) * cos(phi);
  part->pos.y = r * sin(phi);
  part->pos.z = r * sin(theta) * cos(phi);

  part->pos += glm::vec4(emit_pos, 0.0f);
}

void Partical_Model_CPU::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &particl_ubo, sizeof(particl_ubo));
}

void Partical_Model_CPU::createAdditinalBuffer() {}

void Partical_Model_GPU::loadTexture(VkImageViewType type) {
  for (auto path : textures_paths) {
    Texture2DKTX *d_texture = new Texture2DKTX();
    d_texture->loadFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                            vDevice->queue);
    /**
     * Change sampler spectial for particles
     */
    VkSamplerCreateInfo samplerCreateInfo = initializers::samplerCreateInfo();
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    // Different adress mode
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
    samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0f;
    // both particle textures have the same number of mip maps
    samplerCreateInfo.maxLod = float(d_texture->mipLevels);

    if (vDevice->features.samplerAnisotropy) {
      samplerCreateInfo.maxAnisotropy = 8.0f;
      samplerCreateInfo.anisotropyEnable = VK_TRUE;
    }
    //
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    VK_CHECK_RESULT(vkCreateSampler(vDevice->logicalDevice, &samplerCreateInfo,
                                    nullptr, &d_texture->descriptor.sampler));
    d_texture->need_local_destroy = true;
    textures.emplace_back(d_texture);
  }
}

void Partical_Model_GPU::createDescriptorSets() {
  VkDescriptorSetAllocateInfo allocInfo =
      initializers::descriptorSetAllocateInfo(descriptorPool,
                                              &descriptorSetLayout, 1);

  VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                                           &descriptor));

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;
  // Binding 0 : Particle color map
  writeDescriptorSets.push_back(initializers::writeDescriptorSet(
      descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
      &textures.at(0)->descriptor));
  // Binding 1 : Particle gradient ramp
  writeDescriptorSets.push_back(initializers::writeDescriptorSet(
      descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
      &textures.at(1)->descriptor));

  writeDescriptorSets.push_back(initializers::writeDescriptorSet(
      descriptor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2,
      &uniformObjectBuffer.descriptor));

  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(writeDescriptorSets.size()),
                         writeDescriptorSets.data(), 0, NULL);
  /**
   *  New BLOCK OFF SEMAPHORE
   */
  createSemaphore();
  /**
   *  ADD Aditinal compute command buffer build
   */
}

void Partical_Model_GPU::prepareParticles() {
  std::default_random_engine _rndEngine((unsigned)time(nullptr));
  std::normal_distribution<float> rndDist(0.0f, 0.2f);

  // Initial particle positions
  std::vector<ParticleGPU> particleBuffer(PARTICLE_COUNT);
  for (auto &particle : particleBuffer) {
    particle.pos = glm::vec3(rndDist(_rndEngine), rndDist(_rndEngine),
                             rndDist(_rndEngine));
    particle.vel = particle.pos * 0.1f; // glm::vec3(rndDist(_rndEngine));
    particle.gradientPos.x = 0;
    particle.gradientPos.y = 1;
    particle.gradientPos.z = 2;
    particle.lifeTime = 0.f;
    particle.estLifetime = 3.0f;
    particle.randomPos =
        particle.pos; // glm::vec3(rndDist(_rndEngine), rndDist(_rndEngine),
                      // rndDist(_rndEngine));
  }

  VkDeviceSize storageBufferSize = particleBuffer.size() * sizeof(ParticleGPU);

  // Staging
  enma::Buffer stagingBuffer;
  vDevice->createBuffer(
      // The SSBO will be used as a storage buffer for the compute pipeline and
      // as a vertex buffer in the graphics pipeline
      storageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &stagingBuffer, particleBuffer.data());
  vDevice->createBuffer(
      storageBufferSize,
      // The SSBO will be used as a storage buffer for the compute pipeline and
      // as a vertex buffer in the graphics pipeline
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &u_ptr_compute->storageBuffer);

  // Copy from staging buffer to storage buffer
  VkCommandBuffer copyCmd =
      vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
  VkBufferCopy copyRegion = {};
  copyRegion.size = storageBufferSize;
  vkCmdCopyBuffer(copyCmd, stagingBuffer.buffer,
                  u_ptr_compute->storageBuffer.buffer, 1, &copyRegion);
  // Execute a transfer barrier to the compute queue, if necessary
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        0,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->queueFamilyIndex,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }
  vDevice->endSingleTimeCommands(copyCmd, vDevice->queue, true);

  stagingBuffer.destroy();

  // Binding description
  vertices.bindingDescriptions.resize(1);
  vertices.bindingDescriptions[0] = initializers::vertexInputBindingDescription(
      0, sizeof(ParticleGPU), VK_VERTEX_INPUT_RATE_VERTEX);

  // Attribute descriptions
  // Describes memory layout and shader positions
  vertices.attributeDescriptions.resize(2);
  // Location 0 : Position
  vertices.attributeDescriptions[0] =
      initializers::vertexInputAttributeDescription(
          0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ParticleGPU, pos));
  // Location 1 : Gradient position
  vertices.attributeDescriptions[1] =
      initializers::vertexInputAttributeDescription(
          0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ParticleGPU, gradientPos));

  // Assign to vertex buffer
  vertices.inputState = initializers::pipelineVertexInputStateCreateInfo();
  vertices.inputState.vertexBindingDescriptionCount =
      static_cast<uint32_t>(vertices.bindingDescriptions.size());
  vertices.inputState.pVertexBindingDescriptions =
      vertices.bindingDescriptions.data();
  vertices.inputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertices.attributeDescriptions.size());
  vertices.inputState.pVertexAttributeDescriptions =
      vertices.attributeDescriptions.data();
}

void Partical_Model_GPU::prepareUniformBuffers() {
  // Compute shader uniform buffer block
  vDevice->createBuffer(sizeof(ubo_gpu_particl),
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &u_ptr_compute->uniformBuffer);

  // Map for host access
  VK_CHECK_RESULT(u_ptr_compute->uniformBuffer.map());
}

void Partical_Model_GPU::setDescriptorLayout() {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
  // Binding 0 : Particle color map
  setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
      0));
  // Binding 1 : Particle gradient ramp
  setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
      1));
  // Binding 2 : Uniform buffer
  setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 2));

  VkDescriptorSetLayoutCreateInfo descriptorLayout =
      initializers::descriptorSetLayoutCreateInfo(
          setLayoutBindings.data(),
          static_cast<uint32_t>(setLayoutBindings.size()));

  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(vDevice->logicalDevice,
                                              &descriptorLayout, nullptr,
                                              &descriptorSetLayout));

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
      initializers::pipelineLayoutCreateInfo(vkDescriptorLayouts.data(),
                                             vkDescriptorLayouts.size());

  VK_CHECK_RESULT(vkCreatePipelineLayout(vDevice->logicalDevice,
                                         &pipelineLayoutCreateInfo, nullptr,
                                         &pipelineLayout));
}

void Partical_Model_GPU::createDescriptorPool() {
  size_t draw_size = 0;
  draw_size += getTexturesSize();

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount = static_cast<uint32_t>(2);
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  vkPoolSizes.back().descriptorCount = static_cast<uint32_t>(1);
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount = static_cast<uint32_t>(2);

  poolDrawSize += 2;
}

void Partical_Model_GPU::updateMapped() {
  memcpy(u_ptr_compute->uniformBuffer.mapped, &ubo_gpu_particl,
         sizeof(ubo_gpu_particl));
  memcpy(uniformObjectBuffer.mapped, &ubo_pos_particle,
         sizeof(ubo_pos_particle));
}

void Partical_Model_GPU::preparePipeline() {
  // maybe make unique
  prepareParticles();
  prepareUniformBuffers();
  ///

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 0, VK_FALSE);

  VkPipelineRasterizationStateCreateInfo rasterizationState =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
  rasterizationState.depthBiasEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState blendAttachmentState =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

  VkPipelineColorBlendStateCreateInfo colorBlendState =
      initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

  VkPipelineDepthStencilStateCreateInfo depthStencilState =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  // depthStencilState.back.compareOp = VK_COMPARE_OP_LESS;
  // depthStencilState.stencilTestEnable = VK_TRUE;

  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);

  VkPipelineMultisampleStateCreateInfo multisampleState =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(
          dynamicStateEnables.data(),
          static_cast<uint32_t>(dynamicStateEnables.size()), 0);

  // Rendering pipeline
  // Load shaders
  std::array<VkPipelineShaderStageCreateInfo, 2> _shaderStages;

  _shaderStages[0] = shadersStages[1];
  // loadShader(getShadersPath() + "computeparticles/particle.vert.spv",
  // VK_SHADER_STAGE_VERTEX_BIT);
  _shaderStages[1] = shadersStages[2];
  // loadShader(getShadersPath() + "computeparticles/particle.frag.spv",
  // VK_SHADER_STAGE_FRAGMENT_BIT);

  VkGraphicsPipelineCreateInfo pipelineCreateInfo =
      initializers::pipelineCreateInfo(pipelineLayout, vDevice->renderPass, 0);

  pipelineCreateInfo.pVertexInputState = &vertices.inputState;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
  pipelineCreateInfo.pRasterizationState = &rasterizationState;
  pipelineCreateInfo.pColorBlendState = &colorBlendState;
  pipelineCreateInfo.pMultisampleState = &multisampleState;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pDepthStencilState = &depthStencilState;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.stageCount = static_cast<uint32_t>(_shaderStages.size());
  pipelineCreateInfo.pStages = _shaderStages.data();
  pipelineCreateInfo.renderPass = vDevice->renderPass;

  // Additive blending
  blendAttachmentState.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  blendAttachmentState.blendEnable = VK_TRUE;
  blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
  blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  blendAttachmentState.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

  VK_CHECK_RESULT(
      vkCreateGraphicsPipelines(vDevice->logicalDevice, pipelineCache, 1,
                                &pipelineCreateInfo, nullptr, &pipeline));
}

void Partical_Model_GPU::prepareCompute() {
  // Create a compute capable vDevice->logicalDevice queue
  // The VulkanDevice::createLogicalDevice functions finds a compute capable
  // queue and prefers queue families that only support compute Depending on the
  // implementation this may result in different queue family indices for
  // graphics and computes, requiring proper synchronization (see the memory and
  // pipeline barriers)
  vkGetDeviceQueue(vDevice->logicalDevice, u_ptr_compute->queueFamilyIndex, 0,
                   &u_ptr_compute->queue);

  // Create compute pipeline
  // Compute pipelines are created separate from graphics pipelines even if they
  // use the same queue (family index)

  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
      // Binding 0 : Particle position storage buffer
      initializers::descriptorSetLayoutBinding(
          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0),
      // Binding 1 : Uniform buffer
      initializers::descriptorSetLayoutBinding(
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1)};

  VkDescriptorSetLayoutCreateInfo descriptorLayout =
      initializers::descriptorSetLayoutCreateInfo(
          setLayoutBindings.data(),
          static_cast<uint32_t>(setLayoutBindings.size()));

  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(
      vDevice->logicalDevice, &descriptorLayout, nullptr,
      &u_ptr_compute->descriptorSetLayout));

  // vkDescriptorLayouts.emplace_back(u_ptr_compute->descriptorSetLayout);

  VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
      initializers::pipelineLayoutCreateInfo(
          &u_ptr_compute->descriptorSetLayout, 1);

  VK_CHECK_RESULT(vkCreatePipelineLayout(vDevice->logicalDevice,
                                         &pPipelineLayoutCreateInfo, nullptr,
                                         &u_ptr_compute->pipelineLayout));

  VkDescriptorSetAllocateInfo allocInfo =
      initializers::descriptorSetAllocateInfo(
          descriptorPool, &u_ptr_compute->descriptorSetLayout, 1);

  VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                                           &u_ptr_compute->descriptorSet));

  std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets = {
      // Binding 0 : Particle position storage buffer
      initializers::writeDescriptorSet(
          u_ptr_compute->descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
          &u_ptr_compute->storageBuffer.descriptor),
      // Binding 1 : Uniform buffer
      initializers::writeDescriptorSet(
          u_ptr_compute->descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          &u_ptr_compute->uniformBuffer.descriptor),
  };

  vkUpdateDescriptorSets(
      vDevice->logicalDevice,
      static_cast<uint32_t>(computeWriteDescriptorSets.size()),
      computeWriteDescriptorSets.data(), 0, NULL);

  // Create pipeline
  VkComputePipelineCreateInfo computePipelineCreateInfo =
      initializers::computePipelineCreateInfo(u_ptr_compute->pipelineLayout, 0);
  computePipelineCreateInfo.stage = shadersStages[0];
  // loadShader(getShadersPath() + "computeparticles/particle.comp.spv",
  //  VK_SHADER_STAGE_COMPUTE_BIT);
  VK_CHECK_RESULT(vkCreateComputePipelines(
      vDevice->logicalDevice, pipelineCache, 1, &computePipelineCreateInfo,
      nullptr, &u_ptr_compute->pipeline));

  // Separate command pool as queue family for compute may be different than
  // graphics
  VkCommandPoolCreateInfo cmdPoolInfo = {};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.queueFamilyIndex = u_ptr_compute->queueFamilyIndex;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  VK_CHECK_RESULT(vkCreateCommandPool(vDevice->logicalDevice, &cmdPoolInfo,
                                      nullptr, &u_ptr_compute->commandPool));

  // Create a command buffer for compute operations
  u_ptr_compute->commandBuffer = vDevice->beginSingleTimeCommands(
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, u_ptr_compute->commandPool, false);

  // Semaphore for compute & graphics sync
  VkSemaphoreCreateInfo semaphoreCreateInfo =
      initializers::semaphoreCreateInfo();

  VK_CHECK_RESULT(vkCreateSemaphore(vDevice->logicalDevice,
                                    &semaphoreCreateInfo, nullptr,
                                    &u_ptr_compute->compute));

  // Build a single command buffer containing the compute dispatch commands
  buildComputeCommandBuffer();
}

void Partical_Model_GPU::draw(VkCommandBuffer _buffer) {
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, NULL);

  VkDeviceSize offsets[1] = {0};
  vkCmdBindVertexBuffers(_buffer, 0, 1, &u_ptr_compute->storageBuffer.buffer,
                         offsets);
  vkCmdDraw(_buffer, PARTICLE_COUNT, 1, 0, 0);
}

void Partical_Model_GPU::buildComputeCommandBuffer() {
  VkCommandBufferBeginInfo cmdBufInfo = initializers::commandBufferBeginInfo();

  VK_CHECK_RESULT(
      vkBeginCommandBuffer(u_ptr_compute->commandBuffer, &cmdBufInfo));

  // Compute particle movement

  // Add memory barrier to ensure that the (graphics) vertex shader has fetched
  // attributes before compute starts to write to the buffer
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        0,
        VK_ACCESS_SHADER_WRITE_BIT,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->queueFamilyIndex,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(u_ptr_compute->commandBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }

  // Dispatch the compute job
  vkCmdBindPipeline(u_ptr_compute->commandBuffer,
                    VK_PIPELINE_BIND_POINT_COMPUTE, u_ptr_compute->pipeline);
  vkCmdBindDescriptorSets(
      u_ptr_compute->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
      u_ptr_compute->pipelineLayout, 0, 1, &u_ptr_compute->descriptorSet, 0, 0);
  vkCmdDispatch(u_ptr_compute->commandBuffer, PARTICLE_COUNT / 256, 1, 1);

  // Add barrier to ensure that compute shader has finished writing to the
  // buffer Without this the (rendering) vertex shader may display incomplete
  // results (partial data from last frame)
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_SHADER_WRITE_BIT,
        0,
        u_ptr_compute->queueFamilyIndex,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(u_ptr_compute->commandBuffer,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }

  vkEndCommandBuffer(u_ptr_compute->commandBuffer);
}

void Partical_Model_GPU::createAdditinalBuffer() { prepareCompute(); }

void Partical_Model_GPU::initialization() {
  u_ptr_compute->queueFamilyIndex = vDevice->queueFamilyIndices.compute;
}

void Partical_Model_GPU::acquireBarrier(VkCommandBuffer buffer) {
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        0,
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        u_ptr_compute->queueFamilyIndex,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }
}

void Partical_Model_GPU::releaseBarrier(VkCommandBuffer buffer) {
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        0,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->queueFamilyIndex,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }
}

void Partical_Model_GPU::createSemaphore() {
  // Semaphore for compute & graphics sync
  VkSemaphoreCreateInfo semaphoreCreateInfo =
      initializers::semaphoreCreateInfo();
  VK_CHECK_RESULT(vkCreateSemaphore(vDevice->logicalDevice,
                                    &semaphoreCreateInfo, nullptr,
                                    &u_ptr_compute->graphic));

  // Signal the semaphore
  VkSubmitInfo submitInfo = initializers::submitInfo();
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &u_ptr_compute->graphic;
  VK_CHECK_RESULT(
      vkQueueSubmit(vDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));
  VK_CHECK_RESULT(vkQueueWaitIdle(vDevice->queue));
}

VkDeviceSize Partical_Model_GPU::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferParticleGPUPosition));
}

uint32_t Partical_Model_GPU::getTexturesSize() { return textures.size(); }

viBuffer *Partical_Model_GPU::getBuffer() { return nullptr; }

std::vector<uint32_t> *Partical_Model_GPU::getIndices() { return nullptr; }

void Partical_Model_GPU::setObjectInfo(
    pipeline_parameters *_parameters,
    VkGraphicsPipelineCreateInfo *pipelineInfo) {}

void Partical_Model_GPU::update(float frame_time) {}

void Partical_Model_GPU::prepare() {}

void Partical_Model_GPU::createFramebuffers(VulkanSwapChain *vkSwapChain) {}

void Partical_Model_GPU::createRenderPass(VkFormat format) {}

void Partical_Model_GPU::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

VkDescriptorImageInfo *
Partical_Model_GPU::get_descriptor_image(size_t tex_idx) {
  return nullptr;
}

void Partical_Model_GPU::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  additionalDestroy();
}

void Partical_Model_GPU::additionalDestroy() { u_ptr_compute->destroy(); }

void Partical_Model_GPU::clearComputeBlock() {
  vkDestroySemaphore(vDevice->logicalDevice, u_ptr_compute->compute, nullptr);
  vkDestroySemaphore(vDevice->logicalDevice, u_ptr_compute->graphic, nullptr);
}

void Partical_Model_GPU::updateUniformBuffer() {}

Partical_Model_GPU::Partical_Model_GPU() : Object() {
  render_flags = ObjectRenderFlags::CMPTOBJECT;
}

void Partical_Model_GPU::set_count(unsigned int _count) {
  particle_count = _count;
}

void Partical_Model_GPU::set_radius(float _radius) { radius = _radius; }

void Partical_Model_GPU::set_position(glm::vec3 position) {
  this->ubo_pos_particle.emiter_position = position;
}

void Partical_Model_GPU::set_direction(glm::vec3 direction) {
  // err guard
  // direction *= 10;
  this->ubo_gpu_particl.destX = direction.x;
  this->ubo_gpu_particl.destY = direction.y;
  this->ubo_gpu_particl.destZ = direction.z;
}

void Partical_Model_GPU::set_alpha(float value) { alpha = value; };

void Terrain_Model::loadTexture(VkImageViewType type) {
  for (auto path : textures_paths) {
    auto array = tools::split(path, std::string("."));

    if (array.back() == "ktx") {
      // find array word in name
      std::size_t found = array.at(array.size() - 2).find(std::string("array"));
      if (found != std::string::npos) {
        Texture2DArray *d_texture = new Texture2DArray();
        d_texture->loadFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                vDevice->queue);
        textures.emplace_back(d_texture);

        vkDestroySampler(vDevice->logicalDevice,
                         textures.back()->textureSampler, nullptr);
        VkSamplerCreateInfo samplerInfo = initializers::samplerCreateInfo();
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = samplerInfo.addressModeU;
        samplerInfo.addressModeW = samplerInfo.addressModeU;
        samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(textures.back()->mipLevels);
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        if (vDevice->supportedFeatures.samplerAnisotropy) {
          samplerInfo.maxAnisotropy = 4.0f;
          samplerInfo.anisotropyEnable = VK_TRUE;
        }

        VK_CHECK_RESULT(vkCreateSampler(vDevice->logicalDevice, &samplerInfo,
                                        nullptr,
                                        &textures.back()->textureSampler));
        textures.back()->descriptor.sampler = textures.back()->textureSampler;
      } else {
        Texture2DKTX *d_texture = new Texture2DKTX();
        d_texture->loadFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                vDevice->queue);
        textures.emplace_back(d_texture);
      }
    } else {
      textures.emplace_back(new Texture());
      textures.back()->loadTexture(path, vDevice, vSwapChain,
                                   TextureType::SIMPLE, type);
      // For Height map
      VkSamplerCreateInfo samplerInfo = initializers::samplerCreateInfo();
      vkDestroySampler(vDevice->logicalDevice, textures.back()->textureSampler,
                       nullptr);

      samplerInfo.magFilter = VK_FILTER_LINEAR;
      samplerInfo.minFilter = VK_FILTER_LINEAR;
      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
      samplerInfo.addressModeV = samplerInfo.addressModeU;
      samplerInfo.addressModeW = samplerInfo.addressModeU;
      samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
      samplerInfo.minLod = 0.0f;
      samplerInfo.maxLod = static_cast<float>(textures.back()->mipLevels);
      samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
      VK_CHECK_RESULT(vkCreateSampler(vDevice->logicalDevice, &samplerInfo,
                                      nullptr,
                                      &textures.back()->textureSampler));
      textures.back()->descriptor.sampler = textures.back()->textureSampler;
    }
  }
}

Terrain_Model::Terrain_Model(std::string _path, model_type _type)
    : Model(std::move(_path), _type) {}

void Terrain_Model::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void Terrain_Model::setObjectInfo(pipeline_parameters *_parameters,
                                  VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(3);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      // vertex.inputAttributeDescription(0, 3, VertexComponent::Color),
      // vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent),
  };

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  _parameters->dynamicState->pDynamicStates = dynamicStateEnables.data();
  _parameters->dynamicState->dynamicStateCount =
      static_cast<uint32_t>(dynamicStateEnables.size());
  tesselationState = initializers::pipelineTessellationStateCreateInfo(4);
  _parameters->tesselationState = &tesselationState;
  pipelineInfo->pTessellationState = _parameters->tesselationState;
  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
  _parameters->depthStencil->depthTestEnable = VK_TRUE;
  _parameters->depthStencil->depthWriteEnable = VK_TRUE;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_BACK_BIT;
  _parameters->rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
}

VkDeviceSize Terrain_Model::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferTessellation));
}

void Terrain_Model::setDescriptorLayout() {
  // For tesselation
  VkDescriptorSetLayoutBinding tesselationLayoutBinding{};
  tesselationLayoutBinding.binding = 0;
  tesselationLayoutBinding.descriptorCount = 1;
  tesselationLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  tesselationLayoutBinding.stageFlags =
      VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
      VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

  VkDescriptorSetLayoutBinding heightLayoutBinding{};
  heightLayoutBinding.binding = 1;
  heightLayoutBinding.descriptorCount = 1;
  heightLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  heightLayoutBinding.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
                                   VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT |
                                   VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding terrianLayoutBinding{};
  terrianLayoutBinding.binding = 2;
  terrianLayoutBinding.descriptorCount = 1;
  terrianLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  terrianLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      tesselationLayoutBinding, heightLayoutBinding, terrianLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Terrain_Model::draw(VkCommandBuffer _buffer) {
  if (!is_object_visible)
    return;

  VkDeviceSize offsets[1] = {0};

  vkCmdSetLineWidth(_buffer, 1.0f);

  if (vDevice->supportedFeatures.pipelineStatisticsQuery) {
    vkCmdBeginQuery(_buffer, vDevice->queryPool, 0, 0);
  }

  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);

  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices()->size()), 1, 0,
                   0, 0);

  if (vDevice->supportedFeatures.pipelineStatisticsQuery &&
      vDevice->queryPool != VK_NULL_HANDLE) {
    vkCmdEndQuery(_buffer, vDevice->queryPool, 0);
  }
}

void Terrain_Model::updateMapped() {

  vks::Frustum frustum;
  frustum.update(tesselation_ubo.projection * tesselation_ubo.modelview);

  memcpy(tesselation_ubo.frustumPlanes.data(), frustum.planes.data(),
         sizeof(glm::vec4) * 6);

  float savedFactor = tesselation_ubo.tessellationFactor;
  if (!tesselation)
    tesselation_ubo.tessellationFactor = 0.0f;

  memcpy(uniformObjectBuffer.mapped, &tesselation_ubo, sizeof(tesselation_ubo));

  if (!tesselation)
    tesselation_ubo.tessellationFactor = savedFactor;
}

void Terrain_Model::createAdditinalBuffer() { Model::createAdditinalBuffer(); }

void Terrain_Model::updateUniformBuffer() {}

void GLTF_SkyBox::loadTexture(VkImageViewType type) {
  for (const auto &path : textures_paths) {
    auto *d_texture = new TextureCubeMap();
    d_texture->loadFromFile(path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                            vDevice->queue);
    textures.emplace_back(d_texture);
  }
}

GLTF_SkyBox::GLTF_SkyBox(std::string object_path)
    : GLTF_Model(std::move(object_path)) {}

void GLTF_SkyBox::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  u_ptr_model->destroy();
}

void GLTF_SkyBox::setDescriptorLayout() {
  // vkglTF::descriptorSetLayoutImage;
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // VkDescriptorSetLayoutBinding normalLayoutBinding{};
  // normalLayoutBinding.binding = 2;
  // normalLayoutBinding.descriptorCount = 1;
  // normalLayoutBinding.descriptorType =
  // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  // normalLayoutBinding.pImmutableSamplers = nullptr;
  // normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding, samplerLayoutBinding,
      // normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  std::vector<VkPipelineLayoutCreateInfo> pipelineLayoutInfo{};
  pipelineLayoutInfo.resize(1);
  pipelineLayoutInfo.at(0).sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.at(0).setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.at(0).pSetLayouts =
      vkDescriptorLayouts.data(); //&descriptorSetLayout;

  // pipelineLayoutInfo.at(1).sType =
  // VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  // pipelineLayoutInfo.at(1).setLayoutCount = 1;
  // pipelineLayoutInfo.at(1).pSetLayouts = &vkglTF::descriptorSetLayoutImage;

  if (vkCreatePipelineLayout(vDevice->logicalDevice, pipelineLayoutInfo.data(),
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void GLTF_SkyBox::setObjectInfo(pipeline_parameters *_parameters,
                                VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(1);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      // vertex.inputAttributeDescription(0, 1, VertexComponent::Color),
      // vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      // vertex.inputAttributeDescription(0, 3, VertexComponent::Normal),
      // vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent)
  };

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  // _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  // _parameters->colorBlendAttachment->srcColorBlendFactor =
  // VK_BLEND_FACTOR_SRC_ALPHA;
  // _parameters->colorBlendAttachment->dstColorBlendFactor =
  // VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  // _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  // _parameters->colorBlendAttachment->srcAlphaBlendFactor =
  // VK_BLEND_FACTOR_ONE; _parameters->colorBlendAttachment->dstAlphaBlendFactor
  // = VK_BLEND_FACTOR_ZERO; _parameters->colorBlendAttachment->alphaBlendOp =
  // VK_BLEND_OP_ADD; _parameters->colorBlendAttachment->colorWriteMask =
  // VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
  // VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->depthStencil->depthWriteEnable = VK_FALSE;
  _parameters->depthStencil->depthTestEnable = VK_TRUE;
  _parameters->depthStencil->depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_FRONT_BIT;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  _parameters->rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
}

VkDeviceSize GLTF_SkyBox::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferSkyBox));
}

void GLTF_SkyBox::draw(VkCommandBuffer _buffer) {
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);

  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  u_ptr_model->draw(_buffer);
}

void GLTF_SkyBox::createFramebuffers(VulkanSwapChain *vkSwapChain) {
  frameBuffer.resize(vkSwapChain->swapChainImageViews.size());
  std::vector<VkImageView> view_vector;
  swapChainExtent = &vkSwapChain->swapChainExtent;
  colorImage = vkSwapChain->colorImage;
  depthImage = vkSwapChain->depthImage;

  colorImageView = createImageView(vDevice->logicalDevice, colorImage,
                                   imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

  depthImageView = createImageView(
      vDevice->logicalDevice, depthImage, vDevice->findDepthFormat(),
      VK_IMAGE_ASPECT_DEPTH_BIT, vkSwapChain->mipLevels);
  for (size_t i = 0; i < vkSwapChain->swapChainImageViews.size(); i++) {
    view_vector = {colorImageView, depthImageView,
                   vkSwapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(view_vector.size());
    framebufferInfo.pAttachments = view_vector.data();
    framebufferInfo.width = swapChainExtent->width;
    framebufferInfo.height = swapChainExtent->height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &frameBuffer.at(i)) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void GLTF_SkyBox::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  std::array<VkSubpassDescription, 1> subpassDescription{};
  subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription[0].colorAttachmentCount = reference_color_vector.size();
  subpassDescription[0].pColorAttachments = reference_color_vector.data();
  subpassDescription[0].pDepthStencilAttachment = &depthAttachmentRef;
  subpassDescription[0].pResolveAttachments = &colorAttachmentResolveRef;

  std::array<VkSubpassDependency, 2> dependencies{};
  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependencies[0].srcAccessMask = 0;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  dependencies[1].srcSubpass = 0;
  dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependencies[1].srcAccessMask = 0;
  dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                 VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::vector<VkAttachmentDescription> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount =
      static_cast<uint32_t>(subpassDescription.size());
  renderPassInfo.pSubpasses = subpassDescription.data();
  renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
  renderPassInfo.pDependencies = dependencies.data();

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }

  VK_CHECK_RESULT(vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo,
                                     nullptr, &renderPass));

  // Create sampler to sample from the color attachments
  VkSamplerCreateInfo sampler = initializers::samplerCreateInfo();
  sampler.magFilter = VK_FILTER_LINEAR;
  sampler.minFilter = VK_FILTER_LINEAR;
  sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  sampler.addressModeV = sampler.addressModeU;
  sampler.addressModeW = sampler.addressModeU;
  sampler.mipLodBias = 0.0f;
  sampler.maxAnisotropy = 1.0f;
  sampler.minLod = 0.0f;
  sampler.maxLod = 1.0f;
  sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  VK_CHECK_RESULT(vkCreateSampler(vDevice->logicalDevice, &sampler, nullptr,
                                  &this->sampler));
}

void GLTF_SkyBox::bloomRender() {
  offscreenPass.width = 256;
  offscreenPass.height = 256;

  // Find a suitable depth format
  VkFormat fbDepthFormat;
  VkBool32 validDepthFormat =
      tools::getSupportedDepthFormat(vDevice->physicalDevice, &fbDepthFormat);
  assert(validDepthFormat);

  // Create a separate render pass for the offscreen rendering as it may differ
  // from the one used for scene rendering

  std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
  // Color attachment
  attchmentDescriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM;
  attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
  attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attchmentDescriptions[0].finalLayout =
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  // Depth attachment
  attchmentDescriptions[1].format = fbDepthFormat;
  attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
  attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attchmentDescriptions[1].finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorReference = {
      0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  VkAttachmentReference depthReference = {
      1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

  VkSubpassDescription subpassDescription = {};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorReference;
  subpassDescription.pDepthStencilAttachment = &depthReference;

  // Use subpass dependencies for layout transitions
  std::array<VkSubpassDependency, 2> dependencies;

  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  dependencies[1].srcSubpass = 0;
  dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  // Create the actual renderpass
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount =
      static_cast<uint32_t>(attchmentDescriptions.size());
  renderPassInfo.pAttachments = attchmentDescriptions.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpassDescription;
  renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
  renderPassInfo.pDependencies = dependencies.data();

  VK_CHECK_RESULT(vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo,
                                     nullptr, &offscreenPass.renderPass));

  // Create sampler to sample from the color attachments
  VkSamplerCreateInfo sampler = initializers::samplerCreateInfo();
  sampler.magFilter = VK_FILTER_LINEAR;
  sampler.minFilter = VK_FILTER_LINEAR;
  sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  sampler.addressModeV = sampler.addressModeU;
  sampler.addressModeW = sampler.addressModeU;
  sampler.mipLodBias = 0.0f;
  sampler.maxAnisotropy = 1.0f;
  sampler.minLod = 0.0f;
  sampler.maxLod = 1.0f;
  sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  VK_CHECK_RESULT(vkCreateSampler(vDevice->logicalDevice, &sampler, nullptr,
                                  &offscreenPass.sampler));
  // Create two frame buffers
  prepareOffscreenFramebuffer(&offscreenPass.framebuffers[0], FB_COLOR_FORMAT,
                              fbDepthFormat);
  prepareOffscreenFramebuffer(&offscreenPass.framebuffers[1], FB_COLOR_FORMAT,
                              fbDepthFormat);
}

void GLTF_SkyBox::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts;
  layouts.emplace_back(descriptorSetLayout);

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool; // scene->
  allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));

  size_t obj_size = 0;
  if (!textures.empty()) {
    obj_size = textures.size();
    for (size_t k = 0; k < obj_size; k++)
      descriptorWrites.emplace_back(
          initializers::createVkWriteDescriptorTexture(
              k + 1, &textures.at(k)->descriptor, get_descriptor_set()));
  }

  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void GLTF_SkyBox::createDescriptorPool() {
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount = 1;
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount = static_cast<uint32_t>(textures.size());

  poolDrawSize += 1 + textures.size();
}

void GLTF_SkyBox::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo
      vertexInputInfo{}; // = u_ptr_model->getPipelineVertexInputState();
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();
  // auto mt_size = u_ptr_model->get_material_size();
  // for (size_t ct = 0; ct < mt_size; ++ct)
  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void GLTF_SkyBox::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void GLTF_SkyBox::prepareOffscreenFramebuffer(Object::FrameBuffer *frameBuf,
                                              VkFormat colorFormat,
                                              VkFormat depthFormat) {
  // Color attachment
  VkImageCreateInfo image = initializers::imageCreateInfo();
  image.imageType = VK_IMAGE_TYPE_2D;
  image.format = colorFormat;
  image.extent.width = FB_DIM;
  image.extent.height = FB_DIM;
  image.extent.depth = 1;
  image.mipLevels = 1;
  image.arrayLayers = 1;
  image.samples = VK_SAMPLE_COUNT_1_BIT;
  image.tiling = VK_IMAGE_TILING_OPTIMAL;
  // We will sample directly from the color attachment
  image.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
  VkMemoryRequirements memReqs;

  VkImageViewCreateInfo colorImageView = initializers::imageViewCreateInfo();
  colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
  colorImageView.format = colorFormat;
  colorImageView.flags = 0;
  colorImageView.subresourceRange = {};
  colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  colorImageView.subresourceRange.baseMipLevel = 0;
  colorImageView.subresourceRange.levelCount = 1;
  colorImageView.subresourceRange.baseArrayLayer = 0;
  colorImageView.subresourceRange.layerCount = 1;

  VK_CHECK_RESULT(vkCreateImage(vDevice->logicalDevice, &image, nullptr,
                                &frameBuf->color.image));
  vkGetImageMemoryRequirements(vDevice->logicalDevice, frameBuf->color.image,
                               &memReqs);
  memAlloc.allocationSize = memReqs.size;
  memAlloc.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &memAlloc, nullptr,
                                   &frameBuf->color.mem));
  VK_CHECK_RESULT(vkBindImageMemory(
      vDevice->logicalDevice, frameBuf->color.image, frameBuf->color.mem, 0));

  colorImageView.image = frameBuf->color.image;
  VK_CHECK_RESULT(vkCreateImageView(vDevice->logicalDevice, &colorImageView,
                                    nullptr, &frameBuf->color.view));

  // Depth stencil attachment
  image.format = depthFormat;
  image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

  VkImageViewCreateInfo depthStencilView = initializers::imageViewCreateInfo();
  depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
  depthStencilView.format = depthFormat;
  depthStencilView.flags = 0;
  depthStencilView.subresourceRange = {};
  depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  if (tools::formatHasStencil(depthFormat)) {
    depthStencilView.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
  }
  depthStencilView.subresourceRange.baseMipLevel = 0;
  depthStencilView.subresourceRange.levelCount = 1;
  depthStencilView.subresourceRange.baseArrayLayer = 0;
  depthStencilView.subresourceRange.layerCount = 1;

  VK_CHECK_RESULT(vkCreateImage(vDevice->logicalDevice, &image, nullptr,
                                &frameBuf->depth.image));
  vkGetImageMemoryRequirements(vDevice->logicalDevice, frameBuf->depth.image,
                               &memReqs);
  memAlloc.allocationSize = memReqs.size;
  memAlloc.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &memAlloc, nullptr,
                                   &frameBuf->depth.mem));
  VK_CHECK_RESULT(vkBindImageMemory(
      vDevice->logicalDevice, frameBuf->depth.image, frameBuf->depth.mem, 0));

  depthStencilView.image = frameBuf->depth.image;
  VK_CHECK_RESULT(vkCreateImageView(vDevice->logicalDevice, &depthStencilView,
                                    nullptr, &frameBuf->depth.view));

  VkImageView attachments[2];
  attachments[0] = frameBuf->color.view;
  attachments[1] = frameBuf->depth.view;

  VkFramebufferCreateInfo fbufCreateInfo =
      initializers::framebufferCreateInfo();
  fbufCreateInfo.renderPass = offscreenPass.renderPass;
  fbufCreateInfo.attachmentCount = 2;
  fbufCreateInfo.pAttachments = attachments;
  fbufCreateInfo.width = FB_DIM;
  fbufCreateInfo.height = FB_DIM;
  fbufCreateInfo.layers = 1;

  VK_CHECK_RESULT(vkCreateFramebuffer(vDevice->logicalDevice, &fbufCreateInfo,
                                      nullptr, &frameBuf->framebuffer));

  // Fill a descriptor for later use in a descriptor set
  frameBuf->descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  frameBuf->descriptor.imageView = frameBuf->color.view;
  frameBuf->descriptor.sampler = offscreenPass.sampler;
}

void GLTF_SkyBox::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &skybox_ubo, sizeof(skybox_ubo));
}

void GLTF_SkyBox::createAdditinalBuffer() {
  GLTF_Model::createAdditinalBuffer();
}

void GLTF_SkyBox::updateUniformBuffer() {}

///////////////////////////////////////// GLTF MODEl ANIMATE
/////////////////////////////////////////////////////////////
void GLTF_Model_Animate::createDescriptorSets() {
  // Descriptor set for scene matrices
  VkDescriptorSetAllocateInfo allocInfo =
      initializers::descriptorSetAllocateInfo(
          descriptorPool, &descriptorSetLayouts.matrices, 1);
  VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                                           &descriptor));
  VkWriteDescriptorSet writeDescriptorSet =
      initializers::createVkWriteDescriptorBuffer(
          0, &uniformObjectBuffer.descriptor, descriptor);
  vkUpdateDescriptorSets(vDevice->logicalDevice, 1, &writeDescriptorSet, 0,
                         nullptr);

  // Descriptor s,descriptoret for glTF model skin joint matrices
  for (auto &skin : u_ptr_model->getGLTFModel()->skins) {
    const VkDescriptorSetAllocateInfo allocInfo =
        initializers::descriptorSetAllocateInfo(
            descriptorPool, &descriptorSetLayouts.jointMatrices, 1);
    VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                                             &skin.descriptorSet));
    VkWriteDescriptorSet writeDescriptorSet = initializers::writeDescriptorSet(
        skin.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0,
        &skin.ssbo.descriptor);
    vkUpdateDescriptorSets(vDevice->logicalDevice, 1, &writeDescriptorSet, 0,
                           nullptr);
  }

  // // Descriptor sets for glTF model materials
  for (auto &image : u_ptr_model->getGLTFModel()->images) {
    const VkDescriptorSetAllocateInfo allocInfo =
        initializers::descriptorSetAllocateInfo(
            descriptorPool, &descriptorSetLayouts.textures, 1);
    VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                                             &image.descriptorSet));
    VkWriteDescriptorSet writeDescriptorSet = initializers::writeDescriptorSet(
        image.descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
        &image.texture.descriptor);
    vkUpdateDescriptorSets(vDevice->logicalDevice, 1, &writeDescriptorSet, 0,
                           nullptr);
  }
}

void GLTF_Model_Animate::preparePipeline() {
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizationStateCI =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
  VkPipelineColorBlendAttachmentState blendAttachmentStateCI =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlendStateCI =
      initializers::pipelineColorBlendStateCreateInfo(1,
                                                      &blendAttachmentStateCI);
  VkPipelineDepthStencilStateCreateInfo depthStencilStateCI =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineViewportStateCreateInfo viewportStateCI =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampleStateCI =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);
  const std::vector<VkDynamicState> dynamicStateEnables = {
      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicStateCI =
      initializers::pipelineDynamicStateCreateInfo(
          dynamicStateEnables.data(),
          static_cast<uint32_t>(dynamicStateEnables.size()), 0);

  const std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
      initializers::vertexInputBindingDescription(
          0, sizeof(VulkanglTFModelAnimate::Vertex),
          VK_VERTEX_INPUT_RATE_VERTEX),
  };

  const std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
      {0, 0, VK_FORMAT_R32G32B32_SFLOAT,
       offsetof(VulkanglTFModelAnimate::Vertex, pos)},
      {1, 0, VK_FORMAT_R32G32B32_SFLOAT,
       offsetof(VulkanglTFModelAnimate::Vertex, normal)},
      {2, 0, VK_FORMAT_R32G32B32_SFLOAT,
       offsetof(VulkanglTFModelAnimate::Vertex, uv)},
      {3, 0, VK_FORMAT_R32G32B32_SFLOAT,
       offsetof(VulkanglTFModelAnimate::Vertex, color)},
      // POI: Per-Vertex Joint indices and weights are passed to the vertex
      // shader
      {4, 0, VK_FORMAT_R32G32B32A32_SFLOAT,
       offsetof(VulkanglTFModelAnimate::Vertex, jointIndices)},
      {5, 0, VK_FORMAT_R32G32B32A32_SFLOAT,
       offsetof(VulkanglTFModelAnimate::Vertex, jointWeights)},
  };

  VkPipelineVertexInputStateCreateInfo vertexInputStateCI =
      initializers::pipelineVertexInputStateCreateInfo();
  vertexInputStateCI.vertexBindingDescriptionCount =
      static_cast<uint32_t>(vertexInputBindings.size());
  vertexInputStateCI.pVertexBindingDescriptions = vertexInputBindings.data();
  vertexInputStateCI.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertexInputAttributes.size());
  vertexInputStateCI.pVertexAttributeDescriptions =
      vertexInputAttributes.data();

  VkGraphicsPipelineCreateInfo pipelineInfo =
      initializers::pipelineCreateInfo(pipelineLayout, vDevice->renderPass, 0);
  pipelineInfo.pVertexInputState = &vertexInputStateCI;
  pipelineInfo.pInputAssemblyState = &inputAssemblyStateCI;
  pipelineInfo.pRasterizationState = &rasterizationStateCI;
  pipelineInfo.pColorBlendState = &colorBlendStateCI;
  pipelineInfo.pMultisampleState = &multisampleStateCI;
  pipelineInfo.pViewportState = &viewportStateCI;
  pipelineInfo.pDepthStencilState = &depthStencilStateCI;
  pipelineInfo.pDynamicState = &dynamicStateCI;
  pipelineInfo.stageCount = static_cast<uint32_t>(shadersStages.size());
  pipelineInfo.pStages = shadersStages.data();

  // Solid rendering pipeline
  VK_CHECK_RESULT(vkCreateGraphicsPipelines(
      vDevice->logicalDevice, pipelineCache, 1, &pipelineInfo, nullptr,
      &pipeline)); // piplines.solid

  // Wire frame rendering pipeline
  // if (vDevice->features.fillModeNonSolid)
  // {
  //     rasterizationStateCI.polygonMode = VK_POLYGON_MODE_LINE;
  //     rasterizationStateCI.lineWidth = 1.0f;
  //     VK_CHECK_RESULT(vkCreateGraphicsPipelines(vDevice->logicalDevice,
  //     pipelineCache, 1, &pipelineInfo, nullptr, &pipelines.wireframe));
  // }
}

void GLTF_Model_Animate::createDescriptorPool() {
  vkPoolSizes = {
      initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
      initializers::descriptorPoolSize(
          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          static_cast<uint32_t>(u_ptr_model->getGLTFModel()->images.size())),
      initializers::descriptorPoolSize(
          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          static_cast<uint32_t>(u_ptr_model->getGLTFModel()->skins.size()))};

  const uint32_t maxSetCount =
      static_cast<uint32_t>(u_ptr_model->getGLTFModel()->images.size()) +
      static_cast<uint32_t>(u_ptr_model->getGLTFModel()->skins.size()) + 1;

  poolDrawSize += maxSetCount;
}

void GLTF_Model_Animate::setDescriptorLayout() {
  VkDescriptorSetLayoutBinding setLayoutBinding{};
  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI =
      initializers::descriptorSetLayoutCreateInfo(&setLayoutBinding, 1);

  setLayoutBinding = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(vDevice->logicalDevice,
                                              &descriptorSetLayoutCI, nullptr,
                                              &descriptorSetLayouts.matrices));

  setLayoutBinding = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
      0);
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(vDevice->logicalDevice,
                                              &descriptorSetLayoutCI, nullptr,
                                              &descriptorSetLayouts.textures));

  setLayoutBinding = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(
      vDevice->logicalDevice, &descriptorSetLayoutCI, nullptr,
      &descriptorSetLayouts.jointMatrices));

  vkDescriptorLayouts.emplace_back(descriptorSetLayouts.matrices);
  vkDescriptorLayouts.emplace_back(descriptorSetLayouts.jointMatrices);
  vkDescriptorLayouts.emplace_back(descriptorSetLayouts.textures);

  VkPipelineLayoutCreateInfo pipelineLayoutCI =
      initializers::pipelineLayoutCreateInfo(
          vkDescriptorLayouts.data(),
          static_cast<uint32_t>(vkDescriptorLayouts.size()));

  // We will use push constants to push the local matrices of a primitive to the
  // vertex shader
  VkPushConstantRange pushConstantRange = initializers::pushConstantRange(
      VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0);
  // Push constant ranges are part of the pipeline layout
  pipelineLayoutCI.pushConstantRangeCount = 1;
  pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
  VK_CHECK_RESULT(vkCreatePipelineLayout(
      vDevice->logicalDevice, &pipelineLayoutCI, nullptr, &pipelineLayout));
}

void GLTF_Model_Animate::setObjectInfo(
    pipeline_parameters *_parameters,
    VkGraphicsPipelineCreateInfo *pipelineInfo) {
  //    Vertex::vertexInputBindingDescription =
  //    Vertex::inputBindingDescription(0);
  //
  //    Vertex::vertexInputAttributeDescriptions.resize(6);
  //    Vertex::vertexInputAttributeDescriptions = {
  //            Vertex::inputAttributeDescription(0, 0,
  //            VertexComponent::Position), Vertex::inputAttributeDescription(0,
  //            1, VertexComponent::Normal),
  //            Vertex::inputAttributeDescription(0, 2, VertexComponent::UV),
  //            Vertex::inputAttributeDescription(0, 3, VertexComponent::Color),
  //            Vertex::inputAttributeDescription(0, 4,
  //            VertexComponent::Joint0), Vertex::inputAttributeDescription(0,
  //            5, VertexComponent::Weight0),
  //    };
  //
  //    _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  //    _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
  //    static_cast<uint32_t>(Vertex::vertexInputAttributeDescriptions.size());
  //    _parameters->vertexInputInfo->pVertexBindingDescriptions =
  //    &Vertex::vertexInputBindingDescription;
  //    _parameters->vertexInputInfo->pVertexAttributeDescriptions =
  //    Vertex::vertexInputAttributeDescriptions.data();
  //
  //    _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  //    _parameters->colorBlendAttachment->srcColorBlendFactor =
  //    VK_BLEND_FACTOR_SRC_ALPHA;
  //    _parameters->colorBlendAttachment->dstColorBlendFactor =
  //    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  //    _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  //    _parameters->colorBlendAttachment->srcAlphaBlendFactor =
  //    VK_BLEND_FACTOR_ONE;
  //    _parameters->colorBlendAttachment->dstAlphaBlendFactor =
  //    VK_BLEND_FACTOR_ZERO; _parameters->colorBlendAttachment->alphaBlendOp =
  //    VK_BLEND_OP_ADD; _parameters->colorBlendAttachment->colorWriteMask =
  //            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
  //            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  //
  //    _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  //    _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

void GLTF_Model_Animate::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &gltf_animate_ubo,
         sizeof(gltf_animate_ubo));
}

void GLTF_Model_Animate::createRenderPass(VkFormat format) {
  //    GLTF_Model::createRenderPass(format);
}

void GLTF_Model_Animate::draw(VkCommandBuffer _buffer) {
  // All vertices and indices are stored in single buffers, so we only need to
  // bind once
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 0, 1, &descriptor, 0, nullptr);
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  u_ptr_model->getGLTFModel()->draw(_buffer, pipelineLayout);
}

GLTF_Model_Animate::GLTF_Model_Animate(std::string object_path) : Object() {
  u_ptr_model = std::make_unique<AnimGLTF_Model_Impl>();
  obj_path = std::move(object_path);
}

void GLTF_Model_Animate::initialization() {
  u_ptr_model->initialize(vDevice);
  u_ptr_model->loadglTFFile(obj_path);
}

void GLTF_Model_Animate::destroy() {
  vkDestroyDescriptorSetLayout(vDevice->logicalDevice,
                               descriptorSetLayouts.matrices, nullptr);
  vkDestroyDescriptorSetLayout(vDevice->logicalDevice,
                               descriptorSetLayouts.textures, nullptr);
  vkDestroyDescriptorSetLayout(vDevice->logicalDevice,
                               descriptorSetLayouts.jointMatrices, nullptr);
  u_ptr_model->destroy();
}

void GLTF_Model_Animate::update(float frame_time) {
  u_ptr_model->getGLTFModel()->updateAnimation(frame_time);
}

void GLTF_Model_Animate::prepare() {}

void GLTF_Model_Animate::loadTexture(VkImageViewType type) {}

void GLTF_Model_Animate::createUniformBuffer() {
  VK_CHECK_RESULT(
      vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &uniformObjectBuffer));
  VK_CHECK_RESULT(uniformObjectBuffer.map());
  updateMapped();
}

VkDeviceSize GLTF_Model_Animate::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferAnimate));
}

uint32_t GLTF_Model_Animate::getTexturesSize() { return 0; }

VkDescriptorImageInfo *
GLTF_Model_Animate::get_descriptor_image(size_t tex_idx) {
  return nullptr;
}

viBuffer *GLTF_Model_Animate::getBuffer() { return nullptr; }

std::vector<uint32_t> *GLTF_Model_Animate::getIndices() { return nullptr; }

GLTF_Model_Animate::~GLTF_Model_Animate() {}

void GLTF_Model_Animate::createAdditinalBuffer() {}

void GLTF_Model_Animate::updateUniformBuffer() {}

void GLTF_Model::createUniformBuffer() {}

void GLTF_Model::set_visible_node(size_t index, bool flag) {
  u_ptr_model->set_visible(index, flag);
}

GLTF_Model::GLTF_Model(std::string object_path) : Object() {
  u_ptr_model = std::make_unique<GLTF_CImpl>();
  obj_path = std::move(object_path);
  // FIXME: Correct normal map settings
  u_ptr_model->enable_normalMap();
  u_ptr_model->increase_ubo();
}

void GLTF_Model::initialization() {
  u_ptr_model->initialization();
  uint32_t flags = vkglTF::FileLoadingFlags::PreTransformVertices |
                   vkglTF::FileLoadingFlags::PreMultiplyVertexColors |
                   vkglTF::FileLoadingFlags::FlipY;
  u_ptr_model->scene_load(obj_path, vDevice, vDevice->queue, flags);
}

void GLTF_Model::destroy() {
  //    delete scene;
  for (auto &tex : textures) {
    tex->destroy();
  }
  u_ptr_model->destroy();
}

uint32_t GLTF_Model::getNodesSize() const {
  return u_ptr_model->get_node_size();
}

uint32_t GLTF_Model::getLinearNodesSize() const {
  return u_ptr_model->get_linearNode_size();
}

uint32_t GLTF_Model::getMaterialsSize() const {
  return u_ptr_model->get_material_size();
}

uint32_t GLTF_Model::getSkinSize() const {
  return u_ptr_model->get_skin_size();
}

void GLTF_Model::preparePipeline() {
  // For compute work make universal
  prepareInputs();
  // prepareUniformBuffers();

  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo *vertexInputInfo =
      u_ptr_model->getPipelineVertexInputState();
  // vertexInputInfo.sType =
  // VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  // setObjectInfo(&_parameters, &pipelineInfo);
  // std::vector<VkPipelineShaderStageCreateInfo> _shadersStages =
  // {shadersStages[0], shadersStages[1]};
  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();
  // For double sided materials, culling will be disabled
  // rasterizationStateCI.cullMode = material.doubleSided ? VK_CULL_MODE_NONE :
  // VK_CULL_MODE_BACK_BIT

  auto mt_size = u_ptr_model->get_material_size();
  for (size_t ct = 0; ct < mt_size; ++ct) {
    struct MaterialSpecializationData {
      VkBool32 alphaMask;
      float alphaMaskCutoff;
    } materialSpecializationData{};
    std::vector<VkSpecializationMapEntry> specializationMapEntries;
    VkSpecializationInfo specializationInfo;

    materialSpecializationData.alphaMask = u_ptr_model->get_material_alpha(ct);
    materialSpecializationData.alphaMaskCutoff =
        u_ptr_model->get_material_alphacutoff(ct);

    // POI: Constant fragment shader material parameters will be set using
    // specialization constants
    specializationMapEntries = {
        initializers::specializationMapEntry(
            0, offsetof(MaterialSpecializationData, alphaMask),
            sizeof(MaterialSpecializationData::alphaMask)),
        initializers::specializationMapEntry(
            1, offsetof(MaterialSpecializationData, alphaMaskCutoff),
            sizeof(MaterialSpecializationData::alphaMaskCutoff)),
    };
    specializationInfo = initializers::specializationInfo(
        specializationMapEntries, sizeof(materialSpecializationData),
        &materialSpecializationData);
    auto shader_fragment =
        std::find_if(shadersStages.begin(), shadersStages.end(),
                     [](VkPipelineShaderStageCreateInfo &_shader) {
                       return _shader.stage == VK_SHADER_STAGE_FRAGMENT_BIT;
                     });
    (*shader_fragment).pSpecializationInfo = &specializationInfo;

    rasterizer.cullMode = u_ptr_model->get_doublesided(ct)
                              ? VK_CULL_MODE_NONE
                              : VK_CULL_MODE_BACK_BIT;
    VK_CHECK_RESULT(vkCreateGraphicsPipelines(
        vDevice->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
        u_ptr_model->get_material_pipeline(ct)));
  }
}

void GLTF_Model::setObjectInfo(pipeline_parameters *_parameters,
                               VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(5);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      vertex.inputAttributeDescription(0, 3, VertexComponent::Color),
      vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent)};

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  // _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  // _parameters->colorBlendAttachment->srcColorBlendFactor =
  // VK_BLEND_FACTOR_SRC_ALPHA;
  // _parameters->colorBlendAttachment->dstColorBlendFactor =
  // VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  // _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  // _parameters->colorBlendAttachment->srcAlphaBlendFactor =
  // VK_BLEND_FACTOR_ONE; _parameters->colorBlendAttachment->dstAlphaBlendFactor
  // = VK_BLEND_FACTOR_ZERO; _parameters->colorBlendAttachment->alphaBlendOp =
  // VK_BLEND_OP_ADD; _parameters->colorBlendAttachment->colorWriteMask =
  // VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
  // VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->depthStencil->depthWriteEnable = VK_TRUE;
  _parameters->depthStencil->depthTestEnable = VK_TRUE;
  _parameters->depthStencil->depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_CLOCKWISE;
  _parameters->rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
}

void GLTF_Model::update(float frame_time) {}

void GLTF_Model::prepare() {}

void GLTF_Model::createDescriptorSets() {}

void GLTF_Model::createDescriptorPool() {}

void GLTF_Model::draw(VkCommandBuffer _buffer) {
  if (mouse_position != nullptr)
    manage_constant.mousePos = *mouse_position;

  /// set push constant for mouse value and unique selected id
  vkCmdPushConstants(_buffer, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                     sizeof(managePushConstant), &manage_constant);
  u_ptr_model->draw(_buffer, vkglTF::RenderFlags::BindImages, pipelineLayout);
}

void GLTF_Model::loadTexture(VkImageViewType type) {
  for (const auto &tex_path : textures_paths) {
    auto *d_texture = new Texture2DKTX();
    d_texture->loadFromFile(tex_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                            vDevice->queue);
    textures.push_back(d_texture);
  }
}

void GLTF_Model::setDescriptorLayout() {
  vkDescriptorLayouts.emplace_back(vkglTF::descriptorSetLayoutUbo);
  vkDescriptorLayouts.emplace_back(vkglTF::descriptorSetLayoutImage);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(vkDescriptorLayouts.size());
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();
  //
  VkPushConstantRange pushConstantRange = initializers::pushConstantRange(
      VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(managePushConstant), 0);
  // Push constant ranges are part of the pipeline layout
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

VkDeviceSize GLTF_Model::getBufferSize() {
  return VkDeviceSize(sizeof(UniformBufferObject));
}

uint32_t GLTF_Model::getTexturesSize() {
  return static_cast<uint32_t>(u_ptr_model->texture_size() + textures.size());
}

VkDescriptorImageInfo *GLTF_Model::get_descriptor_image(size_t tex_idx) {
  // system to take textures descriptor
  // in first take loading textures
  int check;
  check = (textures.size() - 1);
  if (check < 0) {
    return u_ptr_model->descriptor(tex_idx);
  }

  if (check < tex_idx) {
    return u_ptr_model->descriptor(tex_idx - textures.size() - 1);
  } else {
    return &textures.at(tex_idx)->descriptor;
  }
}

viBuffer *GLTF_Model::getBuffer() { return {}; }

std::vector<uint32_t> *GLTF_Model::getIndices() {
  return new std::vector<uint32_t>();
}

GLTF_Model::~GLTF_Model() {}

void GLTF_Model::updateMapped() {
  // memcpy(uniformObjectBuffer.mapped, &gltf_ubo, sizeof(gltf_ubo));
}

void GLTF_Model::updateUBO(UniformBufferObject *data, size_t mesh_id) {
  u_ptr_model->updateUbo(data, mesh_id);
}

void GLTF_Model::createAdditinalBuffer() {
  // prepareCompute();
}

void GLTF_Model::releaseBarrier(VkCommandBuffer _buffer) {
  // if (vDevice->queueFamilyIndices.graphics !=
  // u_ptr_compute->queueFamilyIndex) {
  //   VkBufferMemoryBarrier buffer_barrier =
  //   {
  //     VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
  //     nullptr,
  //     VK_ACCESS_SHADER_WRITE_BIT,
  //     VK_ACCESS_SHADER_READ_BIT,
  //     vDevice->queueFamilyIndices.graphics,
  //     u_ptr_compute->queueFamilyIndex,
  //     u_ptr_compute->storageBuffer.buffer,
  //     0,
  //     u_ptr_compute->storageBuffer.size
  //   };
  //
  //   vkCmdPipelineBarrier(
  //     _buffer,
  //     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
  //     VK_PIPELINE_STAGE_TRANSFER_BIT,
  //     0,
  //     0, nullptr,
  //     1, &buffer_barrier,
  //     0, nullptr);
  // }
  //
  // VkBufferCopy copy = {0, 0, u_ptr_model->get_vertex_size() *
  // sizeof(vert_ubo_vertices_share)}; vkCmdCopyBuffer(_buffer,
  // u_ptr_compute->hitBuffer.buffer, u_ptr_compute->storageBuffer.buffer, 1,
  // &copy);
}

void GLTF_Model::readShaderData() {
  void *data;
  vkMapMemory(vDevice->logicalDevice, pickObjectBuffer.memory, 0,
              sizeof(uint32_t) * DEPTH_ARRAY_SCALE, 0, (void **)&data);
  auto *pickingData = static_cast<uint32_t *>(data);

  for (uint32_t i = 0; i < DEPTH_ARRAY_SCALE; ++i) {
    if (pickingData[i] != 0) {
      selectedId = pickingData[i];
      break;
    }
  }
  // TODO: FOR DEBUG
  //  if (selectedId != 0) {
  //    std::cout << "Выбран объект с ID: " << selectedId << std::endl;
  //  } else {
  //    std::cout << "Объект не выбран" << std::endl;
  //  }

  manage_constant.selected_unique_ID = selectedId;

  memset(data, 0, sizeof(uint32_t) * DEPTH_ARRAY_SCALE);
  vkUnmapMemory(vDevice->logicalDevice, pickObjectBuffer.memory);
}

void GLTF_Model::updateUniformBuffer() {}

uint32_t GLTF_Model::idSelected() const { return selectedId; }

void GLTF_Model::prepareInputs() {
  // Initial particle positions

  // VkDeviceSize storageBufferSize = u_ptr_model->get_vertex_size() *
  // sizeof(glm::vec2);
  //
  // enma::Buffer stagingBuffer;
  // vDevice->createBuffer(
  //   // The SSBO will be used as a storage buffer for the compute pipeline and
  //   as a vertex buffer in the graphics pipeline storageBufferSize,
  //   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
  //   u_ptr_model->getVertexBuffer().data()
  // );
  //
  // vDevice->createBuffer(
  //   sizeof(int),
  //   // The SSBO will be used as a storage buffer for the compute pipeline and
  //   as a vertex buffer in the graphics pipeline
  //   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
  //   VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
  //   &u_ptr_compute->storageBuffer
  // );

  // Create buffer for get data from compute shader
  //  vDevice->createBuffer(
  //    // The SSBO will be used as a storage buffer for the compute pipeline
  //    and as a vertex buffer in the graphics pipeline sizeof(int),
  //    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
  //    &u_ptr_compute->hitBuffer
  //  );

  // Copy from staging buffer to storage buffer
  // VkCommandBuffer copyCmd =
  // vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
  // VkBufferCopy copyRegion = {};
  // copyRegion.size = sizeof(int);
  // vkCmdCopyBuffer(copyCmd, stagingBuffer.buffer,
  // u_ptr_compute->storageBuffer.buffer, 1, &copyRegion);

  // Execute a transfer barrier to the compute queue, if necessary
  // if (vDevice->queueFamilyIndices.graphics !=
  // u_ptr_compute->queueFamilyIndex) {
  //   VkBufferMemoryBarrier buffer_barrier =
  //   {
  //     VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
  //     nullptr,
  //     VK_ACCESS_SHADER_WRITE_BIT,
  //     VK_ACCESS_SHADER_READ_BIT,
  //     vDevice->queueFamilyIndices.graphics,
  //     u_ptr_compute->queueFamilyIndex,
  //     u_ptr_compute->storageBuffer.buffer,
  //     0,
  //     u_ptr_compute->storageBuffer.size
  //   };
  //
  //   vkCmdPipelineBarrier(
  //     copyCmd,
  //     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
  //     VK_PIPELINE_STAGE_TRANSFER_BIT,
  //     0,
  //     0, nullptr,
  //     1, &buffer_barrier,
  //     0, nullptr);
  // }
  // vDevice->endSingleTimeCommands(copyCmd, vDevice->queue, true);

  // stagingBuffer.destroy();
}

void GLTF_Model::prepareCompute() {
  // Create a compute capable vDevice->logicalDevice queue
  // The VulkanDevice::createLogicalDevice functions finds a compute capable
  // queue and prefers queue families that only support compute Depending on the
  // implementation this may result in different queue family indices for
  // graphics and computes, requiring proper synchronization (see the memory and
  // pipeline barriers) vkGetDeviceQueue(vDevice->logicalDevice,
  // u_ptr_compute->queueFamilyIndex, 0, &u_ptr_compute->queue);
  //
  // // Create compute pipeline
  // // Compute pipelines are created separate from graphics pipelines even if
  // they use the same queue (family index)
  //
  // std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
  //   // Binding 0 : Particle position storage buffer
  //   initializers::descriptorSetLayoutBinding(
  //     VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
  //     VK_SHADER_STAGE_COMPUTE_BIT,
  //     0),
  //   // Binding 1 : Uniform buffer
  //   initializers::descriptorSetLayoutBinding(
  //     VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
  //     VK_SHADER_STAGE_COMPUTE_BIT,
  //     1)
  // };
  //
  // VkDescriptorSetLayoutCreateInfo descriptorLayout =
  //     initializers::descriptorSetLayoutCreateInfo(
  //       setLayoutBindings.data(),
  //       static_cast<uint32_t>(setLayoutBindings.size()));
  //
  // VK_CHECK_RESULT(
  //   vkCreateDescriptorSetLayout(vDevice->logicalDevice, &descriptorLayout,
  //   nullptr,
  //     &u_ptr_compute->descriptorSetLayout));
  //
  // VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
  //     initializers::pipelineLayoutCreateInfo(
  //       &u_ptr_compute->descriptorSetLayout,
  //       1);
  //
  // VkPushConstantRange pushConstantRange = initializers::pushConstantRange(
  //   VK_SHADER_STAGE_COMPUTE_BIT, sizeof(vert_ubo_vertices_share), 0);
  // // Push constant ranges are part of the pipeline layout
  // pPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  // pPipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
  //
  //
  // VK_CHECK_RESULT(
  //   vkCreatePipelineLayout(vDevice->logicalDevice,
  //   &pPipelineLayoutCreateInfo, nullptr,
  //     &u_ptr_compute->pipelineLayout));
  //
  // VkDescriptorSetAllocateInfo allocInfo =
  //     initializers::descriptorSetAllocateInfo(
  //       descriptorPool,
  //       &u_ptr_compute->descriptorSetLayout,
  //       1);
  //
  // VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice,
  // &allocInfo, &u_ptr_compute->descriptorSet));
  //
  // std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets =
  // {
  //   // Binding 0 : Particle position storage buffer
  //   initializers::writeDescriptorSet(
  //     u_ptr_compute->descriptorSet,
  //     VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
  //     0,
  //     &u_ptr_compute->storageBuffer.descriptor),
  //   // Binding 1 : Uniform buffer
  //   initializers::writeDescriptorSet(
  //     u_ptr_compute->descriptorSet,
  //     VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
  //     1,
  //     &u_ptr_compute->hitBuffer.descriptor),
  // };
  //
  // vkUpdateDescriptorSets(vDevice->logicalDevice,
  // static_cast<uint32_t>(computeWriteDescriptorSets.size()),
  //                        computeWriteDescriptorSets.data(), 0, nullptr);
  //
  // // Create pipeline
  // VkComputePipelineCreateInfo computePipelineCreateInfo =
  // initializers::computePipelineCreateInfo(
  //   u_ptr_compute->pipelineLayout, 0);
  // computePipelineCreateInfo.stage = shadersStages[2];
  //
  // //loadShader(getShadersPath() + "computeparticles/particle.comp.spv",
  // // VK_SHADER_STAGE_COMPUTE_BIT);
  // VK_CHECK_RESULT(
  //   vkCreateComputePipelines(vDevice->logicalDevice, pipelineCache, 1,
  //   &computePipelineCreateInfo, nullptr,
  //     &u_ptr_compute->pipeline));
  //
  // // Separate command pool as queue family for compute may be different than
  // graphics VkCommandPoolCreateInfo cmdPoolInfo = {}; cmdPoolInfo.sType =
  // VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; cmdPoolInfo.queueFamilyIndex =
  // u_ptr_compute->queueFamilyIndex; cmdPoolInfo.flags =
  // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  // VK_CHECK_RESULT(vkCreateCommandPool(vDevice->logicalDevice, &cmdPoolInfo,
  // nullptr, &u_ptr_compute->commandPool));
  //
  // // Create a command buffer for compute operations
  // u_ptr_compute->commandBuffer =
  // vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  //                                                                 u_ptr_compute->commandPool,
  //                                                                 false);
  //
  // // Semaphore for compute & graphics sync
  // VkSemaphoreCreateInfo semaphoreCreateInfo =
  // initializers::semaphoreCreateInfo();
  //
  // VK_CHECK_RESULT(
  //   vkCreateSemaphore(vDevice->logicalDevice, &semaphoreCreateInfo, nullptr,
  //   &u_ptr_compute->compute));
  //
  // prepareBuildComputeBuffer();
}

void GLTF_Model::prepareBuildComputeBuffer() {
  VkCommandBufferBeginInfo cmdBufInfo = initializers::commandBufferBeginInfo();

  VK_CHECK_RESULT(
      vkBeginCommandBuffer(u_ptr_compute->commandBuffer, &cmdBufInfo));

  // Compute particle movement

  // Add memory barrier to ensure that the (graphics) vertex shader has fetched
  // attributes before compute starts to write to the buffer
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        0,
        VK_ACCESS_SHADER_WRITE_BIT,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->queueFamilyIndex,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(u_ptr_compute->commandBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }

  VkDeviceSize offsets[1] = {0};
  vkCmdBindVertexBuffers(u_ptr_compute->commandBuffer, 0, 1,
                         &u_ptr_compute->storageBuffer.buffer, offsets);

  // TODO: PushConstant check u_ptr_compute buffer if need
  if (mouse_position) {
    mouse_vec2_ = *mouse_position;
    glm::vec2 cur_window = {*vDevice->uWidth(), *vDevice->uHeight()};
    mouse_vec2_ = mouse_vec2_ / cur_window;
  }
  vkCmdPushConstants(u_ptr_compute->commandBuffer,
                     u_ptr_compute->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT,
                     0, sizeof(glm::vec2), &mouse_vec2_);

  // Dispatch the compute job
  vkCmdBindPipeline(u_ptr_compute->commandBuffer,
                    VK_PIPELINE_BIND_POINT_COMPUTE, u_ptr_compute->pipeline);
  vkCmdBindDescriptorSets(
      u_ptr_compute->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
      u_ptr_compute->pipelineLayout, 0, 1, &u_ptr_compute->descriptorSet, 0, 0);
  uint32_t vertices_size = (u_ptr_model->get_vertex_size() + 256 - 1) / 256;
  vkCmdDispatch(u_ptr_compute->commandBuffer, vertices_size, 1, 1);

  // Add barrier to ensure that compute shader has finished writing to the
  // buffer Without this the (rendering) vertex shader may display incomplete
  // results (partial data from last frame)
  if (vDevice->queueFamilyIndices.graphics != u_ptr_compute->queueFamilyIndex) {
    VkBufferMemoryBarrier buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_SHADER_WRITE_BIT,
        0,
        u_ptr_compute->queueFamilyIndex,
        vDevice->queueFamilyIndices.graphics,
        u_ptr_compute->storageBuffer.buffer,
        0,
        u_ptr_compute->storageBuffer.size};

    vkCmdPipelineBarrier(u_ptr_compute->commandBuffer,
                         VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1,
                         &buffer_barrier, 0, nullptr);
  }

  vkEndCommandBuffer(u_ptr_compute->commandBuffer);
}

void GLTF_Model::prepareUniformBuffers() {
  // Compute shader uniform buffer block
  // vDevice->createBuffer(
  //   sizeof(vert_ubo_vertices_share) * u_ptr_model->get_vertex_size(),
  //   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
  //   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  //   &u_ptr_compute->uniformBuffer
  // );
  //
  // // Map for host access
  // VK_CHECK_RESULT(u_ptr_compute->uniformBuffer.map());
}

Transparent_Model::Transparent_Model(std::string _path)
    : Model(std::move(_path), model_type::from_obj_file) {
  subpass_layout = 1;
  render_flags = ObjectRenderFlags::TRNOBJECT;
}

void Transparent_Model::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  // fixme: changed for viewport
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void Transparent_Model::setObjectInfo(
    pipeline_parameters *_parameters,
    VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(4);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Color),
      vertex.inputAttributeDescription(0, 2, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 3, VertexComponent::UV),
  };

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  _parameters->colorBlendAttachment->srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  _parameters->colorBlendAttachment->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  _parameters->colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

void Transparent_Model::draw(VkCommandBuffer _buffer) {
  if (!is_object_visible)
    return;
  //        vkCmdNextSubpass(_buffer, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, NULL);

  const VkDeviceSize offsets[1] = {0};
  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices()->size()), 1, 0,
                   0, 0);
}

VkDeviceSize Transparent_Model::getBufferSize() {
  return static_cast<VkDeviceSize>(sizeof(UniformBufferTransparent));
}

void Transparent_Model::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &trn_ubo, sizeof(trn_ubo));
}

void Transparent_Model::createAdditinalBuffer() {
  Model::createAdditinalBuffer();
}

void Transparent_Model::updateUniformBuffer() {}

TextOverlay::TextOverlay()
    : Object(), frameBufferWidth(nullptr), frameBufferHeight(nullptr),
      scale(0) {}

TextOverlay::~TextOverlay() = default;

void TextOverlay::prepareResources() {}

void TextOverlay::preparePipeline() {
  // Enable blending, using alpha from red channel of the font texture (see
  // text.frag)
  VkPipelineColorBlendAttachmentState blendAttachmentState{};
  blendAttachmentState.blendEnable = VK_TRUE;
  blendAttachmentState.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  blendAttachmentState.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  blendAttachmentState.srcAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizationState =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
  VkPipelineColorBlendStateCreateInfo colorBlendState =
      initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
  VkPipelineDepthStencilStateCreateInfo depthStencilState =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
  // fixme changed for viewport
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampleState =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

  std::array<VkVertexInputBindingDescription, 2> vertexInputBindings = {
      initializers::vertexInputBindingDescription(0, sizeof(glm::vec4),
                                                  VK_VERTEX_INPUT_RATE_VERTEX),
      initializers::vertexInputBindingDescription(1, sizeof(glm::vec4),
                                                  VK_VERTEX_INPUT_RATE_VERTEX),
  };
  std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributes = {
      initializers::vertexInputAttributeDescription(0, 0,
                                                    VK_FORMAT_R32G32_SFLOAT,
                                                    0), // Location 0: Position
      initializers::vertexInputAttributeDescription(
          1, 1, VK_FORMAT_R32G32_SFLOAT,
          sizeof(glm::vec2)), // Location 1: UV
  };

  VkPipelineVertexInputStateCreateInfo vertexInputState =
      initializers::pipelineVertexInputStateCreateInfo();
  vertexInputState.vertexBindingDescriptionCount =
      static_cast<uint32_t>(vertexInputBindings.size());
  vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
  vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertexInputAttributes.size());
  vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

  VkGraphicsPipelineCreateInfo pipelineCreateInfo =
      initializers::pipelineCreateInfo(pipelineLayout, vDevice->renderPass, 0);
  pipelineCreateInfo.pVertexInputState = &vertexInputState;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
  pipelineCreateInfo.pRasterizationState = &rasterizationState;
  pipelineCreateInfo.pColorBlendState = &colorBlendState;
  pipelineCreateInfo.pMultisampleState = &multisampleState;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pDepthStencilState = &depthStencilState;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.stageCount = static_cast<uint32_t>(shadersStages.size());
  pipelineCreateInfo.pStages = shadersStages.data();
  pipelineCreateInfo.subpass = subpass_layout; // TODO: see that is it

  VK_CHECK_RESULT(
      vkCreateGraphicsPipelines(vDevice->logicalDevice, pipelineCache, 1,
                                &pipelineCreateInfo, nullptr, &pipeline));
}

void TextOverlay::prepareRenderPass() {
  // std::vector<VkAttachmentDescription> attachments;
  // attachments.resize(3);
  // // Color attachment
  // attachments[0].format = colorFormat;
  // attachments[0].samples = vDevice->msaaSamples;
  // // Don't clear the framebuffer (like the renderpass from the example does)
  // attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  // attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  // attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // attachments[0].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  // attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  //
  // // Depth attachment
  // attachments[1].format = depthFormat;
  // attachments[1].samples = vDevice->msaaSamples;
  // attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  // attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  // attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  // attachments[1].finalLayout =
  // VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  //
  // attachments[2].format = colorFormat;
  // attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
  // attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  // attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  // attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  // attachments[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  //
  // VkAttachmentReference colorReference = {};
  // colorReference.attachment = 0;
  // colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  //
  // VkAttachmentReference depthReference = {};
  // depthReference.attachment = 1;
  // depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  //
  // VkAttachmentReference colorAttachmentResolveRef{};
  // colorAttachmentResolveRef.attachment = 2;
  // colorAttachmentResolveRef.layout =
  // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  //
  // // Use subpass dependencies for image layout transitions
  // VkSubpassDependency subpassDependencies[2] = {};
  //
  // // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all
  // commands executed outside of the actual renderpass)
  // subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  // subpassDependencies[0].dstSubpass = 0;
  // subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  // subpassDependencies[0].dstStageMask =
  // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  // subpassDependencies[0].dstAccessMask =
  //     VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
  //     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  // subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  //
  // // Transition from initial to final
  // subpassDependencies[1].srcSubpass = 0;
  // subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  // subpassDependencies[1].srcStageMask =
  // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  // subpassDependencies[1].srcAccessMask =
  //     VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
  //     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  // subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  // subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  //
  // VkSubpassDescription subpassDescription = {};
  // subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  // subpassDescription.flags = 0;
  // subpassDescription.inputAttachmentCount = 0;
  // subpassDescription.pInputAttachments = NULL;
  // subpassDescription.colorAttachmentCount = 1;
  // subpassDescription.pColorAttachments = &colorReference;
  // subpassDescription.pResolveAttachments = &colorAttachmentResolveRef;
  // subpassDescription.pDepthStencilAttachment = &depthReference;
  // subpassDescription.preserveAttachmentCount = 0;
  // subpassDescription.pPreserveAttachments = NULL;
  //
  // VkRenderPassCreateInfo renderPassInfo = {};
  // renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  // renderPassInfo.pNext = NULL;
  // renderPassInfo.attachmentCount = attachments.size();
  // renderPassInfo.pAttachments = attachments.data();
  // renderPassInfo.subpassCount = 1;
  // renderPassInfo.pSubpasses = &subpassDescription;
  // renderPassInfo.dependencyCount = 2;
  // renderPassInfo.pDependencies = subpassDependencies;
  //
  // VK_CHECK_RESULT(vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo,
  // nullptr, &renderPass));
  // // for (size_t i = 0; i < frameBuffers.size(); i++)
  // // {
  // //     VkFramebufferCreateInfo framebufferInfo{};
  // //     framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  // //     framebufferInfo.renderPass = renderPass;
  // //     framebufferInfo.attachmentCount =
  // static_cast<uint32_t>(attachments.size());
  // //     framebufferInfo.pAttachments = attachments.data();
  // //     framebufferInfo.width = *frameBufferWidth;
  // //     framebufferInfo.height = *frameBufferHeight;
  // //     framebufferInfo.layers = 1;
  //
  // //     if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo,
  // nullptr, frameBuffers[i]) != VK_SUCCESS)
  // //     {
  // //         throw std::runtime_error("failed to create framebuffer!");
  // //     }
  // // }
}

void TextOverlay::createFramebuffers(VulkanSwapChain *vkSwapChain) {}

void TextOverlay::updateScale(float nScale) { scale = nScale; }

void TextOverlay::updateFrameSize(uint32_t *width, uint32_t *height) {
  frameBufferHeight = height;
  frameBufferWidth = width;
}

void TextOverlay::beginTextUpdate() {
  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, memory, 0, VK_WHOLE_SIZE,
                              0, (void **)&mapped));
  numLetters = 0;
}

void TextOverlay::addText(std::string text, float x, float y,
                          TextOverlay::TextAlign align) {
  const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

  assert(mapped != nullptr);

  const float charW = 1.5f * scale / *frameBufferWidth;
  const float charH = 1.5f * scale / *frameBufferHeight;

  float fbW = (float)*frameBufferWidth;
  float fbH = (float)*frameBufferHeight;
  x = (x / fbW * 2.0f) - 1.0f;
  y = (y / fbH * 2.0f) - 1.0f;

  // Calculate text width
  float textWidth = 0;
  for (auto letter : text) {
    stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];
    textWidth += charData->advance * charW;
  }

  switch (align) {
  case alignRight:
    x -= textWidth;
    break;
  case alignCenter:
    x -= textWidth / 2.0f;
    break;
  case alignLeft:
    break;
  }

  // Generate a uv mapped quad per char in the new text
  for (auto letter : text) {
    stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];

    mapped->x = (x + (float)charData->x0 * charW);
    mapped->y = (y + (float)charData->y0 * charH);
    mapped->z = charData->s0;
    mapped->w = charData->t0;
    mapped++;

    mapped->x = (x + (float)charData->x1 * charW);
    mapped->y = (y + (float)charData->y0 * charH);
    mapped->z = charData->s1;
    mapped->w = charData->t0;
    mapped++;

    mapped->x = (x + (float)charData->x0 * charW);
    mapped->y = (y + (float)charData->y1 * charH);
    mapped->z = charData->s0;
    mapped->w = charData->t1;
    mapped++;

    mapped->x = (x + (float)charData->x1 * charW);
    mapped->y = (y + (float)charData->y1 * charH);
    mapped->z = charData->s1;
    mapped->w = charData->t1;
    mapped++;

    x += charData->advance * charW;

    numLetters++;
  }
}

void TextOverlay::addPlateText(std::string text, float x, float y,
                               TextAlign align) {
  const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

  assert(mapped != nullptr);

  const float charW = 1.5f * scale / *frameBufferWidth;
  const float charH = 1.5f * scale / *frameBufferHeight;

  float fbW = (float)*frameBufferWidth;
  float fbH = (float)*frameBufferHeight;
  x = (x / fbW * 2.0f);
  y = (y / fbH * 2.0f);

  // Calculate text width
  float textWidth = 0;
  for (auto letter : text) {
    stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];
    textWidth += charData->advance * charW;
  }

  switch (align) {
  case alignRight:
    x -= textWidth;
    break;
  case alignCenter:
    x -= textWidth / 2.0f;
    break;
  case alignLeft:
    break;
  }

  // Generate a uv mapped quad per char in the new text
  for (auto letter : text) {
    stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];

    mapped->x = (x + (float)charData->x0 * charW);
    mapped->y = (y + (float)charData->y0 * charH);
    mapped->z = charData->s0;
    mapped->w = charData->t0;
    mapped++;

    mapped->x = (x + (float)charData->x1 * charW);
    mapped->y = (y + (float)charData->y0 * charH);
    mapped->z = charData->s1;
    mapped->w = charData->t0;
    mapped++;

    mapped->x = (x + (float)charData->x0 * charW);
    mapped->y = (y + (float)charData->y1 * charH);
    mapped->z = charData->s0;
    mapped->w = charData->t1;
    mapped++;

    mapped->x = (x + (float)charData->x1 * charW);
    mapped->y = (y + (float)charData->y1 * charH);
    mapped->z = charData->s1;
    mapped->w = charData->t1;
    mapped++;

    x += charData->advance * charW;

    numLetters++;
  }
}

// TODO: Need update build command buffer
void TextOverlay::endTextUpdate() {
  vkUnmapMemory(vDevice->logicalDevice, memory);
  mapped = nullptr;
}

void TextOverlay::draw(VkCommandBuffer _buffer) {
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, NULL);

  // if (pushConstBlock.scale != glm::vec2(0))
  vkCmdPushConstants(_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                     sizeof(pushConstBlock), &pushConstBlock);

  VkDeviceSize offsets = 0;
  vkCmdBindVertexBuffers(_buffer, 0, 1, &buffer, &offsets);
  vkCmdBindVertexBuffers(_buffer, 1, 1, &buffer, &offsets);
  for (uint32_t j = 0; j < numLetters; j++) {
    vkCmdDraw(_buffer, 4, 1, j * 4, 0);
  }
}

void TextOverlay::initialization() {
  updateFrameSize(vDevice->uWidth(), vDevice->uHeight());
}

VkDeviceSize TextOverlay::getBufferSize() {
  return static_cast<VkDeviceSize>(sizeof(UniformBuffer2D));
}

uint32_t TextOverlay::getTexturesSize() { return textures.size(); }

VkDescriptorImageInfo *TextOverlay::get_descriptor_image(size_t tex_idx) {
  return nullptr;
}

viBuffer *TextOverlay::getBuffer() { return nullptr; }

std::vector<uint32_t> *TextOverlay::getIndices() { return 0; }

void TextOverlay::loadTexture(VkImageViewType type) {
  const uint32_t fontWidth = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
  const uint32_t fontHeight = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;

  static unsigned char font24pixels[fontWidth][fontHeight];
  stb_font_consolas_24_latin1(stbFontData, font24pixels, fontHeight);

  // Pool
  VkCommandPoolCreateInfo cmdPoolInfo = {};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.queueFamilyIndex = vDevice->queueFamilyIndices.graphics;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  VK_CHECK_RESULT(vkCreateCommandPool(vDevice->logicalDevice, &cmdPoolInfo,
                                      nullptr, &commandPool));

  // VkCommandBufferAllocateInfo cmdBufAllocateInfo =
  //   initializers::commandBufferAllocateInfo(
  //     commandPool,
  //     VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  //     (uint32_t)cmdBuffers.size());
  //
  // VK_CHECK_RESULT(vkAllocateCommandBuffers(vDevice->logicalDevice,
  // &cmdBufAllocateInfo, cmdBuffers.data()));

  // // Vertex buffer
  VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(glm::vec4);

  VkBufferCreateInfo bufferInfo = initializers::bufferCreateInfo(
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferSize);
  VK_CHECK_RESULT(
      vkCreateBuffer(vDevice->logicalDevice, &bufferInfo, nullptr, &buffer));

  VkMemoryRequirements memReqs;
  VkMemoryAllocateInfo allocInfo = initializers::memoryAllocateInfo();

  vkGetBufferMemoryRequirements(vDevice->logicalDevice, buffer, &memReqs);
  allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK_RESULT(
      vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr, &memory));
  VK_CHECK_RESULT(
      vkBindBufferMemory(vDevice->logicalDevice, buffer, memory, 0));

  // Font texture
  VkImageCreateInfo imageInfo = initializers::imageCreateInfo();
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = VK_FORMAT_R8_UNORM;
  imageInfo.extent.width = fontWidth;
  imageInfo.extent.height = fontHeight;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VK_CHECK_RESULT(
      vkCreateImage(vDevice->logicalDevice, &imageInfo, nullptr, &image));

  vkGetImageMemoryRequirements(vDevice->logicalDevice, image, &memReqs);
  allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr,
                                   &imageMemory));
  VK_CHECK_RESULT(
      vkBindImageMemory(vDevice->logicalDevice, image, imageMemory, 0));

  // Staging

  struct {
    VkDeviceMemory memory;
    VkBuffer buffer;
  } stagingBuffer;

  VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo();
  bufferCreateInfo.size = allocInfo.allocationSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VK_CHECK_RESULT(vkCreateBuffer(vDevice->logicalDevice, &bufferCreateInfo,
                                 nullptr, &stagingBuffer.buffer));

  // Get memory requirements for the staging buffer (alignment, memory type
  // bits)
  vkGetBufferMemoryRequirements(vDevice->logicalDevice, stagingBuffer.buffer,
                                &memReqs);

  allocInfo.allocationSize = memReqs.size;
  // Get memory type index for a host visible buffer
  allocInfo.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr,
                                   &stagingBuffer.memory));
  VK_CHECK_RESULT(vkBindBufferMemory(
      vDevice->logicalDevice, stagingBuffer.buffer, stagingBuffer.memory, 0));

  uint8_t *data;
  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, stagingBuffer.memory, 0,
                              allocInfo.allocationSize, 0, (void **)&data));
  // Size of the font texture is WIDTH * HEIGHT * 1 byte (only one channel)
  memcpy(data, &font24pixels[0][0], fontWidth * fontHeight);
  vkUnmapMemory(vDevice->logicalDevice, stagingBuffer.memory);
  //
  // // Copy to image
  //
  VkCommandBuffer copyCmd;

  copyCmd = vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                             commandPool, false);

  VkCommandBufferBeginInfo cmdBufInfo = initializers::commandBufferBeginInfo();
  VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));

  // Prepare for transfer
  tools::setImageLayout(copyCmd, image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  VkBufferImageCopy bufferCopyRegion = {};
  bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  bufferCopyRegion.imageSubresource.mipLevel = 0;
  bufferCopyRegion.imageSubresource.layerCount = 1;
  bufferCopyRegion.imageExtent.width = fontWidth;
  bufferCopyRegion.imageExtent.height = fontHeight;
  bufferCopyRegion.imageExtent.depth = 1;

  vkCmdCopyBufferToImage(copyCmd, stagingBuffer.buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &bufferCopyRegion);

  // Prepare for shader read
  tools::setImageLayout(copyCmd, image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

  VkSubmitInfo submitInfo = initializers::submitInfo();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &copyCmd;

  VK_CHECK_RESULT(
      vkQueueSubmit(vDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));
  VK_CHECK_RESULT(vkQueueWaitIdle(vDevice->queue));

  vkFreeCommandBuffers(vDevice->logicalDevice, commandPool, 1, &copyCmd);
  vkFreeMemory(vDevice->logicalDevice, stagingBuffer.memory, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, stagingBuffer.buffer, nullptr);

  VkImageViewCreateInfo imageViewInfo = initializers::imageViewCreateInfo();
  imageViewInfo.image = image;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewInfo.format = imageInfo.format;
  imageViewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                              VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  imageViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  VK_CHECK_RESULT(vkCreateImageView(vDevice->logicalDevice, &imageViewInfo,
                                    nullptr, &view));

  // Sampler
  VkSamplerCreateInfo samplerInfo = initializers::samplerCreateInfo();
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  VK_CHECK_RESULT(
      vkCreateSampler(vDevice->logicalDevice, &samplerInfo, nullptr, &sampler));
}

void TextOverlay::destroy() {
  // Free up all Vulkan resources requested by the text overlay
  vkDestroySampler(vDevice->logicalDevice, sampler, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, buffer, nullptr);
  vkFreeMemory(vDevice->logicalDevice, memory, nullptr);
  vkDestroyImage(vDevice->logicalDevice, image, nullptr);
  vkDestroyImageView(vDevice->logicalDevice, view, nullptr);
  vkFreeMemory(vDevice->logicalDevice, imageMemory, nullptr);
  // vkDestroyDescriptorSetLayout(vDevice->logicalDevice, descriptorSetLayout,
  // nullptr); vkDestroyDescriptorPool(vDevice->logicalDevice, descriptorPool,
  // nullptr);
  vkDestroyCommandPool(vDevice->logicalDevice, commandPool, nullptr);
}

void TextOverlay::setObjectInfo(pipeline_parameters *_parameters,
                                VkGraphicsPipelineCreateInfo *pipelineInfo) {
  _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  _parameters->colorBlendAttachment->srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  _parameters->colorBlendAttachment->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  _parameters->colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

void TextOverlay::update(float frame_time) {}

void TextOverlay::prepare() {}

void TextOverlay::setDescriptorLayout() {
  // Descriptor set layout
  std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings;
  setLayoutBindings[0] = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
      0);
  setLayoutBindings[1] = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo =
      initializers::descriptorSetLayoutCreateInfo(
          setLayoutBindings.data(),
          static_cast<uint32_t>(setLayoutBindings.size()));
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(vDevice->logicalDevice,
                                              &descriptorSetLayoutInfo, nullptr,
                                              &descriptorSetLayout));

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPushConstantRange pushConstantRange = initializers::pushConstantRange(
      VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstBlock), 0);

  // Pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo =
      initializers::pipelineLayoutCreateInfo(vkDescriptorLayouts.data(),
                                             vkDescriptorLayouts.size());

  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  pipelineLayoutInfo.pushConstantRangeCount = 1;

  VK_CHECK_RESULT(vkCreatePipelineLayout(
      vDevice->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));
}

void TextOverlay::createRenderPass(VkFormat format) {}

void TextOverlay::createDescriptorSets() {
  // Descriptor set
  VkDescriptorSetAllocateInfo descriptorSetAllocInfo =
      initializers::descriptorSetAllocateInfo(descriptorPool,
                                              &descriptorSetLayout, 1);

  VK_CHECK_RESULT(vkAllocateDescriptorSets(
      vDevice->logicalDevice, &descriptorSetAllocInfo, &descriptor));

  VkDescriptorImageInfo texDescriptor = initializers::descriptorImageInfo(
      sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  std::array<VkWriteDescriptorSet, 2> writeDescriptorSets;
  writeDescriptorSets[0] = initializers::writeDescriptorSet(
      descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &texDescriptor);
  writeDescriptorSets[1] = (initializers::createVkWriteDescriptorBuffer(
      1, &uniformObjectBuffer.descriptor, descriptor));
  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(writeDescriptorSets.size()),
                         writeDescriptorSets.data(), 0, NULL);
}

void TextOverlay::createDescriptorPool() {
  // Descriptor
  // Font uses a separate descriptor pool
  vkPoolSizes.emplace_back(initializers::descriptorPoolSize(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
  vkPoolSizes.emplace_back(
      initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
  poolDrawSize = vkPoolSizes.size();
}

void TextOverlay::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void TextOverlay::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &text_ubo, sizeof(text_ubo));
}

void TextOverlay::createAdditinalBuffer() {}

void TextOverlay::updateUniformBuffer() {}

Model3D::Model3D(const std::vector<Vertex> &_vertices,
                 const std::vector<uint32_t> &_indices, uint8_t *_texture_data)
    : Object() {
  buff_vertices = _vertices;
  buff_indices = _indices;
  texture_data = _texture_data;
}

void Model3D::initialization() { generateQuad(buff_vertices, buff_indices); }

void Model3D::generateQuad(std::vector<Vertex> _vertices,
                           std::vector<uint32_t> _indices) {
  vertices = _vertices;
  if (vertices.size() == 0) {
    // Setup vertices for a single uv-mapped quad made from two triangles
    vertices = {{{-1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{-1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{-1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{-1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, -1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},

                {{1.000000, -1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, 1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.000000, 1.000000, -1.000000},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}}

    };
  }

  indices = _indices;
  // Setup indices
  if (indices.size() == 0) {
    indices = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
               12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
               24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
  }
  indexCount = static_cast<uint32_t>(indices.size());

  // Create buffers
  // For the sake of simplicity we won't stage the vertex data to the gpu memory
  // Vertex buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &vertexBuffer, vertices.data()));
  // Index buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &indexBuffer, indices.data()));
}

void Model3D::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  vertexBuffer.destroy();
  indexBuffer.destroy();
}

void Model3D::updateTexture(uint8_t *data) {
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  int texMemSize =
      image.texture_width * image.texture_height * image.texture_byte_count;

  VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo();
  bufferCreateInfo.size = texMemSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VK_CHECK_RESULT(vkCreateBuffer(vDevice->logicalDevice, &bufferCreateInfo,
                                 nullptr, &stagingBuffer));

  VkMemoryAllocateInfo memoryAllocateInfo = initializers::memoryAllocateInfo();
  VkMemoryRequirements memoryRequirements = {};
  vkGetBufferMemoryRequirements(vDevice->logicalDevice, stagingBuffer,
                                &memoryRequirements);
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  memoryAllocateInfo.memoryTypeIndex =
      vDevice->getMemoryType(memoryRequirements.memoryTypeBits,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &memoryAllocateInfo,
                                   nullptr, &stagingMemory));
  VK_CHECK_RESULT(vkBindBufferMemory(vDevice->logicalDevice, stagingBuffer,
                                     stagingMemory, 0));

  uint8_t *mapped;

  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, stagingMemory, 0,
                              memoryRequirements.size, 0, (void **)&mapped));
  memcpy(mapped, data, texMemSize);
  vkUnmapMemory(vDevice->logicalDevice, stagingMemory);

  VkCommandBuffer copyCmd =
      vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = textures.back()->mipLevels;
  subresourceRange.layerCount = 1;

  tools::setImageLayout(copyCmd, textures.back()->textureImage,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

  std::vector<VkBufferImageCopy> bufferCopyRegions;
  for (uint32_t i = 0; i < textures.back()->mipLevels; i++) {
    VkBufferImageCopy bufferCopyRegion{};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = i;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = image.texture_width;
    bufferCopyRegion.imageExtent.height = image.texture_height;
    bufferCopyRegion.imageExtent.depth = image.texture_depth;
    bufferCopyRegion.bufferOffset = 0;

    bufferCopyRegions.push_back(bufferCopyRegion);
  }

  vkCmdCopyBufferToImage(copyCmd, stagingBuffer, textures.back()->textureImage,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         bufferCopyRegions.data());

  textures.back()->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  tools::setImageLayout(copyCmd, textures.back()->textureImage,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        textures.back()->imageLayout, subresourceRange);
  vDevice->endSingleTimeCommands(copyCmd, vDevice->queue, true);
  vkFreeMemory(vDevice->logicalDevice, stagingMemory, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, stagingBuffer, nullptr);
}

void Model3D::loadTexture(VkImageViewType type) {
  textures.clear();
  if (!textures_paths.empty()) {
    for (auto &tx_path : textures_paths) {
      // check file format on .png or ktx
      auto array = tools::split(tx_path, std::string("."));

      if (array.back() == "ktx") {
        // find array word in name
        std::size_t found =
            array.at(array.size() - 2).find(std::string("array"));
        if (found != std::string::npos) {
          auto *d_texture = new Texture2DArray();
          d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                  vDevice->queue);
          textures.emplace_back(d_texture);
        } else {
          auto *d_texture = new Texture2DKTX();
          d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                  vDevice->queue);
          textures.emplace_back(d_texture);
        }
      } else {
        textures.emplace_back(new Texture());
        textures.back()->loadTexture(tx_path, vDevice, vSwapChain,
                                     TextureType::SIMPLE, type);
      }
    }
  } else {
    textures.emplace_back(new Texture());
    textures.back()->texture_data = texture_data;
    textures.back()->prepareTexture(vDevice, vSwapChain, image.texture_width,
                                    image.texture_height, image.texture_depth,
                                    image.texture_byte_count);
  }
}

void Model3D::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void Model3D::draw(VkCommandBuffer _buffer) {
  if (!is_object_visible)
    return;
  VkDeviceSize offsets[1] = {0};

  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 0, 1, &descriptor, 0, nullptr);

  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices()->size()), 1, 0,
                   0, 0);
}

uint32_t Model3D::getTexturesSize() {
  return static_cast<uint32_t>(textures.size());
}

std::vector<uint32_t> *Model3D::getIndices() { return &indices; }

void Model3D::setDescriptorLayout() {
  // Для каждого элемента сверху что хотим передать в шейдеры задаем свое поле
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalLayoutBinding{};
  normalLayoutBinding.binding = 2;
  normalLayoutBinding.descriptorCount = 1;
  normalLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalLayoutBinding.pImmutableSamplers = nullptr;
  normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding, samplerLayoutBinding,
      //                normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Model3D::createFramebuffers(VulkanSwapChain *vkSwapChain) {
  std::vector<VkImageView> view_vector;
  // size_t attach_count = 0;
  // // Считаем общее количество attachments
  // if (attachDescriptors.size() > 0 || attachTransparents.size() > 0)
  // {
  // 	attach_count++;
  // 	createGBufferAttachments();
  // }

  // Необходимо формировать с учетом потребности в отрисовке
  vkSwapChain->swapChainFramebuffers.resize(
      vkSwapChain->swapChainImageViews.size());

  for (size_t i = 0; i < vkSwapChain->swapChainImageViews.size(); i++) {
    // view_vector.clear();
    // view_vector.emplace_back(colorImageView);
    // if (attach_count > 0)
    // {
    // 	view_vector.emplace_back(attachments.position.view);
    // 	view_vector.emplace_back(attachments.normal.view);
    // 	view_vector.emplace_back(attachments.albedo.view);
    // }
    // view_vector.emplace_back(depthImageView);
    // view_vector.emplace_back(swapChainImageViews[i]);
    std::array<VkImageView, 3> attachments = {
        vkSwapChain->colorImageView, vkSwapChain->depthImageView,
        vkSwapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vDevice->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = vkSwapChain->swapChainExtent.width;
    framebufferInfo.height = vkSwapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &vkSwapChain->swapChainFramebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Model3D::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = reference_color_vector.size();
  subpass.pColorAttachments = reference_color_vector.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Model3D::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void Model3D::setObjectInfo(pipeline_parameters *_parameters,
                            VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(5);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      vertex.inputAttributeDescription(0, 3, VertexComponent::Color),
      vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent)};

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  _parameters->colorBlendAttachment->blendEnable = VK_FALSE;
  _parameters->depthStencil->depthWriteEnable = VK_TRUE;
  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

viBuffer *Model3D::getBuffer() {
  //        viBuffer buff{};
  trn_buff.vert = vertexBuffer.buffer;
  trn_buff.ind = indexBuffer.buffer;
  return &trn_buff;
}

VkDescriptorImageInfo *Model3D::get_descriptor_image(size_t tex_idx) {
  return &textures.at(tex_idx)->descriptor;
}

void Model3D::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;
  size_t obj_size = getTexturesSize() + 1;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));

  for (size_t k = 1; k < obj_size; k++)
    descriptorWrites.emplace_back(initializers::createVkWriteDescriptorTexture(
        k, get_descriptor_image(static_cast<size_t>(k - 1)),
        get_descriptor_set()));
  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void Model3D::createDescriptorPool() {
  size_t draw_size = 0;
  draw_size += getTexturesSize();

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount =
      static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * draw_size);
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount =
      static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * draw_size * 2);
  poolDrawSize += MAX_FRAMES_IN_FLIGHT * draw_size;
}

VkDeviceSize Model3D::getBufferSize() { return bufferSize; }

void Model3D::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &model3d_ubo, sizeof(model3d_ubo));
}

void Model3D::createAdditinalBuffer() {}

void Model3D::updateUniformBuffer() {}

Model2D::Model2D(const std::vector<Vertex> &_vertices,
                 const std::vector<uint32_t> &_indices, uint8_t *_texture_data)
    : Object() {
  subpass_layout = 1;
  render_flags = ObjectRenderFlags::TRNOBJECT;

  buff_vertices = _vertices;
  buff_indices = _indices;
  texture_data = _texture_data;

  text = new TextOverlay();
  text->subpass_layout = subpass_layout;
  text->render_flags = ObjectRenderFlags::TRNOBJECT;
  text->load_object_shaders({SHADER_DIRECTORY "/plate_text.vert.spv",
                             SHADER_DIRECTORY "/plate_text.frag.spv"});
}

void Model2D::initialization() {
  generateQuad(buff_vertices, buff_indices);
  text->setEngineDepends(vDevice, vSwapChain);
  text->initialization();
}

void Model2D::modelVertexUpdate(const std::vector<Vertex> &points) {
  VkDeviceSize _size = sizeof(Vertex) * points.size();

  if (_size > buff_vertices.capacity()) {
    if (vertexBuffer.buffer) {
      vkDestroyBuffer(vDevice->logicalDevice, vertexBuffer.buffer, nullptr);
      vkFreeMemory(vDevice->logicalDevice, vertexBuffer.memory, nullptr);
    }

    VkDeviceSize _capacity = std::max(_size, buff_vertices.capacity() * 3 / 2);

    VK_CHECK_RESULT(vDevice->createBuffer(
        points.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &vertexBuffer, (void *)(points.data())));
  }

  void *data;
  vkMapMemory(vDevice->logicalDevice, vertexBuffer.memory, 0, _size, 0, &data);
  memcpy(data, points.data(), _size);
  vkUnmapMemory(vDevice->logicalDevice, vertexBuffer.memory);
  buff_vertices = points;
  // Mark update buildCommandBuffer
}

void Model2D::plateTextUpdate() {
  text->text_ubo.viewPos = model2d_ubo.viewPos;
  text->pushConstBlock.scale = pushConstBlock.scale;
  text->pushConstBlock.translate = pushConstBlock.translate;

  text->updateScale(2.0);
  text->updateFrameSize(vDevice->uWidth(), vDevice->uHeight());
  text->beginTextUpdate();
}

void Model2D::addTextToPlate(const std::string &str, float x, float y) {
  text->addPlateText(str, x, y, TextOverlay::alignLeft);
}

void Model2D::plateTextEnd() { text->endTextUpdate(); }

void Model2D::generateQuad(std::vector<Vertex> _vertices,
                           std::vector<uint32_t> _indices) {
  vertices = _vertices;
  if (vertices.size() == 0)
    // Setup vertices for a single uv-mapped quad made from two triangles
    vertices = {{{1.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{-1.0f, -1.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{1.0f, -1.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}}};

  // Setup indices
  if (indices.size() == 0)
    indices = {0, 1, 2, 2, 3, 0};
  indexCount = static_cast<uint32_t>(indices.size());

  // Create buffers
  // For the sake of simplicity we won't stage the vertex data to the gpu memory
  // Vertex buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &vertexBuffer, vertices.data()));
  // Index buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &indexBuffer, indices.data()));
}

void Model2D::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  vertexBuffer.destroy();
  indexBuffer.destroy();
  // FIXME: additional destroy for text overlay on model2
  vkDestroyPipelineCache(vDevice->logicalDevice, text->pipelineCache, nullptr);
  vkDestroyPipeline(vDevice->logicalDevice, text->pipeline, nullptr);
  vkDestroyPipelineLayout(vDevice->logicalDevice, text->pipelineLayout,
                          nullptr);
  vkDestroyRenderPass(vDevice->logicalDevice, text->renderPass, nullptr);
  text->destroyShaderModules();
  text->uniformObjectBuffer.destroy();
  vkDestroyDescriptorSetLayout(vDevice->logicalDevice,
                               text->get_descriptor_set_layout(), nullptr);
  vkDestroyDescriptorPool(vDevice->logicalDevice, text->descriptorPool,
                          nullptr);
  text->object_destroy();
}

void Model2D::updateTexture(uint8_t *data) {
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  int texMemSize =
      image.texture_width * image.texture_height * image.texture_byte_count;

  VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo();
  bufferCreateInfo.size = texMemSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VK_CHECK_RESULT(vkCreateBuffer(vDevice->logicalDevice, &bufferCreateInfo,
                                 nullptr, &stagingBuffer));

  VkMemoryAllocateInfo memoryAllocateInfo = initializers::memoryAllocateInfo();
  VkMemoryRequirements memoryRequirements = {};
  vkGetBufferMemoryRequirements(vDevice->logicalDevice, stagingBuffer,
                                &memoryRequirements);
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  memoryAllocateInfo.memoryTypeIndex =
      vDevice->getMemoryType(memoryRequirements.memoryTypeBits,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &memoryAllocateInfo,
                                   nullptr, &stagingMemory));
  VK_CHECK_RESULT(vkBindBufferMemory(vDevice->logicalDevice, stagingBuffer,
                                     stagingMemory, 0));

  uint8_t *mapped;

  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, stagingMemory, 0,
                              memoryRequirements.size, 0, (void **)&mapped));
  memcpy(mapped, data, texMemSize);
  vkUnmapMemory(vDevice->logicalDevice, stagingMemory);

  VkCommandBuffer copyCmd =
      vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = textures.back()->mipLevels;
  subresourceRange.layerCount = 1;

  tools::setImageLayout(copyCmd, textures.back()->textureImage,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

  std::vector<VkBufferImageCopy> bufferCopyRegions;
  for (uint32_t i = 0; i < textures.back()->mipLevels; i++) {
    VkBufferImageCopy bufferCopyRegion{};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = i;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = image.texture_width;
    bufferCopyRegion.imageExtent.height = image.texture_height;
    bufferCopyRegion.imageExtent.depth = image.texture_depth;
    bufferCopyRegion.bufferOffset = 0;

    bufferCopyRegions.push_back(bufferCopyRegion);
  }

  vkCmdCopyBufferToImage(copyCmd, stagingBuffer, textures.back()->textureImage,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         bufferCopyRegions.data());

  textures.back()->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  tools::setImageLayout(copyCmd, textures.back()->textureImage,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        textures.back()->imageLayout, subresourceRange);
  vDevice->endSingleTimeCommands(copyCmd, vDevice->queue, true);
  vkFreeMemory(vDevice->logicalDevice, stagingMemory, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, stagingBuffer, nullptr);
}

void Model2D::loadTexture(VkImageViewType type) {
  textures.clear();
  if (!textures_paths.empty()) {
    for (auto &tx_path : textures_paths) {
      // check file format on .png or ktx
      auto array = tools::split(tx_path, std::string("."));

      if (array.back() == "ktx") {
        // find array word in name
        std::size_t found =
            array.at(array.size() - 2).find(std::string("array"));
        if (found != std::string::npos) {
          auto *d_texture = new Texture2DArray();
          d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                  vDevice->queue);
          textures.emplace_back(d_texture);
        } else {
          auto *d_texture = new Texture2DKTX();
          d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                  vDevice->queue);
          textures.emplace_back(d_texture);
        }
      } else {
        textures.emplace_back(new Texture());
        textures.back()->loadTexture(tx_path, vDevice, vSwapChain,
                                     TextureType::SIMPLE, type);
      }
    }
  } else {
    textures.emplace_back(new Texture());
    textures.back()->texture_data = texture_data;
    textures.back()->prepareTexture(vDevice, vSwapChain, image.texture_width,
                                    image.texture_height, image.texture_depth,
                                    image.texture_byte_count);
  }
  // FIXME: Recreate for text overlay object
  text->loadTexture(VK_IMAGE_VIEW_TYPE_2D);
  text->setObjectShaders();
}

void Model2D::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
  text->preparePipeline();
}

void Model2D::draw(VkCommandBuffer _buffer) {
  VkDeviceSize offsets[1] = {0};

  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  // vkCmdPushConstants(_buffer, pipelineLayout, VK_SHADER_STAGE_GEOMETRY_BIT,
  // 1, sizeof(PushConstantTo), &push_constants);

  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);

  vkCmdPushConstants(_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                     sizeof(PushConstBlock), &pushConstBlock);
  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices()->size()), 1, 0,
                   0, 0);

  text->draw(_buffer);
}

uint32_t Model2D::getTexturesSize() {
  return static_cast<uint32_t>(textures.size());
}

std::vector<uint32_t> *Model2D::getIndices() { return &indices; }

void Model2D::setDescriptorLayout() {
  // Для каждого элемента сверху что хотим передать в шейдеры задаем свое поле
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalLayoutBinding{};
  normalLayoutBinding.binding = 2;
  normalLayoutBinding.descriptorCount = 1;
  normalLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalLayoutBinding.pImmutableSamplers = nullptr;
  normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // VkDescriptorSetLayoutBinding uboLayoutBindingGeometry{};
  // uboLayoutBinding.binding = 3;
  // uboLayoutBinding.descriptorCount = 1;
  // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  // uboLayoutBinding.pImmutableSamplers = nullptr;
  // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding, samplerLayoutBinding,
      //                normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  std::vector<VkPushConstantRange> pushConstantRange = {
      initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT,
                                      sizeof(PushConstBlock), 0),
      // initializers::pushConstantRange(VK_SHADER_STAGE_GEOMETRY_BIT,
      // sizeof(PushConstantTo), 1)
  };

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pushConstantRangeCount = pushConstantRange.size();
  pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  text->objectSetDescriptorLayout();
}

void Model2D::createFramebuffers(VulkanSwapChain *vkSwapChain) {
  std::vector<VkImageView> view_vector;
  // size_t attach_count = 0;
  // // Считаем общее количество attachments
  // if (attachDescriptors.size() > 0 || attachTransparents.size() > 0)
  // {
  // 	attach_count++;
  // 	createGBufferAttachments();
  // }

  // Необходимо формировать с учетом потребности в отрисовке
  vkSwapChain->swapChainFramebuffers.resize(
      vkSwapChain->swapChainImageViews.size());

  for (size_t i = 0; i < vkSwapChain->swapChainImageViews.size(); i++) {
    // view_vector.clear();
    // view_vector.emplace_back(colorImageView);
    // if (attach_count > 0)
    // {
    // 	view_vector.emplace_back(attachments.position.view);
    // 	view_vector.emplace_back(attachments.normal.view);
    // 	view_vector.emplace_back(attachments.albedo.view);
    // }
    // view_vector.emplace_back(depthImageView);
    // view_vector.emplace_back(swapChainImageViews[i]);
    std::array<VkImageView, 3> attachments = {
        vkSwapChain->colorImageView, vkSwapChain->depthImageView,
        vkSwapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vDevice->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = vkSwapChain->swapChainExtent.width;
    framebufferInfo.height = vkSwapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &vkSwapChain->swapChainFramebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Model2D::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = reference_color_vector.size();
  subpass.pColorAttachments = reference_color_vector.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Model2D::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();

  text->createAllBuffers();
}

void Model2D::setObjectInfo(pipeline_parameters *_parameters,
                            VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(5);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      vertex.inputAttributeDescription(0, 3, VertexComponent::Color),
      vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent)};

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  _parameters->depthStencil->depthWriteEnable = VK_TRUE;
  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  _parameters->colorBlendAttachment->srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  _parameters->colorBlendAttachment->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  _parameters->colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

viBuffer *Model2D::getBuffer() {
  //        viBuffer buff{};
  trn_buff.vert = vertexBuffer.buffer;
  trn_buff.ind = indexBuffer.buffer;
  return &trn_buff;
}

VkDescriptorImageInfo *Model2D::get_descriptor_image(size_t tex_idx) {
  return &textures.at(tex_idx)->descriptor;
}

void Model2D::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;
  size_t obj_size = getTexturesSize() + 1;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));

  for (size_t k = 1; k < obj_size; k++)
    descriptorWrites.emplace_back(initializers::createVkWriteDescriptorTexture(
        k, get_descriptor_image(static_cast<size_t>(k - 1)),
        get_descriptor_set()));
  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);

  text->objectCreateDescriptorSets();
}

void Model2D::createDescriptorPool() {
  size_t draw_size = 0;
  draw_size += getTexturesSize();

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount = 1;
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount = 1;

  poolDrawSize = vkPoolSizes.size();

  text->objectCreateDescriptorPool();
}

VkDeviceSize Model2D::getBufferSize() { return bufferSize; }

void Model2D::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &model2d_ubo, sizeof(model2d_ubo));
  text->updateMapped();
}

void Model2D::createAdditinalBuffer() {}

void Model2D::updateUniformBuffer() {}

Line::Line(std::vector<Vertex> _vertices) : Object() {
  buff_vertices = std::move(_vertices);
}

void Line::generateQuad(std::vector<Vertex> vertices) {
  if (vertices.empty())
    // Setup vertices for a single uv-mapped quad made from two triangles
    vertices = {
        {{line_thick, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, color},
        {{-line_thick, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, color},
        {{-line_thick, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, color},
        {{line_thick, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, color}};

  // Setup indices
  // indices = {0, 1, 2, 2, 3, 0};
  // indexCount = static_cast<uint32_t>(indices.size());

  // Create buffers
  // For the sake of simplicity we won't stage the vertex data to the gpu memory
  // Vertex buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &vertexBuffer, vertices.data()));
  // Index buffer
  // VK_CHECK_RESULT(vDevice->createBuffer(
  //   indices.size() * sizeof(uint32_t),
  //   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
  //   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &indexBuffer, indices.data()));
}

void Line::initialization() { generateQuad(buff_vertices); }

void Line::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  vertexBuffer.destroy();
  indexBuffer.destroy();
}

void Line::updateLinePoints(const std::vector<Vertex> &new_points) {
  VkDeviceSize _size = sizeof(Vertex) * new_points.size();

  if (_size > buff_vertices.capacity()) {
    if (vertexBuffer.buffer) {
      vkDestroyBuffer(vDevice->logicalDevice, vertexBuffer.buffer, nullptr);
      vkFreeMemory(vDevice->logicalDevice, vertexBuffer.memory, nullptr);
    }

    VkDeviceSize _capacity = std::max(_size, buff_vertices.capacity() * 3 / 2);

    VK_CHECK_RESULT(vDevice->createBuffer(
        new_points.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &vertexBuffer, (void *)(new_points.data())));
  }

  void *data;
  vkMapMemory(vDevice->logicalDevice, vertexBuffer.memory, 0, _size, 0, &data);
  memcpy(data, new_points.data(), _size);
  vkUnmapMemory(vDevice->logicalDevice, vertexBuffer.memory);
  buff_vertices = new_points;
  // Mark update buildCommandBuffer
}

void Line::loadTexture(VkImageViewType type) {}

void Line::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_TRUE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
  // Change topology on type line
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY, 0, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  // Changed polygon mode on VK_POLYGON_MODE_LINE type
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE,
          0); ///*_COUNTER*/
  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);
  multisampling.sampleShadingEnable = VK_TRUE;

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  struct MaterialSpecializationData {
    VkBool32 alphaMask;
    float alphaMaskCutoff;
  } materialSpecializationData{};
  std::vector<VkSpecializationMapEntry> specializationMapEntries;
  VkSpecializationInfo specializationInfo;

  materialSpecializationData.alphaMask = 0;
  materialSpecializationData.alphaMaskCutoff = 1;

  // POI: Constant fragment shader material parameters will be set using
  // specialization constants
  specializationMapEntries = {
      initializers::specializationMapEntry(
          0, offsetof(MaterialSpecializationData, alphaMask),
          sizeof(MaterialSpecializationData::alphaMask)),
      initializers::specializationMapEntry(
          1, offsetof(MaterialSpecializationData, alphaMaskCutoff),
          sizeof(MaterialSpecializationData::alphaMaskCutoff)),
  };
  specializationInfo = initializers::specializationInfo(
      specializationMapEntries, sizeof(materialSpecializationData),
      &materialSpecializationData);
  auto shader_fragment =
      std::find_if(shadersStages.begin(), shadersStages.end(),
                   [](VkPipelineShaderStageCreateInfo &_shader) {
                     return _shader.stage == VK_SHADER_STAGE_FRAGMENT_BIT;
                   });
  (*shader_fragment).pSpecializationInfo = &specializationInfo;

  rasterizer.lineWidth = 1.0f;

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void Line::draw(VkCommandBuffer _buffer) {
  VkDeviceSize offsets[1] = {0};

  vkCmdPushConstants(_buffer, pipelineLayout, VK_SHADER_STAGE_GEOMETRY_BIT, 0,
                     sizeof(PushConstantTo), &push_constants);
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);
  vkCmdBindVertexBuffers(_buffer, 0, 1, &vertexBuffer.buffer, offsets);
  vkCmdDraw(_buffer, buff_vertices.size(), 1, 0, 0);

  // vkCmdBindIndexBuffer(_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  // vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices()->size()), 1,
  // 0, 0, 0);
}

uint32_t Line::getTexturesSize() {
  return 1; // static_cast<uint32_t>(textures.size());
}

std::vector<uint32_t> *Line::getIndices() { return &indices; }

void Line::setDescriptorLayout() {
  // Для каждого элемента сверху что хотим передать в шейдеры задаем свое поле
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;

  // VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  // samplerLayoutBinding.binding = 1;
  // samplerLayoutBinding.descriptorCount = 1;
  // samplerLayoutBinding.descriptorType =
  // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  // samplerLayoutBinding.pImmutableSamplers = nullptr;
  // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;

  //
  // VkDescriptorSetLayoutBinding normalLayoutBinding{};
  // normalLayoutBinding.binding = 2;
  // normalLayoutBinding.descriptorCount = 1;
  // normalLayoutBinding.descriptorType =
  // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  // normalLayoutBinding.pImmutableSamplers = nullptr;
  // normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding,
      // samplerLayoutBinding,
      // normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  VkPushConstantRange pushConstantRange = initializers::pushConstantRange(
      VK_SHADER_STAGE_GEOMETRY_BIT, sizeof(PushConstantTo), 0);
  // Push constant ranges are part of the pipeline layout
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Line::createFramebuffers(VulkanSwapChain *vkSwapChain) {
  std::vector<VkImageView> view_vector;
  // size_t attach_count = 0;
  // // Считаем общее количество attachments
  // if (attachDescriptors.size() > 0 || attachTransparents.size() > 0)
  // {
  // 	attach_count++;
  // 	createGBufferAttachments();
  // }

  // Необходимо формировать с учетом потребности в отрисовке
  vkSwapChain->swapChainFramebuffers.resize(
      vkSwapChain->swapChainImageViews.size());

  for (size_t i = 0; i < vkSwapChain->swapChainImageViews.size(); i++) {
    // view_vector.clear();
    // view_vector.emplace_back(colorImageView);
    // if (attach_count > 0)
    // {
    // 	view_vector.emplace_back(attachments.position.view);
    // 	view_vector.emplace_back(attachments.normal.view);
    // 	view_vector.emplace_back(attachments.albedo.view);
    // }
    // view_vector.emplace_back(depthImageView);
    // view_vector.emplace_back(swapChainImageViews[i]);
    std::array<VkImageView, 3> attachments = {
        vkSwapChain->colorImageView, vkSwapChain->depthImageView,
        vkSwapChain->swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vDevice->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = vkSwapChain->swapChainExtent.width;
    framebufferInfo.height = vkSwapChain->swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &vkSwapChain->swapChainFramebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Line::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = reference_color_vector.size();
  subpass.pColorAttachments = reference_color_vector.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Line::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void Line::setObjectInfo(pipeline_parameters *_parameters,
                         VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(2);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Color),
  };

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  // _parameters->colorBlendAttachment->blendEnable = VK_FALSE;
  // _parameters->depthStencil->depthWriteEnable = VK_TRUE;
  // _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

viBuffer *Line::getBuffer() {
  trn_buff.vert = vertexBuffer.buffer;
  trn_buff.ind = indexBuffer.buffer;
  return &trn_buff;
}

VkDescriptorImageInfo *Line::get_descriptor_image(size_t tex_idx) {
  return new VkDescriptorImageInfo();
}

void Line::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));
  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void Line::createDescriptorPool() {
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount = 1;
  poolDrawSize = vkPoolSizes.size();
}

VkDeviceSize Line::getBufferSize() { return bufferSize; }

void Line::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &line_ubo, sizeof(line_ubo));
}

void Line::createAdditinalBuffer() {}

void Line::updateUniformBuffer() {}

TextForm::TextForm()
    : UIForm(), Object(), shift_position(), frameBufferWidth(nullptr),
      frameBufferHeight(nullptr) {
  render_flags = ObjectRenderFlags::TRNOBJECT;
  subpass_layout = 1;
}

void TextForm::init(UIForm *ptr) {
  if (ptr) {
    ptr->Add(this); /// set as child current form
  }
}

TextForm::~TextForm() = default;

void TextForm::preparePipeline() {
  // Enable blending, using alpha from red channel of the font texture (see
  // text.frag)
  VkPipelineColorBlendAttachmentState blendAttachmentState{};
  blendAttachmentState.blendEnable = VK_TRUE;
  blendAttachmentState.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  blendAttachmentState.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  blendAttachmentState.srcAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, 0, VK_FALSE);
  VkPipelineRasterizationStateCreateInfo rasterizationState =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
  VkPipelineColorBlendStateCreateInfo colorBlendState =
      initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
  VkPipelineDepthStencilStateCreateInfo depthStencilState =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
  // fixme changed for viewport
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampleState =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

  std::array<VkVertexInputBindingDescription, 2> vertexInputBindings = {
      initializers::vertexInputBindingDescription(0, sizeof(glm::vec4),
                                                  VK_VERTEX_INPUT_RATE_VERTEX),
      initializers::vertexInputBindingDescription(1, sizeof(glm::vec4),
                                                  VK_VERTEX_INPUT_RATE_VERTEX),
  };
  std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributes = {
      initializers::vertexInputAttributeDescription(0, 0,
                                                    VK_FORMAT_R32G32_SFLOAT,
                                                    0), // Location 0: Position
      initializers::vertexInputAttributeDescription(
          1, 1, VK_FORMAT_R32G32_SFLOAT,
          sizeof(glm::vec2)), // Location 1: UV
  };

  VkPipelineVertexInputStateCreateInfo vertexInputState =
      initializers::pipelineVertexInputStateCreateInfo();
  vertexInputState.vertexBindingDescriptionCount =
      static_cast<uint32_t>(vertexInputBindings.size());
  vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
  vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertexInputAttributes.size());
  vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

  VkGraphicsPipelineCreateInfo pipelineCreateInfo =
      initializers::pipelineCreateInfo(pipelineLayout, vDevice->renderPass, 0);
  pipelineCreateInfo.pVertexInputState = &vertexInputState;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
  pipelineCreateInfo.pRasterizationState = &rasterizationState;
  pipelineCreateInfo.pColorBlendState = &colorBlendState;
  pipelineCreateInfo.pMultisampleState = &multisampleState;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pDepthStencilState = &depthStencilState;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.stageCount = static_cast<uint32_t>(shadersStages.size());
  pipelineCreateInfo.pStages = shadersStages.data();
  pipelineCreateInfo.subpass = subpass_layout; // TODO: see that is it

  VK_CHECK_RESULT(
      vkCreateGraphicsPipelines(vDevice->logicalDevice, pipelineCache, 1,
                                &pipelineCreateInfo, nullptr, &pipeline));
}

void TextForm::updateScale(float nScale) { scale = nScale; }

void TextForm::updateFrameSize(uint32_t *width, uint32_t *height) {
  frameBufferHeight = height;
  frameBufferWidth = width;
}

void TextForm::beginTextUpdate() {
  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, memory, 0, VK_WHOLE_SIZE,
                              0, (void **)&mapped));
  numLetters = 0;
}

void TextForm::addText(std::string text, TextAlign align) {
  const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

  assert(mapped != nullptr);

  const float charW = 1.5f * scale / *vDevice->uWidth();
  const float charH = 1.5f * scale / *vDevice->uHeight();

  auto fbW = static_cast<float>(*vDevice->uWidth());
  auto fbH = static_cast<float>(*vDevice->uHeight());
  auto x = (static_cast<float>(getDrawXPos()) / fbW * 2.0f) - 1.0f;
  auto y = (static_cast<float>(getDrawYPos()) / fbH * 2.0f) - 1.0f;

  // Calculate text width
  float textWidth = 0;
  float maxHigh = 0;
  for (auto letter : text) {
    stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];
    textWidth += charData->advance * charW;
    auto heightChar = charData->advance * charH;
    if (heightChar > maxHigh)
      maxHigh = heightChar;
  }
  // for begin increase width with textWidth
  // them height increase charH

  auto text_block_width = static_cast<uint32_t>(textWidth * fbW / 2.f + 1);
  auto text_block_height = static_cast<uint32_t>(maxHigh * fbH / 2.0 + 1);
  setWidth(text_block_width);
  setHeight(text_block_height);
  make_update();
  ///////////////////////////////////////

  switch (align) {
  case alignRight:
    x -= textWidth;
    break;
  case alignCenter:
    x -= textWidth / 2.0f;
    break;
  case alignLeft:
    break;
  }

  // Generate a uv mapped quad per char in the new text
  for (auto letter : text) {
    stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];

    mapped->x = (x + (float)charData->x0 * charW);
    mapped->y = (y + (float)charData->y0 * charH);
    mapped->z = charData->s0;
    mapped->w = charData->t0;
    mapped++;

    mapped->x = (x + (float)charData->x1 * charW);
    mapped->y = (y + (float)charData->y0 * charH);
    mapped->z = charData->s1;
    mapped->w = charData->t0;
    mapped++;

    mapped->x = (x + (float)charData->x0 * charW);
    mapped->y = (y + (float)charData->y1 * charH);
    mapped->z = charData->s0;
    mapped->w = charData->t1;
    mapped++;

    mapped->x = (x + (float)charData->x1 * charW);
    mapped->y = (y + (float)charData->y1 * charH);
    mapped->z = charData->s1;
    mapped->w = charData->t1;
    mapped++;

    x += charData->advance * charW;

    numLetters++;
  }
}

void TextForm::endTextUpdate() {
  vkUnmapMemory(vDevice->logicalDevice, memory);
  mapped = nullptr;
}

void TextForm::draw(VkCommandBuffer _buffer) {
  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, NULL);

  // if (pushConstBlock.scale != glm::vec2(0))
  vkCmdPushConstants(_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                     sizeof(pushConstBlock), &pushConstBlock);

  VkDeviceSize offsets = 0;
  vkCmdBindVertexBuffers(_buffer, 0, 1, &buffer, &offsets);
  vkCmdBindVertexBuffers(_buffer, 1, 1, &buffer, &offsets);
  for (uint32_t j = 0; j < numLetters; j++) {
    vkCmdDraw(_buffer, 4, 1, j * 4, 0);
  }
}

void TextForm::readShaderData() {
  void *data;
  vkMapMemory(vDevice->logicalDevice, pickObjectBuffer.memory, 0,
              sizeof(uint32_t) * DEPTH_ARRAY_SCALE, 0, (void **)&data);
  auto *pickingData = static_cast<uint32_t *>(data);

  for (uint32_t i = 0; i < DEPTH_ARRAY_SCALE; ++i) {
    if (pickingData[i] != 0) {
      selectedId = pickingData[i];
      break;
    }
  }
  // TODO: FOR DEBUG
  //  if (selectedId != 0) {
  //    std::cout << "Выбран объект с ID: " << selectedId << std::endl;
  //  } else {
  //    std::cout << "Объект не выбран" << std::endl;
  //  }

  manage_constant.selected_unique_ID = selectedId;

  memset(data, 0, sizeof(uint32_t) * DEPTH_ARRAY_SCALE);
  vkUnmapMemory(vDevice->logicalDevice, pickObjectBuffer.memory);
}

void TextForm::updateUniformBuffer() {}

void TextForm::initialization() {
  updateFrameSize(vDevice->uWidth(), vDevice->uHeight());
}

VkDeviceSize TextForm::getBufferSize() {
  return static_cast<VkDeviceSize>(sizeof(UniformBuffer2D));
}

uint32_t TextForm::getTexturesSize() { return textures.size(); }

VkDescriptorImageInfo *TextForm::get_descriptor_image(size_t tex_idx) {
  return nullptr;
}

viBuffer *TextForm::getBuffer() { return nullptr; }

std::vector<uint32_t> *TextForm::getIndices() { return nullptr; }

void TextForm::loadTexture(VkImageViewType type) {
  const uint32_t fontWidth = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
  const uint32_t fontHeight = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;

  static unsigned char font24pixels[fontWidth][fontHeight];
  stb_font_consolas_24_latin1(stbFontData, font24pixels, fontHeight);

  // Pool
  VkCommandPoolCreateInfo cmdPoolInfo = {};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.queueFamilyIndex = vDevice->queueFamilyIndices.graphics;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  VK_CHECK_RESULT(vkCreateCommandPool(vDevice->logicalDevice, &cmdPoolInfo,
                                      nullptr, &commandPool));

  // VkCommandBufferAllocateInfo cmdBufAllocateInfo =
  //   initializers::commandBufferAllocateInfo(
  //     commandPool,
  //     VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  //     (uint32_t)cmdBuffers.size());
  //
  // VK_CHECK_RESULT(vkAllocateCommandBuffers(vDevice->logicalDevice,
  // &cmdBufAllocateInfo, cmdBuffers.data()));

  // // Vertex buffer
  VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(glm::vec4);

  VkBufferCreateInfo bufferInfo = initializers::bufferCreateInfo(
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferSize);
  VK_CHECK_RESULT(
      vkCreateBuffer(vDevice->logicalDevice, &bufferInfo, nullptr, &buffer));

  VkMemoryRequirements memReqs;
  VkMemoryAllocateInfo allocInfo = initializers::memoryAllocateInfo();

  vkGetBufferMemoryRequirements(vDevice->logicalDevice, buffer, &memReqs);
  allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK_RESULT(
      vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr, &memory));
  VK_CHECK_RESULT(
      vkBindBufferMemory(vDevice->logicalDevice, buffer, memory, 0));

  // Font texture
  VkImageCreateInfo imageInfo = initializers::imageCreateInfo();
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = VK_FORMAT_R8_UNORM;
  imageInfo.extent.width = fontWidth;
  imageInfo.extent.height = fontHeight;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VK_CHECK_RESULT(
      vkCreateImage(vDevice->logicalDevice, &imageInfo, nullptr, &image));

  vkGetImageMemoryRequirements(vDevice->logicalDevice, image, &memReqs);
  allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr,
                                   &imageMemory));
  VK_CHECK_RESULT(
      vkBindImageMemory(vDevice->logicalDevice, image, imageMemory, 0));

  // Staging

  struct {
    VkDeviceMemory memory;
    VkBuffer buffer;
  } stagingBuffer;

  VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo();
  bufferCreateInfo.size = allocInfo.allocationSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VK_CHECK_RESULT(vkCreateBuffer(vDevice->logicalDevice, &bufferCreateInfo,
                                 nullptr, &stagingBuffer.buffer));

  // Get memory requirements for the staging buffer (alignment, memory type
  // bits)
  vkGetBufferMemoryRequirements(vDevice->logicalDevice, stagingBuffer.buffer,
                                &memReqs);

  allocInfo.allocationSize = memReqs.size;
  // Get memory type index for a host visible buffer
  allocInfo.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr,
                                   &stagingBuffer.memory));
  VK_CHECK_RESULT(vkBindBufferMemory(
      vDevice->logicalDevice, stagingBuffer.buffer, stagingBuffer.memory, 0));

  uint8_t *data;
  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, stagingBuffer.memory, 0,
                              allocInfo.allocationSize, 0, (void **)&data));
  // Size of the font texture is WIDTH * HEIGHT * 1 byte (only one channel)
  memcpy(data, &font24pixels[0][0], fontWidth * fontHeight);
  vkUnmapMemory(vDevice->logicalDevice, stagingBuffer.memory);
  //
  // // Copy to image
  //
  VkCommandBuffer copyCmd;

  copyCmd = vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                             commandPool, false);

  VkCommandBufferBeginInfo cmdBufInfo = initializers::commandBufferBeginInfo();
  VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));

  // Prepare for transfer
  tools::setImageLayout(copyCmd, image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  VkBufferImageCopy bufferCopyRegion = {};
  bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  bufferCopyRegion.imageSubresource.mipLevel = 0;
  bufferCopyRegion.imageSubresource.layerCount = 1;
  bufferCopyRegion.imageExtent.width = fontWidth;
  bufferCopyRegion.imageExtent.height = fontHeight;
  bufferCopyRegion.imageExtent.depth = 1;

  vkCmdCopyBufferToImage(copyCmd, stagingBuffer.buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &bufferCopyRegion);

  // Prepare for shader read
  tools::setImageLayout(copyCmd, image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

  VkSubmitInfo submitInfo = initializers::submitInfo();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &copyCmd;

  VK_CHECK_RESULT(
      vkQueueSubmit(vDevice->queue, 1, &submitInfo, VK_NULL_HANDLE));
  VK_CHECK_RESULT(vkQueueWaitIdle(vDevice->queue));

  vkFreeCommandBuffers(vDevice->logicalDevice, commandPool, 1, &copyCmd);
  vkFreeMemory(vDevice->logicalDevice, stagingBuffer.memory, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, stagingBuffer.buffer, nullptr);

  VkImageViewCreateInfo imageViewInfo = initializers::imageViewCreateInfo();
  imageViewInfo.image = image;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewInfo.format = imageInfo.format;
  imageViewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                              VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  imageViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  VK_CHECK_RESULT(vkCreateImageView(vDevice->logicalDevice, &imageViewInfo,
                                    nullptr, &view));

  // Sampler
  VkSamplerCreateInfo samplerInfo = initializers::samplerCreateInfo();
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  VK_CHECK_RESULT(
      vkCreateSampler(vDevice->logicalDevice, &samplerInfo, nullptr, &sampler));
}

void TextForm::destroy() {
  // Free up all Vulkan resources requested by the text overlay
  vkDestroySampler(vDevice->logicalDevice, sampler, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, buffer, nullptr);
  vkFreeMemory(vDevice->logicalDevice, memory, nullptr);
  vkDestroyImage(vDevice->logicalDevice, image, nullptr);
  vkDestroyImageView(vDevice->logicalDevice, view, nullptr);
  vkFreeMemory(vDevice->logicalDevice, imageMemory, nullptr);
  vkDestroyCommandPool(vDevice->logicalDevice, commandPool, nullptr);
}

void TextForm::setObjectInfo(pipeline_parameters *_parameters,
                             VkGraphicsPipelineCreateInfo *pipelineInfo) {
  _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  _parameters->colorBlendAttachment->srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  _parameters->colorBlendAttachment->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  _parameters->colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

void TextForm::setDescriptorLayout() {
  // Descriptor set layout
  std::array<VkDescriptorSetLayoutBinding, 2> setLayoutBindings;
  setLayoutBindings[0] = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
      0);
  setLayoutBindings[1] = initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo =
      initializers::descriptorSetLayoutCreateInfo(
          setLayoutBindings.data(),
          static_cast<uint32_t>(setLayoutBindings.size()));
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(vDevice->logicalDevice,
                                              &descriptorSetLayoutInfo, nullptr,
                                              &descriptorSetLayout));

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPushConstantRange pushConstantRange = initializers::pushConstantRange(
      VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstBlock), 0);

  // Pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo =
      initializers::pipelineLayoutCreateInfo(vkDescriptorLayouts.data(),
                                             vkDescriptorLayouts.size());

  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  pipelineLayoutInfo.pushConstantRangeCount = 1;

  VK_CHECK_RESULT(vkCreatePipelineLayout(
      vDevice->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));
}

void TextForm::createDescriptorSets() {
  // Descriptor set
  VkDescriptorSetAllocateInfo descriptorSetAllocInfo =
      initializers::descriptorSetAllocateInfo(descriptorPool,
                                              &descriptorSetLayout, 1);

  VK_CHECK_RESULT(vkAllocateDescriptorSets(
      vDevice->logicalDevice, &descriptorSetAllocInfo, &descriptor));

  VkDescriptorImageInfo texDescriptor = initializers::descriptorImageInfo(
      sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  std::array<VkWriteDescriptorSet, 2> writeDescriptorSets;
  writeDescriptorSets[0] = initializers::writeDescriptorSet(
      descriptor, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &texDescriptor);
  writeDescriptorSets[1] = (initializers::createVkWriteDescriptorBuffer(
      1, &uniformObjectBuffer.descriptor, descriptor));
  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(writeDescriptorSets.size()),
                         writeDescriptorSets.data(), 0, NULL);
}

void TextForm::createDescriptorPool() {
  // Descriptor
  // Font uses a separate descriptor pool
  vkPoolSizes.emplace_back(initializers::descriptorPoolSize(
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
  vkPoolSizes.emplace_back(
      initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
  poolDrawSize = vkPoolSizes.size();
}

void TextForm::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void TextForm::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &text_ubo, sizeof(text_ubo));
}

void TextForm::update(float frame_time) {}

void TextForm::prepare() {}

void TextForm::createRenderPass(VkFormat format) {}

void TextForm::createAdditinalBuffer() {}

ShapeForm::ShapeForm() { setAlignRule(AlignBackGround); }

void ShapeForm::init(UIForm *ptr) {
  if (ptr) {
    ptr->Add(this); /// set as child current form
  }
}

void ShapeForm::recreate_vertices() {
  start_x = parent->getXPos();
  start_y = parent->getYPos();
  auto sh_rect = parent->getShapeRect();
  width = parent->getWidth();   // sh_rect.x;
  height = parent->getHeight(); // sh_rect.y;

  auto fbW = static_cast<float>(*vDevice->uWidth());
  auto fbH = static_cast<float>(*vDevice->uHeight());

  auto x_ = (static_cast<float>(start_x) / fbW * 2.0f) - 1.0f;
  auto y_ = (static_cast<float>(start_y) / fbH * 2.0f) - 1.0f;
  auto w_ = (static_cast<float>((sh_rect.x)) / fbW * 2.0f) - 1.0f;
  auto h_ = (static_cast<float>((sh_rect.y)) / fbH * 2.0f) - 1.0f;

  vertices = {
      {{x_, y_, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.4, 0.8, 0.6, 1}},
      {{w_, y_, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.4, 0.8, 0.6, 1}},
      {{w_, h_, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.4, 0.8, 0.6, 1}},
      {{x_, h_, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.4, 0.8, 0.6, 1}}};

  VkDeviceSize _size = sizeof(Vertex) * vertices.size();

  // if (_size > buff_vertices.capacity()) {
  //   if (vertexBuffer.buffer) {
  //     vkDestroyBuffer(vDevice->logicalDevice, vertexBuffer.buffer, nullptr);
  //     vkFreeMemory(vDevice->logicalDevice, vertexBuffer.memory, nullptr);
  //   }
  //
  //   VkDeviceSize _capacity = std::max(_size, buff_vertices.capacity() * 3 /
  //   2);
  //
  //   VK_CHECK_RESULT(vDevice->createBuffer(
  //     new_points.size() * sizeof(Vertex),
  //     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexBuffer, (void
  //     *)(new_points.data())));
  // }

  void *data;
  vkMapMemory(vDevice->logicalDevice, vertexBuffer.memory, 0, _size, 0, &data);
  memcpy(data, vertices.data(), _size);
  vkUnmapMemory(vDevice->logicalDevice, vertexBuffer.memory);
}

void ShapeForm::initialization() {
  // Generate from Pattern form if not nullptr

  if (parent != nullptr) {
    start_x = parent->getXPos();
    start_y = parent->getYPos();
    width = parent->getWidth();
    height = parent->getHeight();
  }

  if (vertices.size() == 0)
    // Setup vertices for a single uv-mapped quad made from two triangles
    vertices = {{{start_x, start_y, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{width, start_y, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{width, height, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {0.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}},
                {{start_x, height, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f},
                 {0.4, 0.8, 0.6, 1}}};

  // Setup indices
  if (indices.size() == 0)
    indices = {0, 1, 2, 2, 3, 0};
  indexCount = static_cast<uint32_t>(indices.size());

  // Create buffers
  // For the sake of simplicity we won't stage the vertex data to the gpu memory
  // Vertex buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &vertexBuffer, vertices.data()));
  // Index buffer
  VK_CHECK_RESULT(vDevice->createBuffer(
      indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &indexBuffer, indices.data()));
}

VkDeviceSize ShapeForm::getBufferSize() { return bufferSize; }

uint32_t ShapeForm::getTexturesSize() {
  return static_cast<uint32_t>(textures.size());
}

VkDescriptorImageInfo *ShapeForm::get_descriptor_image(size_t tex_idx) {
  return &textures.at(tex_idx)->descriptor;
}

viBuffer *ShapeForm::getBuffer() {
  trn_buff.vert = vertexBuffer.buffer;
  trn_buff.ind = indexBuffer.buffer;
  return &trn_buff;
}

std::vector<uint32_t> *ShapeForm::getIndices() { return &indices; }

void ShapeForm::loadTexture(VkImageViewType type) {
  textures.clear();
  if (!textures_paths.empty()) {
    for (auto &tx_path : textures_paths) {
      // check file format on .png or ktx
      auto array = tools::split(tx_path, std::string("."));

      if (array.back() == "ktx") {
        // find array word in name
        std::size_t found =
            array.at(array.size() - 2).find(std::string("array"));
        if (found != std::string::npos) {
          auto *d_texture = new Texture2DArray();
          d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                  vDevice->queue);
          textures.emplace_back(d_texture);
        } else {
          auto *d_texture = new Texture2DKTX();
          d_texture->loadFromFile(tx_path, VK_FORMAT_R8G8B8A8_UNORM, vDevice,
                                  vDevice->queue);
          textures.emplace_back(d_texture);
        }
      } else {
        textures.emplace_back(new Texture());
        textures.back()->loadTexture(tx_path, vDevice, vSwapChain,
                                     TextureType::SIMPLE, type);
      }
    }
  } else {
    textures.emplace_back(new Texture());
    textures.back()->texture_data = texture_data;
    textures.back()->prepareTexture(vDevice, vSwapChain, image.texture_width,
                                    image.texture_height, image.texture_depth,
                                    image.texture_byte_count);
  }
}

void ShapeForm::destroy() {
  for (auto &tex : textures) {
    tex->destroy();
  }
  vertexBuffer.destroy();
  indexBuffer.destroy();
}

void ShapeForm::setObjectInfo(pipeline_parameters *_parameters,
                              VkGraphicsPipelineCreateInfo *pipelineInfo) {
  vertex.vertexInputBindingDescription = vertex.inputBindingDescription(0);

  vertex.vertexInputAttributeDescriptions.resize(5);
  vertex.vertexInputAttributeDescriptions = {
      vertex.inputAttributeDescription(0, 0, VertexComponent::Position),
      vertex.inputAttributeDescription(0, 1, VertexComponent::Normal),
      vertex.inputAttributeDescription(0, 2, VertexComponent::UV),
      vertex.inputAttributeDescription(0, 3, VertexComponent::Color),
      vertex.inputAttributeDescription(0, 4, VertexComponent::Tangent)};

  _parameters->vertexInputInfo->vertexBindingDescriptionCount = 1;
  _parameters->vertexInputInfo->vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertex.vertexInputAttributeDescriptions.size());
  _parameters->vertexInputInfo->pVertexBindingDescriptions =
      &vertex.vertexInputBindingDescription;
  _parameters->vertexInputInfo->pVertexAttributeDescriptions =
      vertex.vertexInputAttributeDescriptions.data();

  _parameters->depthStencil->depthWriteEnable = VK_TRUE;
  _parameters->inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  _parameters->colorBlendAttachment->blendEnable = VK_TRUE;
  _parameters->colorBlendAttachment->srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  _parameters->colorBlendAttachment->dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  _parameters->colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  _parameters->colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  _parameters->colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;
  _parameters->colorBlendAttachment->colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  _parameters->rasterizer->cullMode = VK_CULL_MODE_NONE;
  _parameters->rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

void ShapeForm::preparePipeline() {
  pipeline_parameters _parameters;
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
  VkPipelineDepthStencilStateCreateInfo depthStencil =
      initializers::pipelineDepthStencilStateCreateInfo(
          VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      initializers::pipelineInputAssemblyStateCreateInfo(
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending =
      initializers::pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  VkPipelineRasterizationStateCreateInfo rasterizer =
      initializers::pipelineRasterizationStateCreateInfo(
          VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT,
          VK_FRONT_FACE_COUNTER_CLOCKWISE, 0); ///*_COUNTER*/
  // fixme: changed for viewport
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState =
      initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
  VkPipelineViewportStateCreateInfo viewportState =
      initializers::pipelineViewportStateCreateInfo(1, 1, 0);
  VkPipelineMultisampleStateCreateInfo multisampling =
      initializers::pipelineMultisampleStateCreateInfo(vDevice->msaaSamples, 0);

  _parameters.dynamicState = &dynamicState;
  _parameters.colorBlendAttachment = &colorBlendAttachment;
  _parameters.depthStencil = &depthStencil;
  _parameters.inputAssembly = &inputAssembly;
  _parameters.viewportState = &viewportState;
  _parameters.rasterizer = &rasterizer;
  _parameters.multisampling = &multisampling;
  _parameters.colorBlending = &colorBlending;
  _parameters.vertexInputInfo = &vertexInputInfo;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.flags = 0;
  pipelineInfo.renderPass = vDevice->renderPass;
  pipelineInfo.subpass = subpass_layout; // TODO: see that is it

  pipelineInfo.layout = pipelineLayout;

  setObjectInfo(&_parameters, &pipelineInfo);

  pipelineInfo.pStages = shadersStages.data();
  pipelineInfo.stageCount = shadersStages.size();

  if (vkCreateGraphicsPipelines(vDevice->logicalDevice, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void ShapeForm::update(float frame_time) {}

void ShapeForm::prepare() {}

void ShapeForm::draw(VkCommandBuffer _buffer) {
  VkDeviceSize offsets[1] = {0};

  vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout, 1, 1, &descriptor, 0, nullptr);

  VkBuffer *vertexBuffers = &getBuffer()->vert;
  vkCmdBindVertexBuffers(_buffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(_buffer, getBuffer()->ind, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(_buffer, static_cast<uint32_t>(getIndices()->size()), 1, 0,
                   0, 0);
}

void ShapeForm::setDescriptorLayout() {
  // Для каждого элемента сверху что хотим передать в шейдеры задаем свое поле
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalLayoutBinding{};
  normalLayoutBinding.binding = 2;
  normalLayoutBinding.descriptorCount = 1;
  normalLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalLayoutBinding.pImmutableSamplers = nullptr;
  normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // VkDescriptorSetLayoutBinding uboLayoutBindingGeometry{};
  // uboLayoutBinding.binding = 3;
  // uboLayoutBinding.descriptorCount = 1;
  // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  // uboLayoutBinding.pImmutableSamplers = nullptr;
  // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings = {
      uboLayoutBinding, samplerLayoutBinding,
      //                normalLayoutBinding,
  };
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  vkDescriptorLayouts.emplace_back(descriptorSetLayout);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = vkDescriptorLayouts.size();
  pipelineLayoutInfo.pSetLayouts = vkDescriptorLayouts.data();

  if (vkCreatePipelineLayout(vDevice->logicalDevice, &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void ShapeForm::createFramebuffers(VulkanSwapChain *vkSwapChain) {}

void ShapeForm::createRenderPass(VkFormat format) {
  imageFormat = format;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = vDevice->findDepthFormat();
  depthAttachment.samples = vDevice->msaaSamples;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = imageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = reference_color_vector.size();
  subpass.pColorAttachments = reference_color_vector.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachment, depthAttachment, colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(vDevice->logicalDevice, &renderPassInfo, nullptr,
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void ShapeForm::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo,
                               &descriptor) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  std::vector<VkWriteDescriptorSet> descriptorWrites;
  size_t obj_size = getTexturesSize() + 1;

  descriptorWrites.emplace_back(initializers::createVkWriteDescriptorBuffer(
      0, &uniformObjectBuffer.descriptor, descriptor));

  for (size_t k = 1; k < obj_size; k++)
    descriptorWrites.emplace_back(initializers::createVkWriteDescriptorTexture(
        k, get_descriptor_image(static_cast<size_t>(k - 1)),
        get_descriptor_set()));
  vkUpdateDescriptorSets(vDevice->logicalDevice,
                         static_cast<uint32_t>(descriptorWrites.size()),
                         descriptorWrites.data(), 0, nullptr);
}

void ShapeForm::createDescriptorPool() {
  size_t draw_size = 0;
  draw_size += getTexturesSize();

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  vkPoolSizes.back().descriptorCount = 1;
  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  vkPoolSizes.back().descriptorCount = 1;

  poolDrawSize = vkPoolSizes.size();
}

void ShapeForm::createUniformBuffer() {
  vDevice->createBuffer(getBufferSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &uniformObjectBuffer);
  uniformObjectBuffer.map();
}

void ShapeForm::updateMapped() {
  memcpy(uniformObjectBuffer.mapped, &model2d_ubo, sizeof(model2d_ubo));
}

void ShapeForm::createAdditinalBuffer() {}

void ShapeForm::updateUniformBuffer() {}

void ShapeForm::updateTexture(uint8_t *data) {
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  int texMemSize =
      image.texture_width * image.texture_height * image.texture_byte_count;

  VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo();
  bufferCreateInfo.size = texMemSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VK_CHECK_RESULT(vkCreateBuffer(vDevice->logicalDevice, &bufferCreateInfo,
                                 nullptr, &stagingBuffer));

  VkMemoryAllocateInfo memoryAllocateInfo = initializers::memoryAllocateInfo();
  VkMemoryRequirements memoryRequirements = {};
  vkGetBufferMemoryRequirements(vDevice->logicalDevice, stagingBuffer,
                                &memoryRequirements);
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  memoryAllocateInfo.memoryTypeIndex =
      vDevice->getMemoryType(memoryRequirements.memoryTypeBits,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &memoryAllocateInfo,
                                   nullptr, &stagingMemory));
  VK_CHECK_RESULT(vkBindBufferMemory(vDevice->logicalDevice, stagingBuffer,
                                     stagingMemory, 0));

  uint8_t *mapped;

  VK_CHECK_RESULT(vkMapMemory(vDevice->logicalDevice, stagingMemory, 0,
                              memoryRequirements.size, 0, (void **)&mapped));
  memcpy(mapped, data, texMemSize);
  vkUnmapMemory(vDevice->logicalDevice, stagingMemory);

  VkCommandBuffer copyCmd =
      vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = textures.back()->mipLevels;
  subresourceRange.layerCount = 1;

  tools::setImageLayout(copyCmd, textures.back()->textureImage,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

  std::vector<VkBufferImageCopy> bufferCopyRegions;
  for (uint32_t i = 0; i < textures.back()->mipLevels; i++) {
    VkBufferImageCopy bufferCopyRegion{};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = i;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = image.texture_width;
    bufferCopyRegion.imageExtent.height = image.texture_height;
    bufferCopyRegion.imageExtent.depth = image.texture_depth;
    bufferCopyRegion.bufferOffset = 0;

    bufferCopyRegions.push_back(bufferCopyRegion);
  }

  vkCmdCopyBufferToImage(copyCmd, stagingBuffer, textures.back()->textureImage,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         bufferCopyRegions.data());

  textures.back()->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  tools::setImageLayout(copyCmd, textures.back()->textureImage,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        textures.back()->imageLayout, subresourceRange);
  vDevice->endSingleTimeCommands(copyCmd, vDevice->queue, true);
  vkFreeMemory(vDevice->logicalDevice, stagingMemory, nullptr);
  vkDestroyBuffer(vDevice->logicalDevice, stagingBuffer, nullptr);
}
