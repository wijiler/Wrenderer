#ifndef WREDESQUEUE_H__
#define WREDESQUEUE_H__
#include <vulkan/vulkan.h>

typedef struct WREVkObj WREVkObj;

typedef void (*destructionFunction)(WREVkObj object);

struct WREVkObj
{
    void *handle;
    destructionFunction destructionCallback;
};

typedef struct
{
    uint32_t capacity;
    uint32_t objectCount;
    WREVkObj *objects;
} DestructionQueue;

extern DestructionQueue WREVKDesQueue;

void destroyVkObjects();
void addDestructableObject(WREVkObj object);
#endif