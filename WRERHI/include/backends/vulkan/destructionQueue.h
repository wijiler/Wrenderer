#ifndef WREDESQUEUE_H__
#define WREDESQUEUE_H__
#include <vulkan/vulkan.h>

typedef struct VkObj VkObj;

typedef void (*destructionFunction)(VkObj object);

struct VkObj
{
    void *handle;
    destructionFunction destructionCallback;
};

typedef struct
{
    uint32_t capacity;
    uint32_t objectCount;
    VkObj *objects;
} DestructionQueue;

extern DestructionQueue WREVKDesQueue;

void destroyVkObjects();
void addDestructableObject(VkObj object);
#endif