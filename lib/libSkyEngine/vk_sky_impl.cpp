//
// Created by ubuntu on 31.07.24.
//
#include <array>
#include <SkyEngine/config/config.h>
#include "vk_sky_impl.h"
#include "vk_sky_pipelineobject.hpp"

using namespace vk_sky;

void VKSky::CImpl::updateBufferMapped()
{
  for (auto &obj : all_objects)
    obj->updateMapped();
}

Screen VKSky::CImpl::getScreen()
{
  Screen _op;
  _op.width = vDevice.Width();
  _op.height = vDevice.Height();
  _op.uWidth = vDevice.uWidth();
  _op.uHeight = vDevice.uHeight();
#ifdef GLFW_LIB_ENABLE
  _op.window = vDevice.get_glfw_window_ptr();
#endif
  return _op;
}

#ifdef GLFW_LIB_ENABLE
void VKSky::CImpl::switchFullScreen()
{
  vDevice.switchFullScreen();
}
#endif

void VKSky::CImpl::endLoop() const
{
  vkDeviceWaitIdle(vDevice.logicalDevice);
}

void VKSky::CImpl::impl_flush() const
{
#ifdef __linux__
  xcb_flush(vDevice.get_connection());
#endif
}

void VKSky::CImpl::version_init(toolConfig *app, toolConfig *eng)
{
  appConfig = app;
  engConfig = eng;
}

#ifdef QT_LIB_ENABLE
VkWidget *VKSky::CImpl::get_vk_widget(QWidget *parent)
{
  vulkan_widget = vDevice.init_widget(parent);
  return vulkan_widget;
}
#endif

void VKSky::CImpl::set_camera_ptr(ObjCamera *aCamera)
{
  camera = aCamera;
}

void VKSky::CImpl::addObjectsToThread()
{
  for (auto &all_object : all_objects)
    addObjectToThread(all_object);
}

void VKSky::CImpl::addObjectToThread(Object *object)
{
  /*
         Add thread type
     */
  std::unique_lock<std::mutex> lk(mute);
  if ((threadObjects.empty()) || (threadObjects.size() <= numThreads))
  {
    threadObjects.emplace_back(&vDevice.logicalDevice);
    threadObjects.back().objectsInThread.push_back(object);
    threadObjects.back().numOfObjectsInThread++;
    loadOnThread++;
  }
  else
  {
    threadObjects.at(threadObjectCount).objectsInThread.push_back(object);
    threadObjects.at(threadObjectCount).numOfObjectsInThread++;
    threadObjectCount++;
    if (threadObjectCount >= threadObjects.size())
      threadObjectCount = 0;
  }

  threadObjects.back().configure_buffer(vDevice.logicalDevice, VkSwapChain.queueNodeIndex, VkSwapChain.imageCount);
}

void VKSky::CImpl::clearThreadObject(uint32_t begin)
{
  std::unique_lock<std::mutex> lk(mute);
  for (auto &thread : threadObjects)
  {
    auto iter = std::find(thread.objectsInThread.begin(), thread.objectsInThread.end(), all_objects.at(begin));
    if (thread.objectsInThread.end() != iter)
    {
      thread.objectsInThread.erase(iter);
      thread.numOfObjectsInThread--;
      break;
    }
  }
}

Object *VKSky::CImpl::createObject(const pipelineObject &pipeline, vk_sky::ObjectFlags flag)
{
  switch (flag)
  {
  case ObjectFlags::BASE_LOAD:
    all_objects.emplace_back(reinterpret_cast<Object *>(pipeline.object));
    break;
  case ObjectFlags::LOAD_IN_THREAD:
    all_objects.emplace_back(reinterpret_cast<Object *>(pipeline.object));
    addObjectToThread(reinterpret_cast<Object *>(pipeline.object));
    break;
  default:
    all_objects.emplace_back(reinterpret_cast<Object *>(pipeline.object));
    break;
  }
  // буфер для хранения объектов
  draw_objects.emplace_back(pipeline);
  if (reinterpret_cast<Object *>(pipeline.object)->render_flags == ObjectRenderFlags::STDOBJECT || reinterpret_cast<Object *>(pipeline.object)->render_flags == ObjectRenderFlags::CMPTOBJECT)
    std_objects.emplace_back(reinterpret_cast<Object *>(pipeline.object));

  if (reinterpret_cast<Object *>(pipeline.object)->render_flags == ObjectRenderFlags::TRNOBJECT)
    trn_objects.emplace_back(reinterpret_cast<Object *>(pipeline.object));

  if (reinterpret_cast<Object *>(pipeline.object)->render_flags == ObjectRenderFlags::CMPTOBJECT)
    compute_objects.emplace_back(reinterpret_cast<Object *>(pipeline.object));

  return reinterpret_cast<Object *>(pipeline.object);
}

void VKSky::CImpl::loadAssets(Object *obj)
{
  obj->setEngineDepends(&vDevice, &VkSwapChain);
  obj->initialization();
  obj->loadTexture();
  obj->setObjectShaders();
}

