#pragma once

#include <cstddef>
#ifdef GLFW_LIB_ENABLE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#else
#endif

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SkyEngine/config/config.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <regex>

// #define LIBGOSYS

//#ifdef LIBGOSYS
//#include <libgosys.h>
//#endif

#ifdef __linux__

#include <dirent.h>

#endif

#ifdef __MINGW32__
#include <dirent.h>
#endif

#ifdef __MINGW64__
#include <dirent.h>
#endif

#include "initializer.hpp"
#include "frustum.hpp"
#include "coloriser.h"

const std::vector<const char *> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char *> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Forward declaration for tools::errorString
namespace tools {
  auto errorString(VkResult errorCode) -> std::string;
}

constexpr void VK_CHECK_RESULT(VkResult res, const char* file = __FILE__, int line = __LINE__) {
    if (res != VK_SUCCESS) {
        std::cout << "Fatal : VkResult is \"" << tools::errorString(res) << "\" in " << file << " at line " << line << std::endl;
        assert(res == VK_SUCCESS);
    }
}

#define VK_FLAGS_NONE 0

#define DEFAULT_FENCE_TIMEOUT 100000000000

///////////////////// OUTPUT DEFINED
///
///
#define OUT_MOD
//////////////////// END

template<class T>
struct optional {
  auto value() -> T {
    return _value;
  };

  auto operator!=(optional elem) -> bool {
    return _value != elem.value();
  };

  auto operator=(T _val) -> optional & {
    _value = _val;
    isValue = true;
    return *this;
  }

  bool has_value() {
    return isValue;
  };

private:
  bool isValue;
  T _value;
};

struct QueueFamilyIndices {
  optional<uint32_t> graphicsFamily;
  optional<uint32_t> presentFamily;

  auto isComplete() -> bool {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};


static auto
CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                              const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) -> VkResult {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }  
    return VK_ERROR_EXTENSION_NOT_PRESENT;
 
}

static void _DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                          "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

static VkImageView
createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                VkImageViewType _type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.pNext = nullptr;
  viewInfo.image = image;
  viewInfo.viewType = _type;
  viewInfo.format = format;
  viewInfo.components = {
    VK_COMPONENT_SWIZZLE_R,
    VK_COMPONENT_SWIZZLE_G,
    VK_COMPONENT_SWIZZLE_B,
    VK_COMPONENT_SWIZZLE_A
  };
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

  return imageView;
}

static std::vector<std::string> get_fonts() {
  std::vector<std::string> fonts;
  const std::string path = std::string(DATA_DIRECTORY) + "/fonts";
  std::ifstream test_dir(path);
  if (!test_dir.good()) {
    std::cerr << "Couldn't open directory " << path << std::endl;
    return fonts;
  }
  test_dir.close();

#if defined(_WIN32)
  WIN32_FIND_DATAA findFileData;
  HANDLE hFind = FindFirstFileA((path + "/*").c_str(), &findFileData);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        fonts.emplace_back(path + "/" + findFileData.cFileName);
      }
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
  }
#else
  DIR *dir = opendir(path.c_str());
  if (dir != nullptr) {
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
      if (ent->d_type != DT_DIR && std::string(ent->d_name) != "." && std::string(ent->d_name) != "..") {
        fonts.emplace_back(path + "/" + ent->d_name);
      }
    }
    closedir(dir);
  }
#endif
  return fonts;
}

namespace tools {

