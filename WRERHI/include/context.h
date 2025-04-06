#ifndef WREREN_H__
#define WREREN_H__
#include <commandList.h>
#include <windowing.h>

#ifdef WREUSEVULKAN
#include <backends/vulkan/WREVulk.h>
#endif
typedef struct
{
    RendererCoreContext core;
    WREwindow window;
    uint32_t commandListCount;
    WRECommandList *commandQueue;
} WREContexObject;

void terminateContext(WREContexObject *context);
void initializeContext(WREContexObject *context);
void submitCommandList(WREContexObject *context, WRECommandList list);
#endif
