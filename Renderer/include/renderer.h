#ifndef WREREN_H__
#define WREREN_H__
#include <windowing.h>
#ifdef WREUSEVULKAN
#include <backends/vulkan/WREVulk.h>
#endif
typedef struct
{
    RendererCoreContext core;
    WREwindow window;
} WRErenderer;

void terminateRenderer(WRErenderer *renderer);
void initializeRenderer(WRErenderer *renderer);
#endif