void VKSky::CImpl::RemoveObject(Object *obj)
{
  obj->destroy();
  // Убираем из вспомогательных массивов объект
  auto iter = std::find(std_objects.begin(), std_objects.end(), obj);
  if (iter != std_objects.end())
  {
    std_objects.erase(iter);
  }
  // Убираем из вспомогательных массивов объект
  iter = std::find(trn_objects.begin(), trn_objects.end(), obj);
  if (iter != trn_objects.end())
  {
    trn_objects.erase(iter);
  }
  //     Очищаем объект
  //     TODO:Перенести управление вектором draw_object внутрь библиотеки
  iter = std::find(all_objects.begin(), all_objects.end(), obj);
  all_objects.erase(iter);
  auto draw_iterator = std::find_if(draw_objects.begin(), draw_objects.end(),
                                    [obj](pipelineObject &_obj)
                                    { return _obj.object == obj; });
  if (draw_iterator != draw_objects.end())
  {
    draw_objects.erase(draw_iterator);
  }
}

void VKSky::CImpl::recreateSwapChain()
{
  if (!prepared)
  {
    return;
  }
  prepared = false;
  resized = true;
  vDevice.windowFrameBuffer();

  vkDeviceWaitIdle(vDevice.logicalDevice);

  VkSwapChain.recreateFrameBuffer();

#ifdef GLFW_LIB_ENABLE
  if (enableUI)
  {
    uiOverlay.resize(VkSwapChain.swapChainExtent.width, VkSwapChain.swapChainExtent.height);
  }
#endif

  destroyCommandBuffers();
  createCommandBuffers();
  buildCommandBuffer();

  for (auto &fence : waitFences)
    vkDestroyFence(vDevice.logicalDevice, fence, nullptr);

  createSynchronizationPrimitives();

  vkDeviceWaitIdle(vDevice.logicalDevice);

  if (VkSwapChain.swapChainExtent.width > 0 && VkSwapChain.swapChainExtent.height > 0)
    camera->updateAspectRatio(static_cast<float>(VkSwapChain.swapChainExtent.width) /
                              static_cast<float>(VkSwapChain.swapChainExtent.height));

  viewChanged();
  prepared = true;
}

void VKSky::CImpl::update()
{
  updateBufferMapped();
}

bool VKSky::CImpl::ready_to_close() const
{
  return vDevice.ready_to_close;
}

void VKSky::CImpl::visible_ui(bool flag)
{
  enableUI = flag;
}

void VKSky::CImpl::destroyCommandBuffers() const
{
  vkFreeCommandBuffers(vDevice.logicalDevice, cmdPool, static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
}

void VKSky::CImpl::prepareObjectType()
{
  auto iter = std::find_if(all_objects.begin(), all_objects.end(),
                           [](Object *obj)
                           { return obj->render_flags == ObjectRenderFlags::TRNOBJECT; });

  if (iter != all_objects.end())
  {
    VkSwapChain.setRenderType(RenderType::USE_TRANSPARENT_OBJECT);
#ifdef GLFW_LIB_ENABLE
    if (enableUI)
      uiOverlay.increaseSubPass();
#endif
  }
}

void VKSky::CImpl::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

void VKSky::CImpl::setupDebugMessenger()
{
#ifdef VULKAN_VALIDATION_LAYERS

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(createInfo);

  if (_CreateDebugUtilsMessengerEXT(vDevice.instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to set up debug messenger!");
  }

#endif
}

void VKSky::CImpl::cleanup()
{
  VkSwapChain.cleanupSwapChain();
  for (auto &obj : all_objects)
    obj->cleanObjectSwapChain();

  vkDestroyPipelineCache(vDevice.logicalDevice, pipelineCache, nullptr);

  for (auto &obj : all_objects)
    vkDestroyPipelineCache(vDevice.logicalDevice, obj->pipelineCache, nullptr);

  for (size_t i = 0; i < all_objects.size(); i++)
  {
    vkDestroyPipeline(vDevice.logicalDevice, all_objects.at(i)->pipeline, nullptr);
    vkDestroyPipelineLayout(vDevice.logicalDevice, all_objects.at(i)->pipelineLayout, nullptr);
    vkDestroyRenderPass(vDevice.logicalDevice, all_objects.at(i)->renderPass, nullptr);
  }
  vkDestroyRenderPass(vDevice.logicalDevice, vDevice.renderPass, nullptr);

  for (auto &obj : all_objects)
  {
    obj->destroyShaderModules();
    obj->uniformObjectBuffer.destroy();
    vkDestroyDescriptorPool(vDevice.logicalDevice, obj->descriptorPool, nullptr);
  }

  destroyCommandBuffers();

  for (auto &threadObject : threadObjects)
    threadObject.destroy();

  for (auto &all_object : all_objects)
    vkDestroyDescriptorSetLayout(vDevice.logicalDevice, all_object->get_descriptor_set_layout(),
                                 nullptr);

  for (auto &all_object : all_objects)
    all_object->object_destroy();

  vDevice.clearQueryPool();

  vkDestroySemaphore(vDevice.logicalDevice, vDevice.semaphores.presentComplete, nullptr);
  vkDestroySemaphore(vDevice.logicalDevice, vDevice.semaphores.renderComplete, nullptr);
  // Destroy Semaphore for compute objects
  for (auto &cmp_object : compute_objects)
    cmp_object->clearComputeBlock();

  for (auto &fence : waitFences)
    vkDestroyFence(vDevice.logicalDevice, fence, nullptr);

#ifdef GLFW_LIB_ENABLE
  if (enableUI)
    uiOverlay.freeResources();
#endif

  vkDestroyCommandPool(vDevice.logicalDevice, cmdPool, nullptr);
  vkDestroyCommandPool(vDevice.logicalDevice, vDevice.commandPool, nullptr);

  vkDestroyPipelineCache(vDevice.logicalDevice, pipelineCache, nullptr);

  vkDestroyDevice(vDevice.logicalDevice, nullptr);
#ifdef VULKAN_VALIDATION_LAYERS
  DestroyDebugUtilsMessengerEXT(vDevice.instance, debugMessenger, nullptr);
#endif
  for (auto &obj : all_objects)
    delete obj;

  vkDestroySurfaceKHR(vDevice.instance, vDevice.surface, nullptr);
  vkDestroyInstance(vDevice.instance, nullptr);
}

bool VKSky::CImpl::checkValidationLayerSupport()
{
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers)
  {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers)
    {
      if (strcmp(layerName, layerProperties.layerName) == 0)
      {
        layerFound = true;
        break;
      }
    }

    if (!layerFound)
    {
      return false;
    }
  }

  return true;
}

