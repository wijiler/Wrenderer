#ifndef WRECOMMANDLIST_H__
#define WRECOMMANDLIST_H__
#include <image.h>
#include <inttypes.h>
#include <pipeline.h>
#include <stdbool.h>

typedef enum
{
    WRE_COMMAND_TYPE_RENDERPASS_START = 0,
    WRE_COMMAND_TYPE_DRAW,
    WRE_COMMAND_TYPE_RENDERPASS_END,
    WRE_COMMAND_TYPE_COMPUTE_DISPATCH,
    WRE_COMMAND_TYPE_SET_PIPELINE,
    WRE_COMMAND_TYPE_PUSH_CONSTANTS,
    /*TODO*/
    WRE_COMMAND_TYPE_BIND_VERTEX,
    WRE_COMMAND_TYPE_BIND_INDEX,
    WRE_COMMAND_TYPE_BIND_DESCRIPTOR,
} WRECommandType;

typedef struct
{
    WRECommandType type;
    char *data;
} WRECommand;

typedef struct
{
    uint32_t commandCapacity;
    uint32_t commandCount;
    WRECommand *commands;
} WRECommandList;

void initializeCommandList(WRECommandList *list);
void startRenderPass(WRECommandList *list, WREImage frameBuffers[8]);
void endRenderPass(WRECommandList *list);
void drawCall(WRECommandList *list, uint32_t vertexCount, uint32_t instanceCount);
void dispatchCompute(WRECommandList *list, uint32_t x, uint32_t y, uint32_t z);
void bindPipeline(WRECommandList *list, WREpipeline pipeline);
void pushConstants(WRECommandList *list, void *pushData);

#endif