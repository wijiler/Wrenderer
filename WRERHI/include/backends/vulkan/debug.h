#ifndef WRE_DEBUG_H__
#define WRE_DEBUG_H__
#include <vulkan/vulkan.h>

void setVkDebugName(char *name, VkObjectType objectType, uint64_t handle);

#endif