void VKSky::CImpl::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                 const VkAllocationCallbacks *pAllocator)
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
                                                                         "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr)
  {
    func(instance, debugMessenger, pAllocator);
  }
}

void VKSky::CImpl::createPipelineCache()
{
  for (auto draw : all_objects)
    draw->createPipelineCache();
  // VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
  // pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  // if (vkCreatePipelineCache(vDevice.logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache)) {
  //     throw std::runtime_error("failed too create pipeline cache");
  // }
}

void VKSky::CImpl::manageViewportDraw(unsigned int current_buff)
{
  std::vector<VkViewport> viewports;
  std::vector<VkRect2D> scissorRects;
  // TODO: make depended from some variable viewport
  unsigned viewport_count = 1;
  viewports.resize(viewport_count);
  scissorRects.resize(viewport_count);
  for (unsigned vc = 0; vc < viewport_count; ++vc)
  {
    viewports[vc] = {
        0, 0, (float)VkSwapChain.swapChainExtent.width / ((float)(vc + 1)),
        (float)VkSwapChain.swapChainExtent.height, 0.0,
        1.0f};
    //                viewports[1] = {(float) VkSwapChain.swapChainExtent.width / 2.0f,
    //                                0,
    //                                (float) VkSwapChain.swapChainExtent.width / 2.0f,
    //                                (float) VkSwapChain.swapChainExtent.height, 0.0,
    //                                1.0f};
    scissorRects[vc] = {
        initializers::rect2D(VkSwapChain.swapChainExtent.width /
                                 (vc + 1),
                             VkSwapChain.swapChainExtent.height, 0, 0)
        //                        initializers::rect2D(VkSwapChain.swapChainExtent.width / 2, VkSwapChain.swapChainExtent.height,
        //                                             VkSwapChain.swapChainExtent.width / 2, 0)
    };
  }
  for (unsigned vc = 0; vc < viewport_count; vc++)
  {
    vkCmdSetViewport(commandBuffers[current_buff], 0, 1, &viewports[vc]);
    vkCmdSetScissor(commandBuffers[current_buff], 0, 1, &scissorRects[vc]);
    //                vkCmdSetLineWidth(commandBuffers[i],1.0f);
    for (auto &obj : std_objects)
      obj->object_draw(commandBuffers[current_buff]);
  }
  if (!trn_objects.empty())
  {
    vkCmdNextSubpass(commandBuffers[current_buff], VK_SUBPASS_CONTENTS_INLINE);
    for (unsigned vc = 0; vc < viewport_count; vc++)
    {
      vkCmdSetViewport(commandBuffers[current_buff], 0, 1, &viewports[vc]);
      vkCmdSetScissor(commandBuffers[current_buff], 0, 1, &scissorRects[vc]);
      // Делаем переход между SubPass на следующий уровень

      for (auto &obj : trn_objects)
        obj->draw(commandBuffers[current_buff]);
    }
  }
}

void VKSky::CImpl::prepareMultiThreadRender()
{
  for (auto &object : threadObjects)
    object.configure_buffer(vDevice.logicalDevice, VkSwapChain.queueNodeIndex, VkSwapChain.imageCount);
}

void VKSky::CImpl::threadRenderFunction(uint32_t threadIndex, uint32_t object_index, uint32_t bufferCount,
                                        VkCommandBufferInheritanceInfo inheritanceInfo)
{
  Object *object = threadObjects.at(threadIndex).objectsInThread.at(object_index);
  if (object == nullptr)
  {
    throw std::runtime_error("object is nullptr");
  }
  VkCommandBufferBeginInfo commandBufferBeginInfo = initializers::commandBufferBeginInfo();
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

  VkCommandBuffer cmdBuffer = threadObjects.at(threadIndex).get_cmdBuffer(object_index, bufferCount);

  VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo));

  VkViewport viewport = initializers::viewport((float)VkSwapChain.swapChainExtent.width,
                                               (float)VkSwapChain.swapChainExtent.height, 0.0f, 1.0f);
  vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

  VkRect2D scissor = initializers::rect2D(static_cast<int32_t>(VkSwapChain.swapChainExtent.width),
                                          static_cast<int32_t>(VkSwapChain.swapChainExtent.height), 0,
                                          0);
  vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

  // if (vDevice.supportedFeatures.pipelineStatisticsQuery && vDevice.queryPool != VK_NULL_HANDLE)
  //     vkCmdResetQueryPool(cmdBuffer, vDevice.queryPool, 0, 2);

  // vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

  object->object_draw(cmdBuffer);

  // vkCmdEndRenderPass(cmdBuffer);

  VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer));
}

