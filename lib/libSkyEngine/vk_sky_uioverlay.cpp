#include <SkyEngine/vk_sky_uioverlay.hpp>
#include "tools.hpp"

#ifdef GLFW_LIB_ENABLE
namespace gui
{
  UIOverlay::UIOverlay()
  {
    get_static(this);

    IMGUI_CHECKVERSION();
    // Init ImGui
    ImGui::CreateContext();
    // Color scheme
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::StyleColorsClassic();
    // style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
    // style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    // style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.0f, 0.0f, 0.1f);
    // style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.8f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
    // style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    // style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    // style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    // style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    // style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    // style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    // style.ItemSpacing = ImVec2(1,1);

    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    fonts = get_fonts();
    if (!fonts.empty())
    {
      for (auto &font_name : fonts)
        io.Fonts->AddFontFromFileTTF(font_name.data(), 24.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
      io.FontDefault = io.Fonts->Fonts[0];
    }
    // io.DisplaySize = ImVec2(width, height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.FontGlobalScale = scale;

    // console parametrs
    ClearLog();
    memset(InputBuf, 0, sizeof(InputBuf));
    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    // Commands.push_back("CLASSIFY");
    AutoScroll = true;
    ScrollToBottom = false;
    AddLog(u8"Логирование: ");

    // io.KeysDown[io.KeyMap[ImGuiKey_Backspace]] = GLFW_KEY_BACKSPACE;
  }

  UIOverlay::~UIOverlay()
  {
  }

  void UIOverlay::preparePipeline(const VkPipelineCache pipelineCache, const VkRenderPass renderPass,
                                  const VulkanSwapChain *swapchain, const VkFormat depthFormat)
  {
    // Pipeline layout
    // Push constants for UI rendering parameters
    VkPushConstantRange pushConstantRange = initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT,
                                                                            sizeof(PushConstBlock), 0);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = initializers::pipelineLayoutCreateInfo(
        &descriptorSetLayout, 1);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device->logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout))
    {
      throw std::runtime_error("failed to create pipeline layout!");
    }

    // Setup graphics pipeline for UI rendering
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    // VkPipelineRasterizationStateCreateInfo rasterizationState =
    //     initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;

    // Enable blending
    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_ALWAYS);

    VkPipelineViewportStateCreateInfo viewportState =
        initializers::pipelineViewportStateCreateInfo(1, 1, 0);

    VkPipelineMultisampleStateCreateInfo multisampleState{}; //=
    // initializers::pipelineMultisampleStateCreateInfo(rasterizationSamples);
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // multisampling.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = device->msaaSamples;

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState =
        initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = initializers::pipelineCreateInfo(pipelineLayout, renderPass);

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaders.size());
    pipelineCreateInfo.pStages = shaders.data();
    pipelineCreateInfo.subpass = subpass;

#if defined(VK_KHR_dynamic_rendering)
    // SRS - if we are using dynamic rendering (i.e. renderPass null), must define color, depth and stencil attachments at pipeline create time
    VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
    if (renderPass == VK_NULL_HANDLE)
    {
      pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
      pipelineRenderingCreateInfo.colorAttachmentCount = 1;
      pipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchain->swapChainImageFormat;
      pipelineRenderingCreateInfo.depthAttachmentFormat = depthFormat;
      pipelineRenderingCreateInfo.stencilAttachmentFormat = depthFormat;
      pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
    }
#endif

    // Vertex bindings an attributes based on ImGui vertex definition
    std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
        initializers::vertexInputBindingDescription(0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX),
    };
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
        initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT,
                                                      offsetof(ImDrawVert, pos)), // Location 0: Position
        initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT,
                                                      offsetof(ImDrawVert, uv)), // Location 1: UV
        initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R8G8B8A8_UNORM,
                                                      offsetof(ImDrawVert, col)), // Location 0: Color
    };
    VkPipelineVertexInputStateCreateInfo vertexInputState = initializers::pipelineVertexInputStateCreateInfo();
    vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
    vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputState;

    if (vkCreateGraphicsPipelines(device->logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr,
                                  &pipeline))
    {
      throw std::runtime_error("failed to create graphics pipeline!");
    }
  }
