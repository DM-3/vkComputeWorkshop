#pragma once
#include "vulkan_utils.h"

class VKC {
public:
    // important stuff
    void startVulkan();
    void execute();
    void endVulkan();

    // getters/setters
    VkDevice        getDevice();
    VkDeviceMemory  getInputMemory();
    VkDeviceMemory  getOutputMemory();

    void      setInputSize(uint32_t);
    uint32_t  getInputSize();
    void      setOutputSize(uint32_t);
    uint32_t  getOutputSize();

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex = 0;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    uint32_t inputSize = 0;
    VkBuffer inputBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputBufferMemory = VK_NULL_HANDLE;
    uint32_t outputSize = 0;
    VkBuffer outputBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputBufferMemory = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    void createInstance();
    void selectPhysicalDevice();
    void selectQueueFamily();
    void createLogicalDevice();

    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createBuffers();
    void createDescriptorSet();

    void createPipeline();
    void createCommandPool();
    void createCommandBuffer();
    void recordCommandBuffer();
};

// public functions
void VKC::startVulkan() {
  createInstance();
  selectPhysicalDevice();
  selectQueueFamily();
  createLogicalDevice();
  
  createDescriptorSetLayout();
  createDescriptorPool();
  createBuffers();
  createDescriptorSet();
  
  createPipeline();
  createCommandPool();
  createCommandBuffer();
  recordCommandBuffer();
  
}

void VKC::execute() {
  VkFence fence;
  VkFenceCreateInfo fenceCI{};
  fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  vkCreateFence(device, &fenceCI, nullptr, &fence);
  
  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &commandBuffer;
  
  vkc::result = vkQueueSubmit(queue, 1, &submit, fence);
  ASSERT_VULKAN(vkc::result);
  
  vkc::result = vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
  ASSERT_VULKAN(vkc::result);
  
  vkDestroyFence(device, fence, nullptr);
}

void VKC::endVulkan() {
  
  vkDestroyBuffer(device, inputBuffer, nullptr);
  vkFreeMemory(device, inputBufferMemory, nullptr);
  vkDestroyBuffer(device, outputBuffer, nullptr);  
  vkFreeMemory(device, outputBufferMemory, nullptr);
  vkDestroyDescriptorPool(device, descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
  
  vkDestroyCommandPool(device, commandPool, nullptr);
  vkDestroyPipeline(device, pipeline, nullptr);
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr); 
}


// getter/setter functions
VkDevice        VKC::getDevice()        { return device; }
VkDeviceMemory  VKC::getInputMemory()   { return inputBufferMemory; }
VkDeviceMemory  VKC::getOutputMemory()  { return outputBufferMemory; }
void      VKC::setInputSize(uint32_t val)   { inputSize = val; }
uint32_t  VKC::getInputSize()               { return inputSize; }
void      VKC::setOutputSize(uint32_t val)  { outputSize = val; }
uint32_t  VKC::getOutputSize()              { return outputSize; }


// private functions
void VKC::createInstance() {
  if (vkc::enableValidationLayers && !vkc::checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }
  
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Compute Shader";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

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
    instanceCI.enabledLayerCount = 0;
  }
  
  vkc::result = vkCreateInstance(&instanceCI, nullptr, &instance);
  ASSERT_VULKAN(vkc::result);
}

void VKC::selectPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if(deviceCount == 0) throw std::runtime_error("No GPUs ;_; ");
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  
  physicalDevice = devices[0];
}

void VKC::selectQueueFamily() {
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

void VKC::createLogicalDevice() {
    VkDeviceQueueCreateInfo queueCI{};
    queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCI.queueFamilyIndex = queueFamilyIndex;
    queueCI.queueCount = 1;
    float queuePriority = 1.0f;
    queueCI.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCI{};
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCI.queueCreateInfoCount = 1;
    deviceCI.pQueueCreateInfos = &queueCI;
    deviceCI.pEnabledFeatures = &deviceFeatures;
    deviceCI.enabledExtensionCount = 0;

    vkc::result = vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device);
    ASSERT_VULKAN(vkc::result);

    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
}