std::vector<const char *> VKSky::CImpl::getRequiredExtensions()
{
  // TODO: Учесть возможные расширения
  std::vector<const char *> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
#ifdef VK_USE_PLATFORM_WIN32_KHR
  extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  extensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
  extensions.emplace_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif
#ifdef VULKAN_VALIDATION_LAYERS
  if (!extensions.empty())
  {
    extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
#endif
  return extensions;
}

void VKSky::CImpl::submitFrame()
{
  if (paused)
    return;

  if (vDevice.supportedFeatures.pipelineStatisticsQuery)
    vDevice.getQueryPoolResult();

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = NULL;

  if (vDevice.semaphores.renderComplete != VK_NULL_HANDLE)
  {
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &vDevice.semaphores.renderComplete;
  }

  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &VkSwapChain.swapChain;

  presentInfo.pImageIndices = &imageIndex;

  VkResult result = vkQueuePresentKHR(vDevice.queue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vDevice.framebufferResized)
  {
    vDevice.framebufferResized = false;
    recreateSwapChain();
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      return;
    }
  }
  else if (result != VK_SUCCESS)
  {
    throw std::runtime_error("failed to present swap chain image!");
  }

  VK_CHECK_RESULT(vkQueueWaitIdle(vDevice.queue));
}

void VKSky::CImpl::endRender()
{
  //    VK_CHECK_RESULT(vkQueueSubmit(vDevice.queue, 1, &submitInfo, renderFence));
  std::vector<VkPipelineStageFlags> graphicsWaitStageMasks{};
  std::vector<VkSemaphore> graphicsWaitSemaphores{};
  std::vector<VkSemaphore> graphicsSignalSemaphores{};

  if (!compute_objects.empty())
  {
    for (auto &obj : compute_objects)
    {

      graphicsWaitSemaphores.emplace_back(*obj->getComputeSemaphore());
      graphicsSignalSemaphores.emplace_back(*obj->getGraphicSemaphore());
      graphicsWaitStageMasks.emplace_back(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
      graphicsWaitStageMasks.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }
    //        graphicsWaitStageMasks = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
    //                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    graphicsWaitSemaphores.emplace_back(vDevice.semaphores.presentComplete);
    graphicsSignalSemaphores.emplace_back(vDevice.semaphores.renderComplete);
    //        graphicsWaitStageMasks.resize(graphicsSignalSemaphores.size());
    // Submit graphics commands
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    submitInfo.waitSemaphoreCount = graphicsWaitSemaphores.size();
    submitInfo.pWaitSemaphores = graphicsWaitSemaphores.data();
    submitInfo.pWaitDstStageMask = graphicsWaitStageMasks.data();
    submitInfo.signalSemaphoreCount = graphicsSignalSemaphores.size();
    submitInfo.pSignalSemaphores = graphicsSignalSemaphores.data();
  }
  else
  {
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
  }
  if (prepared)
  {
    VK_CHECK_RESULT(vkQueueSubmit(vDevice.queue, 1, &submitInfo, VK_NULL_HANDLE));
  }
  submitFrame();
  m_signalFrame = true;
}

void VKSky::CImpl::prepareRender()
{
  if (paused)
    return;
  VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

  // TODO: Move to prepare Farme
  //  Submit compute commands
  if (!compute_objects.empty())
  {
    VkSubmitInfo computeSubmitInfo = initializers::submitInfo();
    for (const auto &obj : compute_objects)
    {
      std::vector<VkSemaphore> graphic, compute;
      std::vector<VkCommandBuffer> buffers;
      std::vector<VkPipelineStageFlags> waitStageMasks;
      graphic.emplace_back(*obj->getGraphicSemaphore());
      compute.emplace_back(*obj->getComputeSemaphore());
      buffers.emplace_back(*obj->getComputeBuffer());
      waitStageMasks.emplace_back(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
      //        }
      computeSubmitInfo.commandBufferCount = buffers.size();
      computeSubmitInfo.pCommandBuffers = buffers.data();
      computeSubmitInfo.waitSemaphoreCount = graphic.size();
      computeSubmitInfo.pWaitSemaphores = graphic.data();
      computeSubmitInfo.pWaitDstStageMask = waitStageMasks.data();
      computeSubmitInfo.signalSemaphoreCount = compute.size();
      computeSubmitInfo.pSignalSemaphores = compute.data();

      //        for (auto &obj: compute_objects) {
      VK_CHECK_RESULT(vkQueueSubmit(obj->getComputeQueue(), 1, &computeSubmitInfo, VK_NULL_HANDLE));
    }
  }
  // TODOEND

  VkResult result = vkAcquireNextImageKHR(vDevice.logicalDevice, VkSwapChain.swapChain, UINT64_MAX,
                                          vDevice.semaphores.presentComplete, (VkFence) nullptr, &imageIndex);

  if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
  {
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      recreateSwapChain();
    }
    return;
  }
  else
  {
    VK_CHECK_RESULT(result);
  }
  VK_CHECK_RESULT(vkQueueWaitIdle(vDevice.queue));
}

void VKSky::CImpl::initial_update()
{
#if defined(_MSC_VER) || defined(_WIN32)
  tStart = std::chrono::steady_clock::now();
#else
  tStart = std::chrono::high_resolution_clock::now();
#endif
  if (viewUpdated)
  {
    viewUpdated = false;
    viewChanged();
  }
}

void VKSky::CImpl::camera_update()
{
  frameCounter++;

#if defined(_MSC_VER) || defined(_WIN32)
  auto tEnd = std::chrono::steady_clock::now();
#else
  auto tEnd = std::chrono::high_resolution_clock::now();
#endif

  auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();

  frameTimer = (float)tDiff / 1000.0f;
  if (frameTimer <= 0)
    frameTimer = 0.00001f;

  camera->update(frameTimer);
  if (camera->moving())
  {
    viewUpdated = true;
  }
  // Convert to clamped timer value
  if (!paused)
  {
    timer += timerSpeed * frameTimer;
    if (timer > 1.0)
    {
      timer -= 1.0f;
    }
  }

  float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count());
  if (fpsTimer > 1000.0f)
  {
    lastFPS = static_cast<uint32_t>((float)frameCounter * (1000.0f / fpsTimer));
    frameCounter = 0;
    lastTimestamp = tEnd;
  }
  tPrevEnd = tEnd;
}

