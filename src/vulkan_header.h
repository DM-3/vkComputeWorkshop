#include "vulkan_utils.h"

class VKC {
public:
  void startVulkan()  {}
  void execute()      {}
  void endVulkan()    {}
  
private:
  VkInstance instance;
  VkDevice device;
  
  void createInstance()     {}
  void createDevice()       {}
  void createQueue()        {}
  void createCommandPool()  {}
  
  void getDevice()      {}
  void getQueue()       {}
  void getCommandPool() {}
  
};

VKC::createInstance() {
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
  
  
}






