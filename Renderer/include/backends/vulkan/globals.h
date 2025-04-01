#ifndef WREVULKGLOBALS_H__
#define WREVULKGLOBALS_H__
#include <vulkan/vulkan.h>
#define FramesInFlightCount 3

extern VkInstance WREVulkinstance;
extern VkPhysicalDevice WREPDevice;
extern VkDevice WREDevice;
extern VkCommandPool WREcommandPool;
extern VkCommandBuffer WREInstantCommandBuffer;
extern VkQueue WREgraphicsQueue, WREpresentQueue, WREcomputeQueue, WREtransferQueue;

#endif