#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "vk_sky_swapchain.hpp"
#include "tools.hpp"

VulkanSwapChain::VulkanSwapChain() {}

VulkanSwapChain::~VulkanSwapChain() {}

void VulkanSwapChain::createSwapChain() {
  searchSurfaceFormat();
  create(vDevice->Width(), vDevice->Height(), true);
  createRenderPass();
  createImageViews();
  createColorResources();
  createDepthResources();
  createFramebuffersSW();
}

void VulkanSwapChain::clearAttachment(FrameBufferAttachments *attach) {
  vkDestroyImageView(vDevice->logicalDevice, attach->view, nullptr);
  vkDestroyImage(vDevice->logicalDevice, attach->image, nullptr);
  vkFreeMemory(vDevice->logicalDevice, attach->mem, nullptr);
}

void VulkanSwapChain::createAttacment(VkFormat format, VkImageUsageFlags usage,
                                      FrameBufferAttachments *attachment) {
  if (attachment->image != VK_NULL_HANDLE) {
    clearAttachment(attachment);
  }
  VkImageAspectFlags aspectMask = 0;
  VkImageLayout imageLayout;

  attachment->format = format;

  if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
    aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
  }

  assert(aspectMask > 0);

  VkImageCreateInfo image = initializers::imageCreateInfo();
  image.imageType = VK_IMAGE_TYPE_2D;
  image.format = format;
  image.extent.width = attachments.width;
  image.extent.height = attachments.height;
  image.extent.depth = 1;
  image.mipLevels = 1;
  image.arrayLayers = 1;
  image.samples = vDevice->msaaSamples;
  image.tiling = VK_IMAGE_TILING_OPTIMAL;

  image.usage = usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
  image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
  VkMemoryRequirements memReqs;

  VK_CHECK_RESULT(vkCreateImage(vDevice->logicalDevice, &image, nullptr,
                                &attachment->image));
  vkGetImageMemoryRequirements(vDevice->logicalDevice, attachment->image,
                               &memReqs);
  memAlloc.allocationSize = memReqs.size;
  memAlloc.memoryTypeIndex = vDevice->getMemoryType(
      memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(vDevice->logicalDevice, &memAlloc, nullptr,
                                   &attachment->mem));
  VK_CHECK_RESULT(vkBindImageMemory(vDevice->logicalDevice, attachment->image,
                                    attachment->mem, 0));

  VkImageViewCreateInfo imageView = initializers::imageViewCreateInfo();
  imageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageView.format = format;
  imageView.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                          VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  imageView.subresourceRange = {};
  imageView.subresourceRange.aspectMask = aspectMask;
  imageView.subresourceRange.baseMipLevel = 0;
  imageView.subresourceRange.levelCount = 1;
  imageView.subresourceRange.baseArrayLayer = 0;
  imageView.subresourceRange.layerCount = 1;
  imageView.image = attachment->image;

  VK_CHECK_RESULT(vkCreateImageView(vDevice->logicalDevice, &imageView, nullptr,
                                    &attachment->view));
}

void VulkanSwapChain::createGBufferAttachments() {
  // ! Не забывать отслеживать формат картинки для отображения
  createAttacment(VK_FORMAT_R16G16B16_SFLOAT,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &attachments.position);
  createAttacment(VK_FORMAT_R16G16B16_SFLOAT,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &attachments.normal);
  createAttacment(VK_FORMAT_R8G8B8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                  &attachments.position);
}

