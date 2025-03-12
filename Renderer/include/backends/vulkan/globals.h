#ifndef WREVULKGLOBALS_H__
#define WREVULKGLOBALS_H__
#include <vulkan/vulkan.h>
extern VkInstance WREVulkinstance;
extern VkPhysicalDevice WREPDevice;
extern VkDevice WREDevice;
extern VkCommandPool WREcommandPool;
extern VkQueue WREgraphicsQueue, WREpresentQueue, WREcomputeQueue, WREtransferQueue;
#endif