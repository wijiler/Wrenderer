#ifndef WREVULKCMDLIST_H__
#define WREVULKCMDLIST_H__
#include <backends/vulkan/initialization.h>
#include <commandList.h>

void WREvkExecuteCommandList(RendererCoreContext *context, RendererWindowContext *winContext, WREcommandList *list);

#endif // WREVULKCMDLIST_H__