void VulkanSwapChain::create(int *width, int *height, bool vsync,
                             bool fullscreen) {
  // Store the current swap chain handle so we can use it later on to ease up
  // recreation
  VkSwapchainKHR oldSwapchain = swapChain;

  // Get physical device surface properties and formats
  VkSurfaceCapabilitiesKHR surfCaps;
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      vDevice->physicalDevice, vDevice->surface, &surfCaps));

  // Get available present modes
  uint32_t presentModeCount;
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(
      vDevice->physicalDevice, vDevice->surface, &presentModeCount, NULL));
  assert(presentModeCount > 0);

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(
      vDevice->physicalDevice, vDevice->surface, &presentModeCount,
      presentModes.data()));

  VkExtent2D swapchainExtent = {};
  // If width (and height) equals the special value 0xFFFFFFFF, the size of the
  // surface will be set by the swapchain
  if (surfCaps.currentExtent.width == (uint32_t)-1) {
    // If the surface size is undefined, the size is set to
    // the size of the images requested.
    swapchainExtent.width = static_cast<uint32_t>(*width);
    swapchainExtent.height = static_cast<uint32_t>(*height);
  } else {
    // If the surface size is defined, the swap chain size must match
    swapchainExtent = surfCaps.currentExtent;
    *width = static_cast<int>(surfCaps.currentExtent.width);
    *height = static_cast<int>(surfCaps.currentExtent.height);
  }

  // Select a present mode for the swapchain

  // The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
  // This mode waits for the vertical blank ("v-sync")
  VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

  // If v-sync is not requested, try to find a mailbox mode
  // It's the lowest latency non-tearing present mode available
  if (!vsync) {
    for (size_t i = 0; i < presentModeCount; i++) {
      if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }
      if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
      }
    }
  }

  // Determine the number of images
  uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
#if (defined(VK_USE_PLATFORM_MACOS_MVK) && defined(VK_EXAMPLE_XCODE_GENERATED))
  // SRS - Work around known MoltenVK issue re 2x frame rate when vsync
  // (VK_PRESENT_MODE_FIFO_KHR) enabled
  struct utsname sysInfo;
  uname(&sysInfo);
  // SRS - When vsync is on, use minImageCount when not in fullscreen or when
  // running on Apple Silcon This forces swapchain image acquire frame rate to
  // match display vsync frame rate
  if (vsync && (!fullscreen || strcmp(sysInfo.machine, "arm64") == 0)) {
    desiredNumberOfSwapchainImages = surfCaps.minImageCount;
  }
