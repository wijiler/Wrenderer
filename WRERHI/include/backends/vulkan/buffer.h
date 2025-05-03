#ifndef WRE_VK_BUFFER_H__
#define WRE_VK_BUFFER_H__
#include <vulkan/vulkan.h>
typedef enum
{
    VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    STORAGE = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    UNIFORM = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    GENERAL = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
} WREBufferUsageFlags;

typedef enum
{
    CPU_GPU = 0,
    GPU_LOCAL,
} WREBufferMemoryFlags;

typedef struct
{
    WREBufferUsageFlags usage;
    WREBufferMemoryFlags locality;
    uint64_t size;
    VkDeviceMemory memory;
    void *mappedMemory;
    VkBuffer buffer;
    VkDeviceAddress address;
} WREVkBuffer;

WREVkBuffer createBuffer(uint64_t size, WREBufferMemoryFlags locality, WREBufferUsageFlags usage);
void destroyBuffer(WREVkBuffer *buffer);

#endif