void VKSky::CImpl::prepareFence()
{
  if (paused)
    return;
  m_signalFrame = false;
  // check fence
  //    VkResult fenceResult;
  //    do {
  //        fenceResult = vkWaitForFences(vDevice.logicalDevice, 1, &renderFence, VK_TRUE, 100000000);
  //    } while (fenceResult == VK_TIMEOUT);
  //
  //    VK_CHECK_RESULT(fenceResult);
  //    vkResetFences(vDevice.logicalDevice, 1, &renderFence);

  prepareRender();
}

// void VKSky::CImpl::implLoop() {
//     //#ifdef QT_LIB_ENABLE
//     //        while (!done) {
//     //            if (vulkan_widget != nullptr)
//     //                vulkan_widget->eventHandler();
//     //            drawFrame();
//     //        }
//     //#endif
// #if defined(VK_USE_PLATFORM_XCB_KHR)
//
//     while (!done) {
//         //                addMoreElements();
//         if (vulkan_widget != nullptr)
//             vulkan_widget->eventHandler();
//         else {
//             xcb_generic_event_t *event;
//             event = xcb_poll_for_event(vDevice.crossWindow.m_connection);
//             while (event != NULL) {// xcb_poll_for_event(vDevice.crossWindow.m_connection))) {
//                 vDevice.crossWindow.handleEvent(event);
//                 free(event);
//             }
//         }
//         ptr->drawFrame();
//     }
// #elif(VK_USE_PLATFORM_WIN32_KHR)
//     while (!done) {
//                 if (vulkan_widget != nullptr)
//                     vulkan_widget->eventHandler();
//                 drawFrame();
//             }
// #endif
//
// }

void VKSky::CImpl::configureAssetsBuffer()
{
  LoadAssets();
}

void VKSky::CImpl::createSynchronizationPrimitives()
{
  VkFenceCreateInfo fenceCreateInfo = initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
  auto sz = commandBuffers.size();
  waitFences.resize(sz);
  for (auto &fence : waitFences)
  {
    VK_CHECK_RESULT(vkCreateFence(vDevice.logicalDevice, &fenceCreateInfo, nullptr, &fence));
  }
}

