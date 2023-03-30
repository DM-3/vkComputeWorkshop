#include "vulkan_utils.h"

class VKC {
public:
  void startVulkan()  {}
  void execute()      {}
  void endVulkan()    {}
  
private:
  VkInstance instance             = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device                 = VK_NULL_HANDLE;
  
  void createInstance()       {}
  void selectPhysicalDevice() {}
  void createDevice()         {}
  void createQueue()          {}
  void createCommandPool()    {}
  
  void getDevice()      {}
  void getQueue()       {}
  void getCommandPool() {}
  
};

VKC::startVulkan() {
  createInstance();
  selectPhysicalDevice();
  
}

VKC::endVulkan() {
  
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
  
  
}






