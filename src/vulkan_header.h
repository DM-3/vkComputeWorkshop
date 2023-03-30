#include "vulkan_utils.h"

class VKC {
public:
  void startVulkan()  {}
  void execute()      {}
  void endVulkan()    {}
  
private:
  VkInstance instance                       = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice           = VK_NULL_HANDLE;
  uint32_t queueFamilyIndex                 = 0;
  VkDevice device                           = VK_NULL_HANDLE;
  VkQueue queue                             = VK_NULL_HANDLE;
  VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
  VkDescriptorPool descriptorPool           = VK_NULL_HANDLE;
  VkDescriptorSet descriptorSet             = VK_NULL_HANDLE;
  VkPipeline pipeline                       = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout           = VK_NULL_HANDLE;
  VkCommandPool commandPool                 = VK_NULL_HANDLE;
  VkCommandBuffer commandBuffer             = VK_NULL_HANDLE;
  
  void createInstance()       {}
  void selectPhysicalDevice() {}
  void selectQueueFamily()    {}
  void createLogicalDevice()  {}
  void createPipeline()       {}
  void createCommandPool()    {}
  void createCommandBuffer()  {}
  void recordCommandBuffer()  {}
  
  void getDevice()      {}
  void getQueue()       {}
  void getCommandPool() {}
  
};

VKC::startVulkan() {
  createInstance();
  selectPhysicalDevice();
  selectQueueFamily();
  createLogicalDevice();
  createPipeline();
  createCommandPool();
  createCommandBuffer();
  recordCommandBuffer();
  
}

VKC::endVulkan() {
  
  vkDestroyCommandPool(device, commandPool, nullptr);
  vkDestroyPipeline(device, pipeline, nullptr);
  vkDestroyPipelineLayout(deivce, pipelineLayout, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr); 
}


VKC::createInstance() {
  if (vkc::enableValidationLayers && !vkc::checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }
  
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Compute Shader";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceCI{};
  instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCI.pApplicationInfo = &appInfo;
  instanceCI.enabledExtensionCount = 0;
  instanceCI.ppEnabledExtensionNames = nullptr;
  instanceCI.enabledLayerCount = 0;
  
  if (vkc::enableValidationLayers) {
    instanceCI.enabledLayerCount = static_cast<uint32_t>(vkc::validationLayers.size());
    instanceCI.ppEnabledLayerNames = vkc::validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
  
  vkc::result = vkCreateInstance(&instanceCI, nullptr, &instance);
  ASSERT_VULKAN(vkc::result);
}

VKC::selectPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if(deviceCount == 0) throw std::runtime_error("No GPUs ;_; ");
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  
  physicalDevice = devices[0];
}

VKC::selectQueueFamily() {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
  
  for(int i = 0; i < queueFamilyCount; i++) {
    if(queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      queueFamilyIndex = i;
      break;
    }
  }
}

VKC::createLogicalDevice() {
  VkDeviceQueueCreateInfo queueCI{};
  queueCIsType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCI.queueFamilyIndex = queueFamilyIndex;
  queueCI.queueCount = 1;
  float queuePriority = 1.0f;
  queueCI.pQueuePriorities = &queuePriority;
  
  VkDeviceCreateInfo deviceCI;
  deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCI.queueCreateInfoCount = 1;
  deviceCI.pQueueCreateInfos = &queueCI;
  deviceCI.pEnabledFeatures = &deviceFeatures;
  deviceCI.enabledExtensionCount = 0;
  
  vkc::result = vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device);
  ASSERT_VULKAN(vkc::result);
  
  vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
}

VKC::createPipeline() {
  auto shaderCode = vkc::readFile("shaders/comp.spv");
  VkShaderModule shaderModule = vkc::createShaderModule(shaderCode);
  
  VkPipelineShaderStageCreateInfo shaderStageCI;
  shaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCI.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  shaderStageCI.module = shaderModule;
  shaderStageCI.pName = "main";
  
  VkPipelineLayoutCreateInfo layoutCI{};
  layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutCI.setLayoutCount = 1;
  layoutCI.pSetLayouts = &descriptorSetLayout;
  layoutCI.pushConstantRangeCount = 0;
  
  vkc::result = vkCreatePipelineLayout(device, &layoutCI, nullptr, &pipelineLayout);
  ASSERT_VULKAN(vkc::result);
  
  VkComputePipelineCreateInfo pipelineCI{};
  pipelineCI.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipelineCI.layout = computePipelineLayout;
  pipelineCI.stage = computeShaderStageInfo;
  
  vkc::result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &pipeline);
  ASSERT_VULKAN(vkc::result);
  
  
  vkDestroyShaderModule(device, shaderModule, nullptr);
}

VKC::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCI{};
  commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BUT;
  commandPoolCI.queueFamilyIndex = queueFamilyIndex;
  
  vkc::result = vkCreateCommandPool(device, &commandPoolCI, nullptr, &commandPool);
  ASSERT_VULKAN(vkc::result);
}

VKC::createCommandBuffer() {
  VkCommandBufferAllocateInfo commandBufferAI{};
  commandBufferAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAI.commandPool = commandPool;
  commandBufferAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAI.commandBufferCount = 1;
  
  vkc::result = vkAllocateCommandBuffers(device, &commandBufferAI, &commandBuffer);
  ASSERT_VULKAN(vkc::result);
}

VKC::recordCommandBuffer() {
  VkCommandBufferBeginInfo commandBufferBI{};
  commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBI.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBI.pInheritanceInfo = nullptr;
  
  vkc::result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
  ASSERT_VULKAN(vkc::result);
  
  // recording commands
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout,
    0, 1, &descriptorSet, 0, 0);
  vkCmdDispatch(commandBuffer, 1, 1, 1);      // specify number of workgroups to dispatch
  
  vkc::result = vkEndCommandBuffer(commandBuffer);
  ASSERT_VULKAN(vkc::result);
}





