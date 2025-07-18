#include <backends/vulkan/destructionQueue.h>
#include <backends/vulkan/globals.h>

DestructionQueue WREVKDesQueue = {100, 0, NULL};

void destroyVkObjects()
{
    vkDeviceWaitIdle(WREdevice);
    for (uint32_t i = WREVKDesQueue.objectCount; i >= 0; i--)
    {
        WREVkObj currentObj = WREVKDesQueue.objects[i];
        currentObj.destructionCallback(currentObj);
    }
}
void addDestructableObject(WREVkObj object)
{
    if (WREVKDesQueue.objectCount == 0 || WREVKDesQueue.capacity == 0)
    {
        WREVKDesQueue.objects = realloc(WREVKDesQueue.objects, sizeof(WREVkObj) * (WREVKDesQueue.objectCount + 100));
        WREVKDesQueue.capacity = 100;
    }
    WREVKDesQueue.objects[WREVKDesQueue.objectCount] = object;
    WREVKDesQueue.capacity -= 1;
    WREVKDesQueue.objectCount += 1;
}