  inline std::vector<std::string> split(std::string stringToBeSplitted, std::string delimeter) {
    std::vector<std::string> splittedString;
    int startIndex = 0;
    int endIndex = 0;
    while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size()) {
      std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
      splittedString.push_back(val);
      startIndex = endIndex + delimeter.size();
    }
    if (startIndex < stringToBeSplitted.size()) {
      std::string val = stringToBeSplitted.substr(startIndex);
      splittedString.push_back(val);
    }
    return splittedString;
  }

  inline void setImageLayout(
    VkCommandBuffer cmdbuffer,
    VkImage image,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkImageSubresourceRange subresourceRange,
    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT) {
    // Create an image barrier object
    VkImageMemoryBarrier imageMemoryBarrier = initializers::imageMemoryBarrier();
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    // Source layouts (old)
    // Source access mask controls actions that have to be finished on the old layout
    // before it will be transitioned to the new layout
    switch (oldImageLayout) {
      case VK_IMAGE_LAYOUT_UNDEFINED:
        // Image layout is undefined (or does not matter)
        // Only valid as initial layout
        // No flags required, listed only for completeness
        imageMemoryBarrier.srcAccessMask = 0;
        break;

      case VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Image is preinitialized
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image is a color attachment
        // Make sure any writes to the color buffer have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        // Image is a depth/stencil attachment
        // Make sure any writes to the depth/stencil buffer have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image is a transfer source
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

      case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image is a transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image is read by a shader
        // Make sure any shader reads from the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
      default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (newImageLayout) {
      case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image will be used as a transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image will be used as a transfer source
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

      case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image will be used as a color attachment
        // Make sure any writes to the color buffer have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        // Image layout will be used as a depth/stencil attachment
        // Make sure any writes to depth/stencil buffer have been finished
        imageMemoryBarrier.dstAccessMask =
            imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

      case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image will be read in a shader (sampler, input attachment)
        // Make sure any writes to the image have been finished
        if (imageMemoryBarrier.srcAccessMask == 0) {
          imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
      default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
      cmdbuffer,
      srcStageMask,
      dstStageMask,
      0,
      0, nullptr,
      0, nullptr,
      1, &imageMemoryBarrier);
  }

  inline VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat) {
    // Since all depth formats may be optional, we need to find a suitable depth format to use
    // Start with the highest precision packed format
    std::vector<VkFormat> depthFormats = {
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      VK_FORMAT_D32_SFLOAT,
      VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_D16_UNORM_S8_UINT,
      VK_FORMAT_D16_UNORM
    };

    for (auto &format: depthFormats) {
      VkFormatProperties formatProps;
      vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
      // Format must support depth stencil attachment for optimal tiling
      if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        *depthFormat = format;
        return true;
      }
    }

    return false;
  }

  inline std::vector<VkAttachmentReference> set_color_attachments_reference() {
    size_t attach_count = 1;
    std::vector<VkAttachmentReference> color_reference_vector{};
    color_reference_vector.resize(attach_count);
    for (size_t attach = 0; attach < color_reference_vector.size(); attach++) {
      color_reference_vector.at(attach).attachment = attach;
      color_reference_vector.at(attach).layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    return color_reference_vector;
  }

  inline VkBool32 formatHasStencil(VkFormat format) {
    std::vector<VkFormat> stencilFormats = {
      VK_FORMAT_S8_UINT,
      VK_FORMAT_D16_UNORM_S8_UINT,
      VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_D32_SFLOAT_S8_UINT,
    };
    return std::find(stencilFormats.begin(), stencilFormats.end(), format) != std::end(stencilFormats);
  }

  // Returns if a given format support LINEAR filtering
  inline VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

    if (tiling == VK_IMAGE_TILING_OPTIMAL)
      return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    if (tiling == VK_IMAGE_TILING_LINEAR)
      return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    return false;
  }

  inline void setImageLayout(
    VkCommandBuffer cmdbuffer,
    VkImage image,
    VkImageAspectFlags aspectMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT) {
    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.layerCount = 1;
    setImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
  }

  inline bool fileExists(const std::string &filename) {
    std::ifstream file(filename.c_str());
    return !file.fail();
  }

  inline void exitFatal(const std::string &message, int32_t exitCode) {
#if defined(_WIN32)
    std::cerr << message << std::endl;
#elif defined(__ANDROID__)
    LOGE("Fatal error: %s", message.c_str());
    vks::android::showAlert(message.c_str());
#endif
    std::cerr << message << "\n";
#if !defined(__ANDROID__)
    exit(exitCode);
#endif
  }

  inline void exitFatal(const std::string &message, VkResult resultCode) {
    exitFatal(message, (int32_t) resultCode);
  }

  /**
   * @brief vulkan error code
   */
  inline std::string errorString(VkResult errorCode) {
    switch (errorCode) {
#define STR(r)   \
    case VK_##r: \
        return #r
      STR(NOT_READY);
      STR(TIMEOUT);
      STR(EVENT_SET);
      STR(EVENT_RESET);
      STR(INCOMPLETE);
      STR(ERROR_OUT_OF_HOST_MEMORY);
      STR(ERROR_OUT_OF_DEVICE_MEMORY);
      STR(ERROR_INITIALIZATION_FAILED);
      STR(ERROR_DEVICE_LOST);
      STR(ERROR_MEMORY_MAP_FAILED);
      STR(ERROR_LAYER_NOT_PRESENT);
      STR(ERROR_EXTENSION_NOT_PRESENT);
      STR(ERROR_FEATURE_NOT_PRESENT);
      STR(ERROR_INCOMPATIBLE_DRIVER);
      STR(ERROR_TOO_MANY_OBJECTS);
      STR(ERROR_FORMAT_NOT_SUPPORTED);
      STR(ERROR_SURFACE_LOST_KHR);
      STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
      STR(SUBOPTIMAL_KHR);
      STR(ERROR_OUT_OF_DATE_KHR);
      STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
      STR(ERROR_VALIDATION_FAILED_EXT);
      STR(ERROR_INVALID_SHADER_NV);
#undef STR
      default:
        return "UNKNOWN_ERROR";
    }
  }

  inline VkPipelineShaderStageCreateInfo
  LoadShader(VkDevice _device, const std::string &filename, VkShaderStageFlagBits stage, VkShaderModule &module) {
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
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = stage;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "main";

    module = shaderModule;

    return vertShaderStageInfo;
  }
}
