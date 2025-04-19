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
    WRECommandList currentFrame;
} WREContextObject;

void terminateContext(WREContextObject *context);
void initializeContext(WREContextObject *context);
void submitCommandList(WREContextObject *context, WRECommandList list);
void executeCommandList(WREContextObject *context);
#endif
