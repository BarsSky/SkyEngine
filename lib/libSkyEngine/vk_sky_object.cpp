//
// Created by ubuntu on 31.07.24.
//
#include <SkyEngine/vk_sky_object.hpp>
#include <SkyEngine/vk_sky_device.hpp>
#include "tools.hpp"

Object::Object() {
  u_ptr_compute = std::make_unique<ComputeInst>();
}

void Object::setObjectShaders() {
  for (auto &path: shaders_paths) {
    VkShaderStageFlagBits flag = VK_SHADER_STAGE_VERTEX_BIT;
    auto array = tools::split(path, std::string("."));
    if (array.at(array.size() - 2) == "vert")
      flag = VK_SHADER_STAGE_VERTEX_BIT;
    if (array.at(array.size() - 2) == "frag")
      flag = VK_SHADER_STAGE_FRAGMENT_BIT;
    if (array.at(array.size() - 2) == "tesc")
      flag = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if (array.at(array.size() - 2) == "tese")
      flag = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    if (array.at(array.size() - 2) == "comp")
      flag = VK_SHADER_STAGE_COMPUTE_BIT;
    if (array.at(array.size() - 2) == "geom")
      flag = VK_SHADER_STAGE_GEOMETRY_BIT;

    shadersStages.push_back(LoadShader(path, flag));
  }
}

void Object::load_object_shaders(std::vector<std::string> paths) {
  shaders_paths = std::move(paths);
}

std::vector<VkPipelineShaderStageCreateInfo> Object::getShaderStages() const {
  return shadersStages;
}

void Object::setEngineDepends(VulkanDevice *device, VulkanSwapChain *swapChain) {
  vDevice = device;
  vSwapChain = swapChain;
  u_ptr_compute->set_device(vDevice);
}

void Object::destroyShaderModules() {
  for (auto &module: shaderModules)
    vkDestroyShaderModule(vDevice->logicalDevice, module, nullptr);
}

std::vector<VkShaderModule> Object::getShaderModules() const {
  return shaderModules;
}

VkPipelineLayout Object::get_pipeline_layout() {
  return pipelineLayout;
}

VkDescriptorSetLayout Object::get_descriptor_set_layout() {
  return descriptorSetLayout;
}

VkDescriptorSet Object::get_descriptor_set() {
  return descriptor;
}

Object::~Object() {
  for (auto &texture: textures)
    delete texture;
}

void Object::cleanObjectSwapChain() {
  vkDestroyImageView(vDevice->logicalDevice, depthImageView, nullptr);
  vkDestroyImage(vDevice->logicalDevice, depthImage, nullptr);
  vkFreeMemory(vDevice->logicalDevice, depthImageMemory, nullptr);

  vkDestroyImageView(vDevice->logicalDevice, colorImageView, nullptr);
  vkDestroyImage(vDevice->logicalDevice, colorImage, nullptr);
  vkFreeMemory(vDevice->logicalDevice, colorImageMemory, nullptr);

  for (size_t i = 0; i < frameBuffer.size(); i++) {
    vkDestroyFramebuffer(vDevice->logicalDevice, frameBuffer.at(i), nullptr);
  }
}

void Object::load_textures_paths(std::vector<std::string> paths) {
  textures_paths = std::move(paths);
}

void Object::object_destroy() {
  clearUniqueBuffers();
  destroy();
}

glm::vec3 Object::position() {
  return {*x, *y, *z};
}

glm::vec3 Object::rotation_axis() {
  return {*omega_x, *omega_y, *omega_z};
}

float Object::rotation_speed() {
  return *omega_w;
}

void Object::set_rotation_angle(float *angle) {
  omega_w = angle;
}