#ifdef GLFW_LIB_ENABLE
  void UIOverlay::prepareResources(GLFWwindow *window)
  {
    // add ui callbacks
    if (window != nullptr)
      ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGuiIO &io = ImGui::GetIO();
    // Create font texture
    unsigned char *fontData = nullptr;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
    VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

    // SRS - Set ImGui style scale factor to handle retina and other HiDPI displays (same as font scaling above)
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);

    // Create target image for copy
    VkImageCreateInfo imageInfo = initializers::imageCreateInfo();
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.extent.width = texWidth;
    imageInfo.extent.height = texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (vkCreateImage(device->logicalDevice, &imageInfo, nullptr, &fontImage))
    {
      throw std::runtime_error("failed to create image!");
    }
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device->logicalDevice, fontImage, &memReqs);
    VkMemoryAllocateInfo memAllocInfo = initializers::memoryAllocateInfo();
    memAllocInfo.allocationSize = memReqs.size;
    memAllocInfo.memoryTypeIndex = device->getMemoryType(memReqs.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (vkAllocateMemory(device->logicalDevice, &memAllocInfo, nullptr, &fontMemory))
    {
      throw std::runtime_error("failed to create memory!");
    }
    if (vkBindImageMemory(device->logicalDevice, fontImage, fontMemory, 0))
    {
      throw std::runtime_error("failed to create image memory!");
    }

    // Image view
    VkImageViewCreateInfo viewInfo = initializers::imageViewCreateInfo();
    viewInfo.image = fontImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device->logicalDevice, &viewInfo, nullptr, &fontView))
    {
      throw std::runtime_error("failed to create image view!");
    }

    // Staging buffers for font data upload
    enma::Buffer stagingBuffer;

    device->createBuffer(uploadSize,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &stagingBuffer);

    stagingBuffer.map();
    memcpy(stagingBuffer.mapped, fontData, uploadSize);
    stagingBuffer.unmap();

    // Copy buffer data to font image
    VkCommandBuffer copyCmd = device->beginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    // Prepare for transfer
    tools::setImageLayout(
        copyCmd,
        fontImage,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_HOST_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT);

    // Copy
    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = texWidth;
    bufferCopyRegion.imageExtent.height = texHeight;
    bufferCopyRegion.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        copyCmd,
        stagingBuffer.buffer,
        fontImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &bufferCopyRegion);

    // Prepare for shader read
    tools::setImageLayout(
        copyCmd,
        fontImage,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    device->endSingleTimeCommands(copyCmd, queue, true);

    stagingBuffer.destroy();

    // Font texture Sampler
    VkSamplerCreateInfo samplerInfo = initializers::samplerCreateInfo();
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    if (vkCreateSampler(device->logicalDevice, &samplerInfo, nullptr, &sampler))
    {
      throw std::runtime_error("failed to create sampler!");
    }

    // Descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes = {
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)};
    VkDescriptorPoolCreateInfo descriptorPoolInfo = initializers::descriptorPoolCreateInfo(poolSizes, 2);
    if (vkCreateDescriptorPool(device->logicalDevice, &descriptorPoolInfo, nullptr, &descriptorPool))
    {
      throw std::runtime_error("failed to create descriptor pool!");
    }

    // Descriptor set layout
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                 VK_SHADER_STAGE_FRAGMENT_BIT, 0),
    };
    VkDescriptorSetLayoutCreateInfo descriptorLayout = initializers::descriptorSetLayoutCreateInfo(
        setLayoutBindings);
    if (vkCreateDescriptorSetLayout(device->logicalDevice, &descriptorLayout, nullptr, &descriptorSetLayout))
    {
      throw std::runtime_error("failed to create descriptor layout!");
    }

    // Descriptor set
    VkDescriptorSetAllocateInfo allocInfo = initializers::descriptorSetAllocateInfo(descriptorPool,
                                                                                    &descriptorSetLayout, 1);
    if (vkAllocateDescriptorSets(device->logicalDevice, &allocInfo, &descriptorSet))
    {
      throw std::runtime_error("failed to create allocate descriptors!");
    }
    VkDescriptorImageInfo fontDescriptor = initializers::descriptorImageInfo(
        sampler,
        fontView,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        initializers::createVkWriteDescriptorTexture(0, &fontDescriptor, descriptorSet)};
    vkUpdateDescriptorSets(device->logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()),
                           writeDescriptorSets.data(), 0, nullptr);
  }
  #endif // GLFW_LIB_ENABLE

  bool UIOverlay::update()
  {
    ImDrawData *imDrawData = ImGui::GetDrawData();
    bool updateCmdBuffers = false;

    if (!imDrawData)
    {
      return false;
    };

    // Note: Alignment is done inside buffer creation
    VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if ((vertexBufferSize == 0) || (indexBufferSize == 0))
    {
      return false;
    }

    // Vertex buffer
    if ((vertexBuffer.buffer == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount))
    {
      vertexBuffer.unmap();
      vertexBuffer.destroy();
      if (device->createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &vertexBuffer) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create buffer!");
      }
      vertexCount = imDrawData->TotalVtxCount;
      vertexBuffer.unmap();
      vertexBuffer.map();
      updateCmdBuffers = true;
    }

    // Index buffer
    if ((indexBuffer.buffer == VK_NULL_HANDLE) || (indexCount < imDrawData->TotalIdxCount))
    {
      indexBuffer.unmap();
      indexBuffer.destroy();
      if (device->createBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &indexBuffer) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create buffer!");
      }
      indexCount = imDrawData->TotalIdxCount;
      indexBuffer.map();
      updateCmdBuffers = true;
    }

    // Upload data
    ImDrawVert *vtxDst = (ImDrawVert *)vertexBuffer.mapped;
    ImDrawIdx *idxDst = (ImDrawIdx *)indexBuffer.mapped;

    for (int n = 0; n < imDrawData->CmdListsCount; n++)
    {
      const ImDrawList *cmd_list = imDrawData->CmdLists[n];
      memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
      memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
      vtxDst += cmd_list->VtxBuffer.Size;
      idxDst += cmd_list->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU
    vertexBuffer.flush();
    indexBuffer.flush();

    return updateCmdBuffers;
  }

  void UIOverlay::draw(const VkCommandBuffer commandBuffer)
  {
    ImDrawData *imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
    {
      return;
    }

    ImGuiIO &io = ImGui::GetIO();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0,
                            NULL);

    pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
    pushConstBlock.translate = glm::vec2(-1.0f);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock),
                       &pushConstBlock);

    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
    {
      const ImDrawList *cmd_list = imDrawData->CmdLists[i];
      for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
      {
        const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
        VkRect2D scissorRect;
        scissorRect.offset.x = (std::max)((int32_t)(pcmd->ClipRect.x), 0);
        scissorRect.offset.y = (std::max)((int32_t)(pcmd->ClipRect.y), 0);
        scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
        scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
        vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
        indexOffset += pcmd->ElemCount;
      }
      vertexOffset += cmd_list->VtxBuffer.Size;
    }
  }

  void UIOverlay::resize(uint32_t width, uint32_t height)
  {
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)(width), (float)(height));
  }

  void UIOverlay::freeResources()
  {
#ifdef GLFW_LIB_ENABLE
    ImGui_ImplGlfw_Shutdown();
#endif
    if (ImGui::GetCurrentContext())
    {
      ImGui::DestroyContext();
    }

    vertexBuffer.destroy();
    indexBuffer.destroy();
    vkDestroyShaderModule(device->logicalDevice, module_vert, nullptr);
    vkDestroyShaderModule(device->logicalDevice, module_frag, nullptr);

    vkDestroyImageView(device->logicalDevice, fontView, nullptr);
    vkDestroyImage(device->logicalDevice, fontImage, nullptr);
    vkFreeMemory(device->logicalDevice, fontMemory, nullptr);
    vkDestroySampler(device->logicalDevice, sampler, nullptr);
    vkDestroyDescriptorSetLayout(device->logicalDevice, descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device->logicalDevice, descriptorPool, nullptr);
    vkDestroyPipelineLayout(device->logicalDevice, pipelineLayout, nullptr);
    vkDestroyPipeline(device->logicalDevice, pipeline, nullptr);
  }

  bool UIOverlay::header(const char *caption)
  {
    return ImGui::CollapsingHeader(caption, ImGuiTreeNodeFlags_DefaultOpen);
  }

  bool UIOverlay::checkBox(const char *caption, bool *value)
  {
    bool res = ImGui::Checkbox(caption, value);
    if (res)
    {
      updated = true;
    };
    return res;
  }

  bool UIOverlay::checkBox(const char *caption, int32_t *value)
  {
    bool val = (*value == 1);
    bool res = ImGui::Checkbox(caption, &val);
    *value = val;
    if (res)
    {
      updated = true;
    };
    return res;
  }

  bool UIOverlay::radioButton(const char *caption, bool value)
  {
    bool res = ImGui::RadioButton(caption, value);
    if (res)
    {
      updated = true;
    };
    return res;
  }

  bool UIOverlay::inputFloat(const char *caption, float *value, float step, uint32_t precision)
  {
    bool res = ImGui::InputFloat(caption, value, step, step * 10.0f, "%.3f");
    if (res)
    {
      updated = true;
    };
    return res;
  }

  bool UIOverlay::inputFloat3(const char *caption, float *value, uint32_t precision)
  {
    return ImGui::InputFloat3(caption, value, "%.3f");
  }

  bool UIOverlay::inputText(const char *caption, std::string *str, ImGuiInputTextFlags flags,
                            ImGuiInputTextCallback callback, void *user_data)
  {
    return ImGui::InputText(caption, str, flags, callback, user_data);
  }

  bool UIOverlay::inputInt(const char *caption, int *value)
  {
    return ImGui::InputInt(caption, value);
  }

  bool UIOverlay::inputTable(const char *str_id, int column, ImGuiTableFlags flags, const ImVec2 &outer_size,
                             float inner_width)
  {
    if (ImGui::BeginTable(str_id, column, flags, outer_size, inner_width))
    {
      for (int row = 0; row < column; ++row)
      {
        ImGui::TableNextRow();
        for (int col = 0; col < column; ++col)
        {
          ImGui::TableSetColumnIndex(col);
          ImGui::Text("Row %d Column %d", row, col);
        }
        ImGui::TableHeader("1");
      }
      ImGui::EndTable();
    }
    return true;
  }

  // bool UIOverlay::sliderFloat(const char *caption, float *value, float min, float max)
  // {
  //     bool res = ImGui::SliderFloat(caption, value, min, max, "%.3f");
  //     if (res)
  //     {
  //         updated = true;
  //     };
  //     return res;
  // }

  // bool UIOverlay::sliderInt(const char *caption, int *value, int min, int max)
  // {
  //     bool res = ImGui::SliderInt(caption, value, min, max, "%3");
  //     if (res)
  //     {
  //         updated = true;
  //     };
  //     return res;
  // }

  bool UIOverlay::comboBox(const char *caption, int32_t *itemindex, std::vector<std::string> items)
  {
    if (items.empty())
    {
      return false;
    }
    std::vector<const char *> charitems;
    charitems.reserve(items.size());
    for (size_t i = 0; i < items.size(); i++)
    {
      charitems.push_back(items[i].c_str());
    }
    uint32_t itemCount = static_cast<uint32_t>(charitems.size());
    bool res = ImGui::Combo(caption, itemindex, &charitems[0], itemCount, itemCount);
    if (res)
    {
      updated = true;
    };
    return res;
  }

  bool UIOverlay::button(const char *caption)
  {
    bool res = ImGui::Button(caption);
    if (res)
    {
      updated = true;
    };
    return res;
  }

  void UIOverlay::text(const char *formatstr, ...)
  {
    va_list args;
    va_start(args, formatstr);
    ImGui::TextV(formatstr, args);
    va_end(args);
  }

  void UIOverlay::colorEdit(const char *caption, float *color)
  {
    ImGui::ColorEdit4(caption, color);
  }

  void UIOverlay::showFontSelector(const char *label)
  {
    ImGui::ShowFontSelector(label);
  }

  void gui::UIOverlay::showStyleSelector(const char *label)
  {
    ImGui::ShowStyleSelector(label);
  }

  void gui::UIOverlay::setSeparator()
  {
    ImGui::Separator();
  }

  bool
  UIOverlay::setListBox(const char *label, int *current_item, std::vector<std::string> &items, int height_in_items)
  {
    return ImGui::ListBox(
        label, current_item,
        [](void *data, int idx, const char **out_text)
        {
          std::vector<std::string> *vector = reinterpret_cast<std::vector<std::string> *>(data);
          if (idx < 0 || idx >= vector->size())
            return false;
          *out_text = vector->at(idx).c_str();
          return true;
        },
        (void *)&items, items.size(), height_in_items);
  }

  void UIOverlay::setConsoleLog(const char *label)
  {
    drawConsole(label, &show_console);
  }

  void UIOverlay::setInSameLine(float pos_x, float spacing_w)
  {
    ImGui::SameLine(pos_x, spacing_w);
  }

  void UIOverlay::on_off_console()
  {
    show_console = !show_console;
  }

  void UIOverlay::AddLog(const char *fmt, ...)
  {
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    Items.push_back(Strdup(buf));
  }

  void UIOverlay::increaseSubPass()
  {
    subpass++;
  }

  bool UIOverlay::inputFloat4(const char *caption, float *value, uint32_t precision)
  {
    return ImGui::InputFloat4(caption, value, "%.3f");
  }

  bool UIOverlay::sliderFloat(const char *caption, float *value, float min, float max)
  {
    return false;
  }
}
#endif // GLFW_LIB_ENABLE