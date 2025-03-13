//
// Created by f_f on 03.12.24.
//

#ifndef PK_DISPLAY_ANIMODEL_IMPL_H
#define PK_DISPLAY_ANIMODEL_IMPL_H

#include <utility>
#include "animodel.hpp"

class GLTF_Model_Animate::AnimGLTF_Model_Impl {
public:
  AnimGLTF_Model_Impl() = default;

  ~AnimGLTF_Model_Impl() = default;

  void initialize(VulkanDevice *_device) {
    this->device = _device;
    glTFModel = new VulkanglTFModelAnimate();
  }
  void loadglTFFile(const std::string &filename) const {
    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;


#if defined(__ANDROID__)
    // On Android all assets are packed with the apk in a compressed form, so we need to open them using the asset manager
// We let tinygltf handle this, by passing the asset manager of our app
tinygltf::asset_manager = androidApp->activity->assetManager;
#endif
    bool binary = false;
    size_t extpos = filename.rfind('.', filename.length());
    if (extpos != std::string::npos) {
      binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
    }

    size_t _pos = filename.find_last_of('/');
    if (_pos == std::string::npos) {
      _pos = filename.find_last_of('\\');
    }
    auto filePath = filename.substr(0, _pos);

//        gltfContext.SetImageLoader(loadImageDataFunc, nullptr);

    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&glTFInput, &error, &warning, filename)
                             : gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);


    // Pass some Vulkan resources required for setup and rendering to the glTF model loading class
    glTFModel->vulkanDevice = device;
    glTFModel->copyQueue = device->queue;

    std::vector<uint32_t> indexBuffer;
    std::vector<VulkanglTFModelAnimate::Vertex> vertexBuffer;

    if (fileLoaded) {
      glTFModel->loadImages(glTFInput);
      glTFModel->loadMaterials(glTFInput);
      glTFModel->loadTextures(glTFInput);
      const tinygltf::Scene &scene = glTFInput.scenes[0];
      for (size_t i = 0; i < scene.nodes.size(); i++) {
        const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
        glTFModel->loadNode(node, glTFInput, nullptr, scene.nodes[i], indexBuffer, vertexBuffer);
      }
      glTFModel->loadSkins(glTFInput);
      glTFModel->loadAnimations(glTFInput);
      // Calculate initial pose
      for (auto node: glTFModel->nodes) {
        glTFModel->updateJoints(node);
      }
    } else {
      tools::exitFatal("Could not open the glTF file.\n", -1);
      return;
    }

    // Create and upload vertex and index buffer
    size_t vertexBufferSize = vertexBuffer.size() * sizeof(VulkanglTFModelAnimate::Vertex);
    size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
    glTFModel->indices.count = static_cast<uint32_t>(indexBuffer.size());

    struct StagingBuffer {
      VkBuffer buffer;
      VkDeviceMemory memory;
    } vertexStaging, indexStaging;

    // Create host visible staging buffers (source)
    VK_CHECK_RESULT(device->createBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &vertexStaging.buffer,
        vertexBufferSize,
        &vertexStaging.memory,
        vertexBuffer.data()));
    // Index data
    VK_CHECK_RESULT(device->createBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &indexStaging.buffer,
        indexBufferSize,
        &indexStaging.memory,
        indexBuffer.data()));

    // Create device local buffers (target)
    VK_CHECK_RESULT(device->createBuffer(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &glTFModel->vertices.buffer,
        vertexBufferSize,
        &glTFModel->vertices.memory));
    VK_CHECK_RESULT(device->createBuffer(
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &glTFModel->indices.buffer,
        indexBufferSize,
        &glTFModel->indices.memory));

    // Copy data from staging buffers (host) do device local buffer (gpu)
    VkCommandBuffer copyCmd = device->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, glTFModel->vertices.buffer, 1, &copyRegion);
    copyRegion.size = indexBufferSize;
    vkCmdCopyBuffer(copyCmd, indexStaging.buffer, glTFModel->indices.buffer, 1, &copyRegion);
    device->endSingleTimeCommands(copyCmd, device->queue, true);

    // Free staging resources
    vkDestroyBuffer(device->logicalDevice, vertexStaging.buffer, nullptr);
    vkFreeMemory(device->logicalDevice, vertexStaging.memory, nullptr);
    vkDestroyBuffer(device->logicalDevice, indexStaging.buffer, nullptr);
    vkFreeMemory(device->logicalDevice, indexStaging.memory, nullptr);
  }

  VulkanglTFModelAnimate *getGLTFModel() const {
    return glTFModel;
  }

  void destroy() const {
    delete glTFModel;
  }

private:
  VulkanDevice *device = nullptr;
  VulkanglTFModelAnimate* glTFModel;
};

#endif //PK_DISPLAY_ANIMODEL_IMPL_H