#endif
  if ((surfCaps.maxImageCount > 0) &&
      (desiredNumberOfSwapchainImages > surfCaps.maxImageCount)) {
    desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
  }

  // Find the transformation of the surface
  VkSurfaceTransformFlagsKHR preTransform;
  if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    // We prefer a non-rotated transform
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  } else {
    preTransform = surfCaps.currentTransform;
  }

  // Find a supported composite alpha format (not all devices support alpha
  // opaque)
  VkCompositeAlphaFlagBitsKHR compositeAlpha =
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  // Simply select the first composite alpha format available
  std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };
  for (auto &compositeAlphaFlag : compositeAlphaFlags) {
    if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
      compositeAlpha = compositeAlphaFlag;
      break;
    };
  }

  VkSwapchainCreateInfoKHR swapchainCI = {};
  swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCI.surface = vDevice->surface;
  swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
  swapchainCI.imageFormat = swapChainImageFormat;
  swapchainCI.imageColorSpace = swapChainColorSpace;
  swapchainCI.imageExtent = {swapchainExtent.width, swapchainExtent.height};
  swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
  swapchainCI.imageArrayLayers = 1;
  swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCI.queueFamilyIndexCount = 0;
  swapchainCI.presentMode = swapchainPresentMode;
  // Setting oldSwapChain to the saved handle of the previous swapchain aids in
  // resource reuse and makes sure that we can still present already acquired
  // images
  swapchainCI.oldSwapchain = oldSwapchain;
  // Setting clipped to VK_TRUE allows the implementation to discard rendering
  // outside of the surface area
  swapchainCI.clipped = VK_TRUE;
  swapchainCI.compositeAlpha = compositeAlpha;

  // Enable transfer source on swap chain images if supported
  if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  // Enable transfer destination on swap chain images if supported
  if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  VK_CHECK_RESULT(vkCreateSwapchainKHR(vDevice->logicalDevice, &swapchainCI,
                                       nullptr, &swapChain));

  // If an existing swap chain is re-created, destroy the old swap chain
  // This also cleans up all the presentable images
  if (oldSwapchain != VK_NULL_HANDLE) {
    for (uint32_t i = 0; i < imageCount; i++) {
      vkDestroyImageView(vDevice->logicalDevice, swapChainImageViews[i],
                         nullptr);
    }
    vkDestroySwapchainKHR(vDevice->logicalDevice, oldSwapchain, nullptr);
  }
  VK_CHECK_RESULT(vkGetSwapchainImagesKHR(vDevice->logicalDevice, swapChain,
                                          &imageCount, NULL));

  // Get the swap chain images
  swapChainImages.resize(imageCount);
  VK_CHECK_RESULT(vkGetSwapchainImagesKHR(vDevice->logicalDevice, swapChain,
                                          &imageCount, swapChainImages.data()));
  swapChainExtent = swapchainExtent;
  vDevice->setImageCount(imageCount);
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  if ((availableFormats.size() == 1) &&
      (availableFormats[0].format == VK_FORMAT_UNDEFINED)) {
    swapChainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapChainColorSpace = availableFormats[0].colorSpace;
  } else {
    // iterate over the list of available surface format and
    // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
    bool found_B8G8R8A8_UNORM = false;
    for (auto &&surfaceFormat : availableFormats) {
      if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
        swapChainImageFormat = surfaceFormat.format;
        swapChainColorSpace = surfaceFormat.colorSpace;
        found_B8G8R8A8_UNORM = true;
        return surfaceFormat;
      }
    }

    // in case VK_FORMAT_B8G8R8A8_UNORM is not available
    // select the first available color format
    if (!found_B8G8R8A8_UNORM) {
      swapChainImageFormat = availableFormats[0].format;
      swapChainColorSpace = availableFormats[0].colorSpace;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
      return availablePresentMode;
    }
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
    if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanSwapChain::cleanupSwapChain() {
  vkDestroyImageView(vDevice->logicalDevice, depthImageView, nullptr);
  vkDestroyImage(vDevice->logicalDevice, depthImage, nullptr);
  vkFreeMemory(vDevice->logicalDevice, depthImageMemory, nullptr);

  vkDestroyImageView(vDevice->logicalDevice, colorImageView, nullptr);
  vkDestroyImage(vDevice->logicalDevice, colorImage, nullptr);
  vkFreeMemory(vDevice->logicalDevice, colorImageMemory, nullptr);

  for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
    vkDestroyFramebuffer(vDevice->logicalDevice, swapChainFramebuffers.at(i),
                         nullptr);
  }

  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    vkDestroyImageView(vDevice->logicalDevice, swapChainImageViews.at(i),
                       nullptr);
  }

  vkDestroySwapchainKHR(vDevice->logicalDevice, swapChain, nullptr);
}

void VulkanSwapChain::addDescriptorAttach(VkDescriptorSet *descriptor) {
  attachDescriptors.emplace_back(descriptor);
}

void VulkanSwapChain::addDescriptorTransperent(VkDescriptorSet *descriptor) {
  attachTransparents.emplace_back(descriptor);
}

/// @brief
void VulkanSwapChain::createFramebuffersSW() {
  std::vector<VkImageView> attachments;

  attachments.resize(imageCount);
  attachments[0] = colorImageView;
  attachments[1] = depthImageView;
  swapChainFramebuffers.resize(imageCount);

  for (int i = 0; i < swapChainImageViews.size(); i++) {
    attachments[2] = swapChainImageViews.at(i);
    // Необходимо формировать с учетом потребности в отрисовке

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vDevice->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vDevice->logicalDevice, &framebufferInfo, nullptr,
                            &swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
    //        }
  }
}