void VKSky::CImpl::createCommandBuffers()
{
  commandBuffers.resize(VkSwapChain.imageCount);
  uiCmdBuffer.resize(VkSwapChain.imageCount);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = cmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(vDevice.logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(uiCmdBuffer.size());

  if (vkAllocateCommandBuffers(vDevice.logicalDevice, &allocInfo, uiCmdBuffer.data()) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void VKSky::CImpl::buildCommandBuffer()
{
  VkCommandBufferBeginInfo beginInfo = initializers::commandBufferBeginInfo();

  // std::array<VkClearValue, 2> clearValues{};
  std::vector<VkClearValue> clearValues;
  clearValues.resize(2);
  for (auto k = 0; k < 4; k++)
    clearValues[0].color.float32[k] = color_array[k]; // 0. 0. 0.
  // clearValues[0].color = color_array;
  clearValues[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vDevice.renderPass;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = VkSwapChain.swapChainExtent;
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  for (size_t i = 0; i < commandBuffers.size(); i++)
  {
    std::vector<VkCommandBuffer> lcommandBuffers;

    VkCommandBufferInheritanceInfo inheritanceInfo = initializers::commandBufferInheritanceInfo();
    inheritanceInfo.renderPass = vDevice.renderPass;
    inheritanceInfo.framebuffer = VkSwapChain.swapChainFramebuffers[i];
    renderPassInfo.framebuffer = VkSwapChain.swapChainFramebuffers[i];

    VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[i], &beginInfo));

    acquireBarrier(commandBuffers[i]);

    if (vDevice.supportedFeatures.pipelineStatisticsQuery && vDevice.queryPool != VK_NULL_HANDLE)
    {
      vkCmdResetQueryPool(commandBuffers[i], vDevice.queryPool, 0, 2);
    }

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         (threadObjects.empty())
                             ? VK_SUBPASS_CONTENTS_INLINE
                             : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    // VK_SUBPASS_CONTENTS_INLINE , VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS

    if (threadObjects.empty())
    {
      manageViewportDraw(i);
    }
    else
    {
      std::unique_lock<std::mutex> lk(mute);

      for (uint32_t t = 0; t < threadObjects.size(); t++)
      {
        // threadObjects
        for (uint32_t objNum = 0; objNum < threadObjects.at(t).numOfObjectsInThread; objNum++)
          threadPool.threads[t]->addJob([=]
                                        { threadRenderFunction(t, objNum, i, inheritanceInfo); });
      }

      threadPool.wait();

      lk.unlock();

      for (auto &threadObject : threadObjects)
      {
        for (uint32_t k = 0; k < threadObject.numOfObjectsInThread; k++)
        {
          lcommandBuffers.emplace_back(threadObject.get_cmdBuffer(k, i));
        }
      }
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
      beginInfo.pInheritanceInfo = &inheritanceInfo;
      VK_CHECK_RESULT(vkBeginCommandBuffer(uiCmdBuffer[i], &beginInfo));
      // Todo: Make Variable
      //  указывается с учетом расположения на экране в окне
      VkViewport viewports[2];
      viewports[0] = {
          0, 0, (float)VkSwapChain.swapChainExtent.width / 2.0f,
          (float)VkSwapChain.swapChainExtent.height, 0.0,
          1.0f};
      // initializers::viewport((float) VkSwapChain.swapChainExtent.width,
      //                    (float) VkSwapChain.swapChainExtent.height, 0.0f, 1.0f);
      viewports[1] = {
          (float)VkSwapChain.swapChainExtent.width / 2.0f,
          (float)VkSwapChain.swapChainExtent.width / 2.0f,
          (float)(float)VkSwapChain.swapChainExtent.height, 0.0,
          1.0f};
      // initializers::viewport((float) VkSwapChain.swapChainExtent.width,
      //                   (float) VkSwapChain.swapChainExtent.height, 0.0f, 1.0f);
      vkCmdSetViewport(uiCmdBuffer[i], 0, 2, viewports);

      VkRect2D scissorRects[2] = {
          initializers::rect2D(VkSwapChain.swapChainExtent.width /
                                   2,
                               VkSwapChain.swapChainExtent.height, 0, 0),
          initializers::rect2D(VkSwapChain.swapChainExtent.width / 2, VkSwapChain.swapChainExtent.height,
                               VkSwapChain.swapChainExtent.width / 2, 0)};

      // auto scissor = initializers::rect2D(static_cast<int32_t>(VkSwapChain.swapChainExtent.width),
      //                                     static_cast<int32_t>(VkSwapChain.swapChainExtent.height), 0, 0);
      vkCmdSetScissor(uiCmdBuffer[i], 0, 2, scissorRects);
    }
    // Всегда отображается последним и с последним subpass
    drawUI((threadObjects.empty()) ? commandBuffers[i] : uiCmdBuffer[i]); // commandBuffers[i]);

    if (!threadObjects.empty())
    {
      VK_CHECK_RESULT(vkEndCommandBuffer(uiCmdBuffer[i]));

      lcommandBuffers.emplace_back(uiCmdBuffer[i]);

      vkCmdExecuteCommands(commandBuffers[i], lcommandBuffers.size(), lcommandBuffers.data());
    }

    vkCmdEndRenderPass(commandBuffers[i]);

    releaseBarrier(commandBuffers[i]);

    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[i]));
  }
  vkQueueWaitIdle(vDevice.queue);

  readSharedData();
}

void VKSky::CImpl::drawUI(VkCommandBuffer commandBuffer)
{
  #ifdef GLFW_LIB_ENABLE
  if (enableUI && uiOverlay.visible)
  {
    const VkViewport viewport = initializers::viewport(static_cast<float>(VkSwapChain.swapChainExtent.width),
                                                       static_cast<float>(VkSwapChain.swapChainExtent.height),
                                                       0.0f,
                                                       1.0f);
    const VkRect2D scissor = initializers::rect2D(static_cast<int32_t>(VkSwapChain.swapChainExtent.width),
                                                  static_cast<int32_t>(VkSwapChain.swapChainExtent.height),
                                                  0,
                                                  0);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    uiOverlay.draw(commandBuffer);
  }
  #endif
}

void VKSky::CImpl::updateCommandBuffer()
{
  buildCommandBuffer();
}

void VKSky::CImpl::createCommandPool()
{
  // QueueFamilyIndices queueFamilyIndices = vDevice.findQueueFamilies(vDevice.getPhysicalDevice());

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = VkSwapChain.queueNodeIndex;

  if (vkCreateCommandPool(vDevice.logicalDevice, &poolInfo, nullptr, &cmdPool) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create graphics command pool!");
  }
}

