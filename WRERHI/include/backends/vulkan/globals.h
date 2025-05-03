#ifndef WREVULKGLOBALS_H__
#define WREVULKGLOBALS_H__
#include <vulkan/vulkan.h>
#define FramesInFlightCount 3

extern VkInstance WREvulkInstance;
extern VkPhysicalDevice WREpDevice;
extern VkDevice WREdevice;
extern VkCommandPool WREcommandPool;
extern VkFence WREinstantFence;
extern VkCommandBuffer WREinstantCommandBuffer;
extern VkQueue WREgraphicsQueue, WREpresentQueue, WREcomputeQueue, WREtransferQueue;
extern VkDeviceMemory WREStagingMemory;
extern void *WREstagingMappedMemory;
extern VkBuffer WREstagingBuffer;
extern int32_t deviceLocalHeapIndex, hostSharedHeapIndex;

#endif