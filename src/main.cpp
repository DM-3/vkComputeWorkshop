#include "vulkan_header.h"
#define INPUT_COUNT 128
#define OUTPUT_COUNT 128


int main() {
  
  float arr_in[INPUT_COUNT];
  float arr_out[OUTPUT_COUNT];
  
  
  VKC comp;
  
  comp.setInputSize(INPUT_COUNT * sizeof(float));
  comp.setOutputSize(OUTPUT_COUNT * sizeof(float));
  
  comp.startVulkan();
  
  void* data;
  vkMapMemory(comp.getDevice(), comp.getInputMemory(), 0, comp.getInputSize(), 0, &data);
  memcpy(data, arr_in, INPUT_COUNT * sizeof(float));
  vkUnmapMemory(comp.getDevice(), comp.getInputMemory());
  
  comp.execute();
  
  vkMapMemory(comp.getDevice(), comp.getOutputMemory(), 0, comp.getOuptutSize(), 0, &data);
  memcpy(data, arr_out, OUTPUT_COUNT * sizeof(float));
  vkUnmapMemory(comp.getDevice(), comp.getOutputMemory());
  
  comp.endVulkan();
  
  return 1;
}
