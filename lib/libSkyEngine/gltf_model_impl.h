//
// Created by ubuntu on 31.07.24.
//

#ifndef PK_DISPLAY_GLTF_MODEL_IMPL_H
#define PK_DISPLAY_GLTF_MODEL_IMPL_H

#include <utility>
#include "gltfmodel.hpp"

class GLTF_Model::GLTF_CImpl {
public:
  GLTF_CImpl() = default;

  ~GLTF_CImpl() = default;

  void destroy() {
    delete scene;
  }

  void initialization() {
    scene = new vkglTF::Model();
    renderFlags = vkglTF::RenderFlags::BindImages;
  }

  void increase_ubo() {
    scene->add_some_ubo();
  }

  void scene_load(std::string filename, VulkanDevice *device, VkQueue transferQueue,
    const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices |
                                      vkglTF::FileLoadingFlags::PreMultiplyVertexColors |
                                      vkglTF::FileLoadingFlags::FlipY) {
    scene->loadFromFile(std::move(filename), device, transferQueue, glTFLoadingFlags);
  }

  uint32_t get_material_size() const {
    ///FIXME:Исправить -1 так как может
    /// потребоваться для без текстурных объектов
    return static_cast<uint32_t>(scene->materials.size() - 1);
  }

  VkBool32 get_material_alpha(size_t index) const {
    return scene->materials.at(index).alphaMode == vkglTF::Material::ALPHAMODE_MASK;
  }

  float get_material_alphacutoff(size_t index) const {
    return scene->materials.at(index).alphaCutoff;
  }

  uint32_t get_textures_size() const {
    return static_cast<uint32_t>(scene->textures.size());
  }


  uint32_t get_skin_size() const {
    return static_cast<uint32_t>(scene->skins.size());
  }

  size_t get_skins(size_t num) {
    return scene->skins.at(num)->inverseBindMatrices.size();
  }

  glm::mat<4, 4, float> *get_skins_data(size_t num) {
    return scene->skins.at(num)->inverseBindMatrices.data();
  }

  VkPipelineVertexInputStateCreateInfo *getPipelineVertexInputState() {
    return vkglTF::Vertex::getPipelineVertexInputState(
      {
        vkglTF::VertexComponent::Position,
        vkglTF::VertexComponent::Normal,
        vkglTF::VertexComponent::UV,
        vkglTF::VertexComponent::Color,
        vkglTF::VertexComponent::Tangent
      }
    );
  }

  void enable_normalMap() {
    vkglTF::descriptorBindingFlags = vkglTF::DescriptorBindingFlags::ImageBaseColor|vkglTF::DescriptorBindingFlags::ImageNormalMap;
  }

  void draw(VkCommandBuffer commandBuffer, uint32_t renderflags = 0,VkPipelineLayout pipelineLayout = VK_NULL_HANDLE,
            uint32_t bindImageSet = 1) {
    scene->draw(commandBuffer, renderflags, pipelineLayout, bindImageSet);
  }

  VkDescriptorImageInfo *descriptor(size_t tex_id) {
    return &scene->textures.at(tex_id).descriptor;
  }

  void set_visible(size_t index, bool flag) {
    scene->nodes.at(index)->visible = flag;
  }

  VkDescriptorPool get_pool_descriptor() {
    return scene->descriptorPool;
  }

  VkPipeline* get_material_pipeline(size_t index) {
    return &scene->materials.at(index).pipeline;
  }

  bool get_doublesided(size_t index) {
    return scene->materials.at(index).doubleSided;
  }

  size_t get_vertex_size() {
    return scene->vertices.count;
  }

  std::vector<uint32_t> getIndexBuffer() const {
    return scene->getIndexBuffer();
  }

  std::vector<glm::vec2> getVertexBuffer() const {
    return scene->getVertexBuffer();
  }

  VkBuffer* get_vertex_buffer() {
    return &scene->vertices.buffer;
  }

  VkDeviceMemory* get_vertex_memory() {
    return &scene->vertices.memory;
  }

  void updateUbo(void* data, size_t mesh_id) const {
    auto ct_data = static_cast<vkglTF::Mesh::UniformBufferObject*>(data);
    if (scene->linearNodes.at(mesh_id)->mesh) {
      scene->linearNodes.at(mesh_id)->mesh->uniformBase.unique_id = ct_data->unique_id;
      scene->linearNodes.at(mesh_id)->mesh->uniformBase.matrix = ct_data->model;
      scene->linearNodes.at(mesh_id)->mesh->uniformBase.proj = ct_data->proj;
      scene->linearNodes.at(mesh_id)->mesh->uniformBase.view = ct_data->view;
      scene->linearNodes.at(mesh_id)->mesh->uniformBase.viewPos = ct_data->viewPos;
      scene->linearNodes.at(mesh_id)->mesh->uniformBase.lightPositon = ct_data->lightPositon;
      scene->linearNodes.at(mesh_id)->update();
    }
  }

  VkDescriptorImageInfo *get_colorMap_descriptor(size_t tex_id) {
    return &scene->materials.at(tex_id).baseColorTexture->descriptor;
  }

  VkDescriptorImageInfo *get_normalMap_descriptor(size_t tex_id) {
    return &scene->materials.at(tex_id).normalTexture->descriptor;
  }

  size_t texture_size() {
    return scene->textures.size();
  }

  size_t get_node_size() {
    return scene->nodes.size();
  }

  size_t get_linearNode_size() {
    return scene->linearNodes.size();
  }

  size_t get_binding_count() {
    size_t binding_count = 0;
    if (!scene->linearNodes.empty())
      if (!scene->linearNodes.at(0)->mesh->primitives.empty()) {
        if (scene->linearNodes.at(0)->mesh->primitives.at(0)->material.baseColorTexture != nullptr)
          ++binding_count;
        if (scene->linearNodes.at(0)->mesh->primitives.at(0)->material.normalTexture != nullptr)
          ++binding_count;
      }
    return binding_count;
  }

  bool check_baseColor_texture(size_t tex_id) const {
    return scene->materials.at(tex_id).baseColorTexture != nullptr;
  }

  bool check_normal_texture(size_t tex_id) const {
    return scene->materials.at(tex_id).normalTexture != nullptr;
  }

  VkDescriptorSet get_material_descriptor(size_t num) {
    return scene->materials.at(num).descriptorSet;
  }

  VkDescriptorSet *get_material_desc_ptr(size_t num) {
    return &scene->materials.at(num).descriptorSet;
  }

  VkDescriptorBufferInfo *get_mesh_descriptor(size_t num) {
    return &scene->linearNodes.at(19)->mesh->uniformBuffer.descriptor;
  }

  VkDescriptorSet get_mesh_descriptor_set(size_t num) {
    return scene->linearNodes.at(19)->mesh->uniformBuffer.descriptorSet;
  }

  VkDescriptorSet *get_mesh_desc_ptr(size_t num) {
    return &scene->linearNodes.at(19)->mesh->uniformBuffer.descriptorSet;
  }

private:
  vkglTF::Model *scene{};
  uint32_t renderFlags{};
};

#endif //PK_DISPLAY_GLTF_MODEL_IMPL_H
