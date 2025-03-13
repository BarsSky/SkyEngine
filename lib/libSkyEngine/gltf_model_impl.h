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

  void initialization() {
    scene = new vkglTF::Model();
    renderFlags = vkglTF::RenderFlags::BindImages;
  }

  void scene_load(std::string filename, VulkanDevice *device, VkQueue transferQueue) {
    const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices |
                                      vkglTF::FileLoadingFlags::PreMultiplyVertexColors |
                                      vkglTF::FileLoadingFlags::FlipY;
    scene->loadFromFile(std::move(filename), device, transferQueue, glTFLoadingFlags);
  }

  uint32_t get_material_size() const {
    return static_cast<uint32_t>(scene->materials.size());
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

  void
  draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout = VK_NULL_HANDLE, uint32_t bindImageSet = 1) {
    scene->draw(commandBuffer, 0, pipelineLayout, bindImageSet);
  }

  VkDescriptorImageInfo *descriptor(size_t tex_id) {
    return &scene->textures.at(tex_id).descriptor;
  }

  size_t texture_size() {
    return scene->textures.size();
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

  void destroy() {
    delete scene;
  }

private:
  vkglTF::Model *scene{};
  uint32_t renderFlags{};
};

#endif //PK_DISPLAY_GLTF_MODEL_IMPL_H