void VKSky::CImpl::createInstance()
{
#ifdef VULKAN_VALIDATION_LAYERS
  if (!checkValidationLayerSupport())
  {
    throw std::runtime_error("validation layers requested, but not available!");
  }
#endif
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = appConfig->name.data();
  appInfo.applicationVersion = VK_MAKE_VERSION(appConfig->major_version, appConfig->minor_version,
                                               appConfig->patch_version);
  appInfo.pEngineName = engConfig->name.data();
  appInfo.engineVersion = VK_MAKE_VERSION(engConfig->major_version, engConfig->minor_version,
                                          engConfig->patch_version);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#ifdef VULKAN_VALIDATION_LAYERS
  createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
  createInfo.ppEnabledLayerNames = validationLayers.data();

  populateDebugMessengerCreateInfo(debugCreateInfo);
  createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
#else
  createInfo.enabledLayerCount = 0;

  createInfo.pNext = nullptr;
#endif

  if (vkCreateInstance(&createInfo, vDevice.g_Allocator, &vDevice.instance) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create instance!");
  }
#ifdef QT_LIB_ENABLE
  //    vDevice.q_instance.setVkInstance(vDevice.instance);
  //    if(!vDevice.q_instance.create()) {
  //        throw std::runtime_error("failed to create instance with error code: " + std::to_string(vDevice.q_instance.errorCode()));
  //    }
#endif
}

void VKSky::CImpl::initWindow()
{
#ifdef QT_LIB_ENABLE
  vDevice.initWindow(appConfig->name, vulkan_widget);
#endif
#ifdef GLFW_LIB_ENABLE
  vDevice.initWindow(appConfig->name);
#endif
  screen.width = vDevice.Width();
  screen.height = vDevice.Height();
  VkSwapChain.vDevice = &vDevice;
}

void VKSky::CImpl::initVulkan()
{
  // prepare
  createInstance();
  setupDebugMessenger();
  vDevice.createSurface();
  vDevice.pickPhysicalDevice();
  vDevice.createLogicalDevice(enabledDeviceExtensions, deviceCreatepNextChain);
  VkSwapChain.createSwapChain();

  createCommandPool();

  vkGetDeviceQueue(vDevice.logicalDevice, vDevice.queueFamilyIndices.graphics, 0, &vDevice.queue);

  createCommandBuffers();
  createSynchronizationPrimitives();

  configureAssetsBuffer();

  // prepareMultiThreadRender();

  createDescriptorSetLayout();
  createPipelineCache();
  createGraphicsPipeline();
  // Prepare UI
  createUI();
  vDevice.setupQueryResultBuffer();

  createUniformBuffers();

  createDescriptorPool();
  createDescriptorSets();
  createAdditinalBuffer();

  createSyncObjects();
  //
  submitInfo = initializers::submitInfo();
  submitInfo.pWaitDstStageMask = &submitPipelineStages;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &vDevice.semaphores.presentComplete;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &vDevice.semaphores.renderComplete;

  buildCommandBuffer();
  prepared = true;
}

void VKSky::CImpl::first_initialization()
{
  // TODO: Add Qt Events on keyboards
  initVulkan();
}

void VKSky::CImpl::do_magick()
{
  first_initialization();
}

void VKSky::CImpl::createUniformBuffers()
{
  for (auto &all_object : all_objects)
    all_object->createAllBuffers();
}

void VKSky::CImpl::createDescriptorPool()
{
  for (size_t i = 0; i < all_objects.size(); i++)
    all_objects.at(i)->objectCreateDescriptorPool();
}

void VKSky::CImpl::createDescriptorSets()
{
  for (size_t j = 0; j < all_objects.size(); j++)
    all_objects.at(j)->objectCreateDescriptorSets();
}

// Virtual function
void VKSky::CImpl::createDescriptorSetLayout()
{
  for (auto draw : all_objects)
    draw->objectSetDescriptorLayout();
}

// Function can change not conceptual
void VKSky::CImpl::createGraphicsPipeline()
{
  for (auto &obj : all_objects)
    obj->preparePipeline();
}

void VKSky::CImpl::createSyncObjects()
{
  VkSemaphoreCreateInfo semaphoreInfo = initializers::semaphoreCreateInfo();

  if (vkCreateSemaphore(vDevice.logicalDevice, &semaphoreInfo, nullptr, &vDevice.semaphores.presentComplete) !=
          VK_SUCCESS ||
      vkCreateSemaphore(vDevice.logicalDevice, &semaphoreInfo, nullptr, &vDevice.semaphores.renderComplete) !=
          VK_SUCCESS) // ||
  {
    throw std::runtime_error("failed to create synchronization objects for a frame!");
  }

  //    VkFenceCreateInfo fenceCreateInfo = initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
  //    vkCreateFence(vDevice.logicalDevice, &fenceCreateInfo, nullptr, &renderFence);
}

void VKSky::CImpl::LoadAssets()
{
  for (auto &all_object : all_objects)
  {
    all_object->setEngineDepends(&vDevice, &VkSwapChain);
    all_object->initialization();
    all_object->loadTexture();
    all_object->setObjectShaders();
    all_object->prepare();
  }
}

void VKSky::CImpl::viewChanged()
{
  updateCommandBuffer();
}

ThreadObject::ThreadObject(VkDevice *_device)
{
  device = _device;
}

void ThreadObject::destroy()
{
  if (!commandBuffer.empty())
  {
    vkFreeCommandBuffers(*device, commandPool, static_cast<uint32_t>(commandBuffer.size()),
                         commandBuffer.data());
    vkDestroyCommandPool(*device, commandPool, nullptr);
  }
}

