#include <backends/vulkan/buffer.h>
#include <backends/vulkan/globals.h>

// TODO: TLSF, or slab?, Either way implement an allocation callback
WREVkBuffer createBuffer(uint64_t size, WREBufferMemoryFlags locality, WREBufferUsageFlags usage)
{
    WREVkBuffer buf = {usage, locality, size, NULL, NULL, 0x0, NULL};

    VkMemoryAllocateInfo vkMemAlloc = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        NULL,
        size,
        (locality == CPU_GPU) ? hostSharedHeapIndex : deviceLocalHeapIndex,
    };
    vkAllocateMemory(WREdevice, &vkMemAlloc, NULL, &buf.memory);

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

    vkBindBufferMemory(WREdevice, buf.buffer, buf.memory, 0);

    VkBufferDeviceAddressInfo deviceAdress = {
        VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        NULL,
        buf.buffer,
    };

    buf.address = vkGetBufferDeviceAddress(WREdevice, &deviceAdress);

    if (locality == CPU_GPU)
    {
        vkMapMemory(WREdevice, buf.memory, 0, size, 0, buf.mappedMemory);
    }

    return buf;
}

void pushCPUBuffer(WREVkBuffer buffer, void *data, size_t size)
{
    memcpy(buffer.mappedMemory, data, size);
}

void pushDatatoBuffer(WREVkBuffer buffer, void *data, size_t size)
{
    memcpy(WREstagingMappedMemory, 0x0, 1000000);
    memcpy(WREstagingMappedMemory, data, size);
    VkBufferCopy copyRegion = {
        0,
        0,
        buffer.size,
    };
    VkCommandBufferBeginInfo begInf = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        0,
        NULL,
    };
    vkWaitForFences(WREdevice, 1, &WREinstantFence, VK_TRUE, UINT64_MAX);
    vkBeginCommandBuffer(WREinstantCommandBuffer, &begInf);
    vkCmdCopyBuffer(WREinstantCommandBuffer, WREstagingBuffer, buffer.buffer, 1, &copyRegion);

    vkEndCommandBuffer(WREinstantCommandBuffer);
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;

    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = 0;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &WREinstantCommandBuffer;

    vkQueueSubmit(WREgraphicsQueue, 1, &submitInfo, WREinstantFence);
    if (size > 1000000)
    {
        uint32_t offset = 0;
        uint32_t remaining = size;
        for (uint32_t i = 1000000; i >= size; i += 1000000)
        {
            vkWaitForFences(WREdevice, 1, &WREinstantFence, VK_TRUE, UINT64_MAX);
            memcpy(WREstagingMappedMemory, 0x0, 1000000);
            memcpy(WREstagingMappedMemory, data, size);
            VkBufferCopy copyRegion = {
                0,
                offset,
                remaining < 1000000 ? remaining : 1000000};
            VkCommandBufferBeginInfo begInf = {
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                NULL,
                0,
                NULL,
            };
            vkBeginCommandBuffer(WREinstantCommandBuffer, &begInf);
            vkCmdCopyBuffer(WREinstantCommandBuffer, WREstagingBuffer, buffer.buffer, 1, &copyRegion);
            vkEndCommandBuffer(WREinstantCommandBuffer);
            VkSubmitInfo submitInfo = {0};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = NULL;

            submitInfo.waitSemaphoreCount = 0;
            submitInfo.pWaitSemaphores = NULL;
            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores = 0;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &WREinstantCommandBuffer;

            vkQueueSubmit(WREgraphicsQueue, 1, &submitInfo, WREinstantFence);
            offset += 1000000;
            remaining -= 1000000;
        }
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