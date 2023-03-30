#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <fstream>

#define ASSERT_VULKAN


namespace vkc {
  
  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };
  
  #ifdef NDEBUG
    const bool enableValidationLayers = false;
  #else
    const bool enableValidationLayers = true;
  #endif
  
  VkResult result;
    
  // function
  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
          if (strcmp(layerName, layerProperties.layerName) == 0) {
              layerFound = true;
              break;
          }
      }
      if (!layerFound) return false;
    }

    return true;
  }
  
  // function
  static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    
    file.close();
    return buffer;
  }
  
  // function
  VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code) {
    VkShaderModuleCreateInfo moduleCI;
    moduleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCI.codeSize = code.size();
    moduleCI.pCode = reinterpret_cast<const uint32_t*>(code.data());
        
    VkShaderModule shaderModule;
    vkc::result = vkCreateShaderModule(device, &moduleCI, nullptr, &shaderModule);
    ASSERT_VULKAN(vkc::result);
    
    return shaderModule;
  }
  
  // function
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
        return i;
      }
    }
    
    throw std::runtime_error("failed to find suitable memory type");
  }
  
  
};



