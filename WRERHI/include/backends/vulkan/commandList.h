#ifndef WREVULKCMDLIST_H__
#define WREVULKCMDLIST_H__
#include <backends/vulkan/initialization.h>
#include <commandList.h>

void vkExecuteCommandList(RendererCoreContext *context, RendererWindowContext *winContext, WRECommandList *list);

#endif // WREVULKCMDLIST_H__