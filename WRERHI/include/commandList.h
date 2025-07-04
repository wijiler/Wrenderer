#ifndef WRECOMMANDLIST_H__
#define WRECOMMANDLIST_H__
#include <image.h>
#include <inttypes.h>
#include <pipeline.h>
#include <stdbool.h>
#ifdef WREUSEVULKAN
#include <backends/vulkan/shader.h>
#endif

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
} WREcommandList;

void initializeCommandList(WREcommandList *list);
void startRenderPass(WREcommandList *list, WREimage *frameBuffers[8], uint8_t frameBufCount);
void endRenderPass(WREcommandList *list);
void drawCall(WREcommandList *list, uint32_t vertexCount, uint32_t instanceCount);
void dispatchCompute(WREcommandList *list, uint32_t x, uint32_t y, uint32_t z);
void bindPipeline(WREcommandList *list, WREpipeline pipeline);
void bindPushConstants(WREcommandList *list, uint32_t size, uint32_t offset, WREshaderStage stage, void *pushData);

#endif