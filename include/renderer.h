#ifndef REND
#define REND
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  VertexBuf,
  IndexBuf,
  UniformBuf,
} BUFFERTYPE;

typedef enum {
  HOST_ACCESS,   // CPU_ONLY
  DEVICE_ACCESS, // GPU_ONLY
} BufferAccess;

typedef struct {
  int dataSize;
  VkBufferUsageFlags usage;
  BufferAccess access;
} BufferCreateInfo;

typedef struct {
  BUFFERTYPE type;
  uint32_t index, size;
  VkBuffer buffer;
  VkDeviceMemory *associatedMemory;
} Buffer;

typedef struct {
  float worldMatrix[16];
} pushConstants;

typedef struct {
  char *resources;
  Buffer *buffers;
  int bufferCount;
  char *passName;
  pushConstants pc;
} rendergraph_leaf_t;

typedef struct {

  void *resources;
  rendergraph_leaf_t Root;
} rendergraph_t;

#define FRAMECOUNT 3

typedef struct {
  VkInstance instance;

  VkPhysicalDevice pDev;
  int qfi;
  VkSurfaceKHR surface;
  VkDevice lDev;
  VkQueue pQueue, gQueue;

  VkSwapchainKHR swapChain;
  VkSurfaceFormatKHR sFormat;
  VkPresentModeKHR sPresentMode;
  VkImage *swapChainImages;
  unsigned int imgCount;
  VkImageView *swapChainImageViews;

  VkFence fences[FRAMECOUNT];
  VkSemaphore imageAvailiable[FRAMECOUNT];
  VkSemaphore renderFinished[FRAMECOUNT];

  int currentBuffer;
  VkCommandPool *commandPool;
  VkCommandBuffer commandBuffers[FRAMECOUNT];

  GLFWwindow *window;

} VulkanCore_t;

typedef struct {
  rendergraph_t *rendergraph;
  VulkanCore_t vkCore;
} renderer_t;

void createBuffer(VulkanCore_t core, BufferCreateInfo *createInfo, Buffer *buf);
void initRenderer(renderer_t *renderer);
void pushDataToBuffer(VulkanCore_t core, void *data, uint32_t dataSize,
                      Buffer buf);
void copyBuf(VulkanCore_t core, Buffer src, Buffer dest);
void destroyBuffer(Buffer buf, VulkanCore_t core);
void destroyRenderer(renderer_t *renderer);
#ifdef __cplusplus
}
#endif
#endif