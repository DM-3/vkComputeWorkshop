#include "vulkan_header.h"
#define INPUT_SIZE 128
#define OUTPUT_SIZE 128


int main() {
  VKC computation;
  
  computation.setInputSize(INPUT_SIZE * sizeof(float));
  computation.setOutputSize(OUTPUT_SIZE * sizeof(float));
  
  computation.startVulkan();
  
  float arr_in[INPUT_SIZE];
  float arr_out[OUTPUT_SIZE];
  
  computation.execute();
  computation.endVulkan();
  
  return 1;
}