void VKC::createDescriptorSetLayout() {
  std::vector<VkDescriptorSetLayoutBinding> layoutBindings(2);
  
  layoutBindings[0].binding = 0;
  layoutBindings[0].descriptorCount = 1;
  layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBindings[0].pImmutableSamplers = nullptr;
  layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  
  layoutBindings[1].binding = 1;
  layoutBindings[1].descriptorCount = 1;
  layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBindings[1].pImmutableSamplers = nullptr;
  layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  
  VkDescriptorSetLayoutCreateInfo layoutCI{};
  layoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutCI.bindingCount = layoutBindings.size();
  layoutCI.pBindings = layoutBindings.data();
  
  vkc::result = vkCreateDescriptorSetLayout(device, &layoutCI, nullptr, &descriptorSetLayout);
  ASSERT_VULKAN(vkc::result);
}

void VKC::createDescriptorPool() {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSize.descriptorCount = 2;
  
  VkDescriptorPoolCreateInfo poolCI{};
  poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolCI.poolSizeCount = 1;
  poolCI.pPoolSizes = &poolSize;
  poolCI.maxSets = 1;
  
  vkc::result = vkCreateDescriptorPool(device, &poolCI, nullptr, &descriptorPool);
  ASSERT_VULKAN(vkc::result);
}

void VKC::createBuffers() {
  vkc::createBuffer(device, physicalDevice, &inputBuffer, &inputBufferMemory, inputSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  vkc::createBuffer(device, physicalDevice, &outputBuffer, &outputBufferMemory, outputSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

void VKC::createDescriptorSet() {
  VkDescriptorSetAllocateInfo descriptorSetAI{};
  descriptorSetAI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAI.descriptorPool = descriptorPool;
  descriptorSetAI.descriptorSetCount = 1;
  descriptorSetAI.pSetLayouts = &descriptorSetLayout;
  
  vkc::result = vkAllocateDescriptorSets(device, &descriptorSetAI, &descriptorSet);
  ASSERT_VULKAN(vkc::result);
  
  
  std::vector<VkDescriptorBufferInfo> bufferInfos(2);
  
  bufferInfos[0].buffer = inputBuffer;
  bufferInfos[0].offset = 0;
  bufferInfos[0].range = inputSize;
  
  bufferInfos[1].buffer = outputBuffer;
  bufferInfos[1].offset = 0;
  bufferInfos[1].range = outputSize;
  
  
  std::vector<VkWriteDescriptorSet> descriptorWrites(2);
  
  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].dstSet = descriptorSet;
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pBufferInfo = &bufferInfos[0];
  
  descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[1].dstSet = descriptorSet;
  descriptorWrites[1].dstBinding = 1;
  descriptorWrites[1].dstArrayElement = 0;
  descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  descriptorWrites[1].descriptorCount = 1;
  descriptorWrites[1].pBufferInfo = &bufferInfos[1];
  
  vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void VKC::createPipeline() {
  auto shaderCode = vkc::readFile("shaders/comp.spv");
  VkShaderModule shaderModule = vkc::createShaderModule(device, shaderCode);
  
  VkPipelineShaderStageCreateInfo shaderStageCI{};
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
  pipelineCI.layout = pipelineLayout;
  pipelineCI.stage = shaderStageCI;
  
  vkc::result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &pipeline);
  ASSERT_VULKAN(vkc::result);
  
  
  vkDestroyShaderModule(device, shaderModule, nullptr);
}

void VKC::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCI{};
  commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCI.queueFamilyIndex = queueFamilyIndex;
  
  vkc::result = vkCreateCommandPool(device, &commandPoolCI, nullptr, &commandPool);
  ASSERT_VULKAN(vkc::result);
}

void VKC::createCommandBuffer() {
  VkCommandBufferAllocateInfo commandBufferAI{};
  commandBufferAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAI.commandPool = commandPool;
  commandBufferAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAI.commandBufferCount = 1;
  
  vkc::result = vkAllocateCommandBuffers(device, &commandBufferAI, &commandBuffer);
  ASSERT_VULKAN(vkc::result);
}

void VKC::recordCommandBuffer() {
  VkCommandBufferBeginInfo commandBufferBI{};
  commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBI.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBI.pInheritanceInfo = nullptr;
  
  vkc::result = vkBeginCommandBuffer(commandBuffer, &commandBufferBI);
  ASSERT_VULKAN(vkc::result);
  
  // recording commands
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout,
    0, 1, &descriptorSet, 0, 0);
  vkCmdDispatch(commandBuffer, 1, 1, 1);      // specify number of workgroups to dispatch
  
  vkc::result = vkEndCommandBuffer(commandBuffer);
  ASSERT_VULKAN(vkc::result);
}





