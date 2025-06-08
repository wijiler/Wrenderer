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
    CPU_GPU = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    GPU_LOCAL = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
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
void pushCPUBuffer(WREVkBuffer buffer, void *data, size_t size);
void pushDatatoBuffer(WREVkBuffer buffer, void *data, size_t size);
void destroyBuffer(WREVkBuffer *buffer);

#endif