void ThreadObject::configure_buffer(VkDevice _device, uint32_t queueNodeIndex, uint32_t imageIndex)
{
  destroy();
  // Create one command pool for each thread
  VkCommandPoolCreateInfo cmdPoolInfo = initializers::commandPoolCreateInfo();
  cmdPoolInfo.queueFamilyIndex = queueNodeIndex;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  VK_CHECK_RESULT(vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &commandPool));

  // One secondary command buffer per object that is updated by this thread
  commandBuffer.resize(numOfObjectsInThread * imageIndex);
  // Generate secondary command buffers for each thread
  VkCommandBufferAllocateInfo secondaryCmdBufAllocateInfo =
      initializers::commandBufferAllocateInfo(
          commandPool,
          VK_COMMAND_BUFFER_LEVEL_SECONDARY,
          commandBuffer.size());
  VK_CHECK_RESULT(vkAllocateCommandBuffers(_device, &secondaryCmdBufAllocateInfo, commandBuffer.data()));
}

void ThreadObject::remove()
{
}

VkCommandBuffer ThreadObject::get_cmdBuffer(uint32_t elNum, uint32_t imageIndex)
{
  return commandBuffer[elNum + (numOfObjectsInThread * imageIndex)];
}

std::vector<VkCommandBuffer> ThreadObject::get_buffers()
{
  return commandBuffer;
}

VKSky::CImpl::CImpl()
{
  numThreads = std::thread::hardware_concurrency();
  assert(numThreads > 0);
  std::cout << "numThreads = " << numThreads << std::endl;
  threadPool.setThreadCount(numThreads);
}

VKSky::CImpl::~CImpl()
{
  vDevice.set_quit(true);
  if (magick_thread.joinable())
    magick_thread.join();
  cleanup();
}

void VKSky::CImpl::run() { do_magick(); }

void VKSky::CImpl::createAdditinalBuffer() const
{
  for (auto &obj : all_objects)
    obj->createAdditinalBuffer();
}

void VKSky::CImpl::acquireBarrier(VkCommandBuffer _buffer)
{
  for (auto &obj : all_objects)
    obj->acquireBarrier(_buffer);
}

void VKSky::CImpl::releaseBarrier(VkCommandBuffer _buffer)
{
  for (auto &obj : all_objects)
    obj->releaseBarrier(_buffer);
}

void VKSky::CImpl::readSharedData()
{
  for (auto &obj : all_objects)
    obj->readShaderData();
}

void VKSky::CImpl::set_new_window_size(int width, int height)
{
  //    *vDevice.Width() = width;
  //    *vDevice.Height() = height;
}

void VKSky::CImpl::waitForCurrentFrameComplete()
{
  paused = true;
  while (!m_signalFrame)
  {
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}

void VKSky::CImpl::updateOverlay()
{
  if (!enableUI)
    return;
}

bool VKSky::CImpl::is_ui_enable() const
{
  return enableUI;
}

void VKSky::CImpl::prepareUI()
{
#ifdef GLFW_LIB_ENABLE
  // setUIParametrs();
  if (!enableUI)
    return;

  ImGui_ImplGlfw_NewFrame();

  ImGuiIO &io = ImGui::GetIO();

  io.DisplaySize = ImVec2(static_cast<float>(VkSwapChain.swapChainExtent.width),
                          static_cast<float>(VkSwapChain.swapChainExtent.height));
  io.DeltaTime = frameTimer;

  ImGui::NewFrame();
  // Standart cap for ui
  //    ImGui::SetNextWindowSize(io.DisplaySize);
  //    ImGui::SetNextWindowPos(ImVec2(0,0));
  if (ui_param.fix_on_position())
  {
    ImGui::SetNextWindowSize(ui_param.fix_on_position.pos(), ui_param.fix_on_position.cond());
  }

  ImGui::Begin(ui_param.ui_name.c_str(), nullptr,
               ui_param.ui_flags); // | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
  //    ImGui::ShowDemoWindow();
  for (auto &ui_string : ui_param.preview_text)
  {
    ImGui::TextUnformatted(ui_string.c_str());
  }
  if (ui_param.enable_fps_rate)
    ImGui::Text("%.2f ms/frame (%.1d fps)", (1000.0f / static_cast<float>(lastFPS)), lastFPS);
#endif
}

void VKSky::CImpl::renderUI()
{
#ifdef GLFW_LIB_ENABLE
  if (!enableUI)
    return;
  //
  ImGui::End();
  ImGui::Render();

  if (uiOverlay.update() || uiOverlay.updated)
  {
    buildCommandBuffer();
    uiOverlay.updated = false;
  }
#endif
}

void VKSky::CImpl::createUI()
{
#ifndef QT_LIB_ENABLE
  if (enableUI)
  {
    uiOverlay.device = &vDevice;
    uiOverlay.queue = vDevice.queue;
    uiOverlay.shaders = {
        tools::LoadShader(vDevice.logicalDevice, std::string(SHADER_DIRECTORY) + "/uioverlay.vert.spv",
                          VK_SHADER_STAGE_VERTEX_BIT,
                          uiOverlay.module_vert),
        tools::LoadShader(vDevice.logicalDevice, std::string(SHADER_DIRECTORY) + "/uioverlay.frag.spv",
                          VK_SHADER_STAGE_FRAGMENT_BIT,
                          uiOverlay.module_frag),
    };
    uiOverlay.prepareResources(vDevice.get_glfw_window_ptr());
    uiOverlay.preparePipeline(pipelineCache, vDevice.renderPass, &VkSwapChain, vDevice.findDepthFormat());
  }
#endif // QT_LIB_ENABLE
}

#ifdef QT_LIB_ENABLE

#include "moc_vk_sky_impl.cpp"

#endif