void Object::set_track(float *x, float *y, float *z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

void Object::set_rotate_axis(float *x, float *y, float *z) {
  omega_x = x;
  omega_y = y;
  omega_z = z;
}

void Object::set_rotate_track(float *x, float *y, float *z, float *w) {
  this->omega_x = x;
  this->omega_y = y;
  this->omega_z = z;
  this->omega_w = w;
}

VkPipelineShaderStageCreateInfo Object::LoadShader(const std::string &filename, VkShaderStageFlagBits stage) {
  //
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> code(fileSize);

  file.seekg(0);
  file.read(code.data(), fileSize);

  file.close();
  //
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(vDevice->logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = stage;
  vertShaderStageInfo.module = shaderModule;
  vertShaderStageInfo.pName = "main";

  shaderModules.emplace_back(shaderModule);

  return vertShaderStageInfo;
}

void Object::createUniqueBuffers() {
  VkDeviceSize bufferSize = sizeof(uint32_t) * DEPTH_ARRAY_SCALE;
  vDevice->createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        &pickObjectBuffer);
  pickObjectBuffer.map(bufferSize);

  memset(pickObjectBuffer.mapped, 0, (size_t) bufferSize);
  pickObjectBuffer.unmap();
}

void Object::clearUniqueBuffers() {
  pickObjectBuffer.destroy();
  vkDestroyDescriptorSetLayout(vDevice->logicalDevice,pickDescriptorSetLayout,nullptr);
}

void Object::drawObjectBase(VkCommandBuffer _buffer) {

  /// Set SSBO buffer what placed in set = 0 and binding 0
  vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &pickDescriptor,
                          0, nullptr);
}

void Object::createObjectBaseDescriptor() {

  VkDescriptorSetAllocateInfo allocInfo =
      initializers::descriptorSetAllocateInfo(
        descriptorPool,
        &pickDescriptorSetLayout,
        1);

  // ATTENTION: For each layer of set need unique descriptor to set
  VK_CHECK_RESULT(vkAllocateDescriptorSets(vDevice->logicalDevice, &allocInfo, &pickDescriptor));

  std::vector<VkWriteDescriptorSet> pickWriteDescriptorSets =
  {
    // Binding 0 : Particle position storage buffer
    initializers::writeDescriptorSet(
      pickDescriptor,
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      0,
      &pickObjectBuffer.descriptor)
  };

  vkUpdateDescriptorSets(vDevice->logicalDevice, static_cast<uint32_t>(pickWriteDescriptorSets.size()),
                         pickWriteDescriptorSets.data(), 0, nullptr);
}

void Object::createObjectBasePool() {

  // Make increase pool in scene

  vkPoolSizes.emplace_back();
  vkPoolSizes.back().type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  vkPoolSizes.back().descriptorCount = 1;
  poolDrawSize = vkPoolSizes.size();
}

void Object::allocateDescriptorPool() {
  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(vkPoolSizes.size());
  poolInfo.pPoolSizes = vkPoolSizes.data();
  poolInfo.maxSets = poolDrawSize;

  if (vkCreateDescriptorPool(vDevice->logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void Object::setObjectBaseLayout() {

  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
    // Binding 0 : Particle position storage buffer
    initializers::descriptorSetLayoutBinding(
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      VK_SHADER_STAGE_FRAGMENT_BIT,
      0)
  };

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  layoutInfo.pBindings = setLayoutBindings.data();
  //
  if (vkCreateDescriptorSetLayout(vDevice->logicalDevice, &layoutInfo, nullptr, &pickDescriptorSetLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
      }

  vkDescriptorLayouts.emplace_back(pickDescriptorSetLayout);
}

void Object::createPipelineCache() {
  VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
  pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  if (vkCreatePipelineCache(vDevice->logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache)) {
    throw std::runtime_error("failed too create pipeline cache");
  }
}

void Object::clearComputeBlock() {
}

VkCommandBuffer *Object::getComputeBuffer() const {
  return &u_ptr_compute->commandBuffer;
}

VkQueue Object::getComputeQueue() const {
  return u_ptr_compute->queue;
}

VkSemaphore *Object::getGraphicSemaphore() const {
  return &u_ptr_compute->graphic;
}

void Object::readShaderData() {
}

void Object::setVisibleProperty(bool flag) {
  is_object_visible = flag;
}

VkSemaphore *Object::getComputeSemaphore() {
  return &u_ptr_compute->compute;
}
