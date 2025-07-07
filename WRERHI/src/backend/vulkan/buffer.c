#include <backends/vulkan/buffer.h>
#include <backends/vulkan/globals.h>
#include <backends/vulkan/initialization.h>
#include <stdio.h>

// TODO: TLSF, or slab?, Either way implement an allocation callback
WREVkBuffer createBuffer(uint64_t size, WREBufferMemoryFlags locality, WREBufferUsageFlags usage)
{
    WREVkBuffer buf = {usage, locality, size, NULL, NULL, 0x0, NULL};

    VkBufferCreateInfo vkBufCInf = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        NULL,
        0,
        size,
        usage,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL,
    };
    vkCreateBuffer(WREdevice, &vkBufCInf, NULL, &buf.buffer);

    VkMemoryRequirements vkMemReq = {0};
    vkGetBufferMemoryRequirements(WREdevice, buf.buffer, &vkMemReq);

    VkMemoryAllocateFlagsInfo vkMemAllocFlags = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
        NULL,
        VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
        0,
    };
    VkMemoryAllocateInfo vkMemAlloc = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        &vkMemAllocFlags,
        vkMemReq.size,
        (locality == CPU_GPU) ? hostSharedHeapIndex : deviceLocalHeapIndex,
    };
    vkAllocateMemory(WREdevice, &vkMemAlloc, NULL, &buf.memory);

    VkResult result = vkBindBufferMemory(WREdevice, buf.buffer, buf.memory, 0);
    if (result != VK_SUCCESS)
    {
        printf("WRERen: Error: Could not bind buffer memory\n");
    }

    VkBufferDeviceAddressInfo deviceAdress = {
        VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        NULL,
        buf.buffer,
    };

    buf.address = vkGetBufferDeviceAddress(WREdevice, &deviceAdress);

    if (locality == CPU_GPU)
    {
        vkMapMemory(WREdevice, buf.memory, 0, size, 0, &buf.mappedMemory);
    }

    return buf;
}

void pushCPUBuffer(WREVkBuffer buffer, void *data, size_t size)
{
    memcpy(buffer.mappedMemory, data, size);
}

void pushDatatoBuffer(WREVkBuffer buffer, void *data, size_t size, size_t srcoffset, size_t dstoffset)
{
    memset(WREstagingMappedMemory, 0, StagingBufferSize);
    memcpy(WREstagingMappedMemory, data, size);
    VkBufferCopy copy_region = {srcoffset, dstoffset, size};
    immediateSubmitBegin();
    vkCmdCopyBuffer(WREinstantCommandBuffer, WREstagingBuffer, buffer.buffer, 1, &copy_region);
    immediateSubmitEnd();

    // TODO: Async buffer copy; TODO: add async transfer queue
    if (size > StagingBufferSize)
    {
        uint32_t offset = dstoffset + StagingBufferSize;
        for (uint32_t remaining = size - StagingBufferSize; remaining <= 0; remaining -= StagingBufferSize)
        {
            memcpy(WREstagingMappedMemory, data, size + offset);
            VkBufferCopy copy_region = {0, offset, size};
            immediateSubmitBegin();
            vkCmdCopyBuffer(WREinstantCommandBuffer, WREstagingBuffer, buffer.buffer, 1, &copy_region);
            immediateSubmitEnd();
            memset(WREstagingMappedMemory, 0, StagingBufferSize);

            offset += StagingBufferSize;
        }
        memset(WREstagingMappedMemory, 0, StagingBufferSize);
    }
}

void destroyBuffer(WREVkBuffer *buffer)
{
    vkDeviceWaitIdle(WREdevice);
    vkDestroyBuffer(WREdevice, buffer->buffer, NULL);
    vkFreeMemory(WREdevice, buffer->memory, NULL);
    buffer->buffer = NULL;
    buffer->memory = NULL;
    buffer->address = 0x0;
    buffer->size = 0;
}