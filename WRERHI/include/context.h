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
    WREcommandList currentFrame;
} WREcontextObject;

void terminateContext(WREcontextObject *context);
void initializeContext(WREcontextObject *context);
void submitCommandList(WREcontextObject *context, WREcommandList list);
void executeCommandList(WREcontextObject *context);
#endif