void VulkanSwapChain::createImageViews() {
  swapChainImageViews.resize(swapChainImages.size());

  for (size_t i = 0; i < swapChainImages.size(); i++) {
    swapChainImageViews[i] = createImageView(
        vDevice->logicalDevice, swapChainImages[i], swapChainImageFormat,
        VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
  }
}

// Формируется общий ренедер
void VulkanSwapChain::createRenderPass() {
  std::vector<VkAttachmentDescription> attachments;
  // соблюдаем порядок формирования дополнений
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapChainImageFormat;
  colorAttachment.samples = vDevice->msaaSamples;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  attachments.emplace_back(colorAttachment);

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
  attachments.emplace_back(depthAttachment);

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = swapChainImageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachments.emplace_back(colorAttachmentResolve);

  std::vector<VkAttachmentReference> reference_color_vector =
      tools::set_color_attachments_reference();

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // For standart type subpass
  std::vector<VkSubpassDescription> subpassDescription;
  subpassDescription.emplace_back(VkSubpassDescription());
  subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription[0].colorAttachmentCount = reference_color_vector.size();
  subpassDescription[0].pColorAttachments = reference_color_vector.data();
  subpassDescription[0].pDepthStencilAttachment = &depthAttachmentRef;
  subpassDescription[0].pResolveAttachments = &colorAttachmentResolveRef;

  if (render_type == RenderType::USE_TRANSPARENT_OBJECT) {
    subpassDescription.emplace_back(VkSubpassDescription());

    subpassDescription[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription[1].colorAttachmentCount = reference_color_vector.size();
    subpassDescription[1].pColorAttachments = reference_color_vector.data();
    subpassDescription[1].pDepthStencilAttachment = &depthAttachmentRef;
    subpassDescription[1].pResolveAttachments = &colorAttachmentResolveRef;

    // subpassDescription[1].inputAttachmentCount = 1;
    // subpassDescription[1].pInputAttachments = &inputReferences;
  }
  uint32_t srcSubpass = 0, dstSubpass = VK_SUBPASS_EXTERNAL;
  std::vector<VkSubpassDependency> dependencies;
  dependencies.emplace_back(VkSubpassDependency());
  dependencies.back().srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies.back().dstSubpass = 0;
  dependencies.back().srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies.back().dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies.back().srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies.back().dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies.back().dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  // dependencies.back().srcSubpass = VK_SUBPASS_EXTERNAL;
  // dependencies.back().dstSubpass = 0;
  // dependencies.back().srcStageMask =
  // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
  // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  // dependencies.back().srcAccessMask = 0;
  // dependencies.back().dstStageMask =
  // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
  // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  // dependencies.back().dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
  // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  // dependencies.back().dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  if (render_type == RenderType::USE_TRANSPARENT_OBJECT) {
    dependencies.emplace_back(VkSubpassDependency());
    dependencies.back().srcSubpass = 0;
    dependencies.back().dstSubpass = 1;
    dependencies.back().srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies.back().srcAccessMask = 0;
    dependencies.back().dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies.back().dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    srcSubpass = 1;
  }

  dependencies.emplace_back(VkSubpassDependency());
  dependencies.back().srcSubpass = srcSubpass;
  dependencies.back().dstSubpass = dstSubpass;
  dependencies.back().srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies.back().dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies.back().srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies.back().dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies.back().dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

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
                         &vDevice->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void VulkanSwapChain::createColorResources() {
  VkFormat colorFormat = swapChainImageFormat;

  createImage(swapChainExtent.width, swapChainExtent.height, 1,
              vDevice->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage,
              colorImageMemory);
  colorImageView = createImageView(vDevice->logicalDevice, colorImage,
                                   colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void VulkanSwapChain::createDepthResources() {
  VkFormat depthFormat = vDevice->findDepthFormat();

  createImage(swapChainExtent.width, swapChainExtent.height, 1,
              vDevice->msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage,
              depthImageMemory);
  depthImageView =
      createImageView(vDevice->logicalDevice, depthImage, depthFormat,
                      VK_IMAGE_ASPECT_DEPTH_BIT, mipLevels);
}

void VulkanSwapChain::transitionImageLayout(VkImage image, VkFormat format,
                                            VkImageLayout oldLayout,
                                            VkImageLayout newLayout,
                                            uint32_t mipLevels) {
  VkCommandBuffer commandBuffer =
      vDevice->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevels;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  vDevice->endSingleTimeCommands(commandBuffer, vDevice->queue);
}

void VulkanSwapChain::createImage(uint32_t width, uint32_t height,
                                  uint32_t mipLevels,
                                  VkSampleCountFlagBits numSamples,
                                  VkFormat format, VkImageTiling tiling,
                                  VkImageUsageFlags usage,
                                  VkMemoryPropertyFlags properties,
                                  VkImage &image, VkDeviceMemory &imageMemory) {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = numSamples;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(vDevice->logicalDevice, &imageInfo, nullptr, &image) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(vDevice->logicalDevice, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      vDevice->findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(vDevice->logicalDevice, &allocInfo, nullptr,
                       &imageMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(vDevice->logicalDevice, image, imageMemory, 0);
}

void VulkanSwapChain::setRenderType(RenderType flag) { render_type = flag; }

void VulkanSwapChain::recreateFrameBuffer() {
  create(vDevice->Width(), vDevice->Height(), true);
  // makeSwapchain();
  createImageViews();
  vkDestroyImageView(vDevice->logicalDevice, depthImageView, nullptr);
  vkDestroyImage(vDevice->logicalDevice, depthImage, nullptr);
  vkFreeMemory(vDevice->logicalDevice, depthImageMemory, nullptr);
  vkDestroyImageView(vDevice->logicalDevice, colorImageView, nullptr);
  vkDestroyImage(vDevice->logicalDevice, colorImage, nullptr);
  vkFreeMemory(vDevice->logicalDevice, colorImageMemory, nullptr);
  createDepthResources();
  createColorResources();

  for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
    vkDestroyFramebuffer(vDevice->logicalDevice, swapChainFramebuffers.at(i),
                         nullptr);
  }
  createFramebuffersSW();
}

void VulkanSwapChain::searchSurfaceFormat() {
  // Get available queue family properties
  uint32_t queueCount;
  vkGetPhysicalDeviceQueueFamilyProperties(vDevice->physicalDevice, &queueCount,
                                           NULL);
  assert(queueCount >= 1);

  std::vector<VkQueueFamilyProperties> queueProps(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(vDevice->physicalDevice, &queueCount,
                                           queueProps.data());

  // Iterate over each queue to learn whether it supports presenting:
  // Find a queue with present support
  // Will be used to present the swap chain images to the windowing system
  std::vector<VkBool32> supportsPresent(queueCount);
  for (uint32_t i = 0; i < queueCount; i++) {
    vkGetPhysicalDeviceSurfaceSupportKHR(vDevice->physicalDevice, i,
                                         vDevice->surface, &supportsPresent[i]);
  }

  // Search for a graphics and a present queue in the array of queue
  // families, try to find one that supports both
  uint32_t graphicsQueueNodeIndex = UINT32_MAX;
  uint32_t presentQueueNodeIndex = UINT32_MAX;
  for (uint32_t i = 0; i < queueCount; i++) {
    if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      if (graphicsQueueNodeIndex == UINT32_MAX) {
        graphicsQueueNodeIndex = i;
      }

      if (supportsPresent[i] == VK_TRUE) {
        graphicsQueueNodeIndex = i;
        presentQueueNodeIndex = i;
        break;
      }
    }
  }
  if (presentQueueNodeIndex == UINT32_MAX) {
    // If there's no queue that supports both present and graphics
    // try to find a separate present queue
    for (uint32_t i = 0; i < queueCount; ++i) {
      if (supportsPresent[i] == VK_TRUE) {
        presentQueueNodeIndex = i;
        break;
      }
    }
  }

  // Exit if either a graphics or a presenting queue hasn't been found
  if (graphicsQueueNodeIndex == UINT32_MAX ||
      presentQueueNodeIndex == UINT32_MAX) {
    tools::exitFatal("Could not find a graphics and/or presenting queue!", -1);
  }

  // todo : Add support for separate graphics and presenting queue
  if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
    tools::exitFatal(
        "Separate graphics and presenting queues are not supported yet!", -1);
  }

  queueNodeIndex = graphicsQueueNodeIndex;

  SwapChainSupportDetails swapChainSupport =
      vDevice->querySwapChainSupport(vDevice->getPhysicalDevice());

  surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
}
