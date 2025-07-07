#ifndef WRECOMMANDLIST_H__
#define WRECOMMANDLIST_H__
#include <buffer.h>
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
    WRE_COMMAND_TYPE_BIND_VERTEX,
    WRE_COMMAND_TYPE_BIND_INDEX,
    /*TODO*/
    WRE_COMMAND_TYPE_BIND_DESCRIPTOR,
} WRECommandType;
typedef enum
{
    WRE_COLOR_ATTACHMENT,
    WRE_DEPTH_IMAGE,
    WRE_STENCIL_BUFFER,
} WREAttachmentUsage;
typedef struct
{
    WREimage *img;
    WREAttachmentUsage usage;
} WREAttachment;

typedef union
{
    struct
    {
        WREAttachment *frameBuffers;
        uint8_t frameBufCount;
    } renderpass;
    uint32_t draw_call[2];
    uint32_t compute_dispatch[3];
    WREpipeline *pipeline;
    struct
    {
        void *data;
        WREshaderStage stage;
        uint32_t size, offset;
    } push_constant;
    struct
    {
        uint64_t offset;
        VkBuffer buf;
    } buffer_binding;
} WRECommandData;

typedef struct
{
    WRECommandType type;
    WRECommandData data;
} WRECommand;

typedef struct
{
    uint32_t commandCapacity;
    uint32_t commandCount;
    WRECommand *commands;
} WREcommandList;

void initializeCommandList(WREcommandList *list);
void resetCommandList(WREcommandList *list);
void startRenderPass(WREcommandList *list, WREAttachment *frameBuffers, uint8_t frameBufCount);
void endRenderPass(WREcommandList *list);
void drawCall(WREcommandList *list, uint32_t vertexCount, uint32_t instanceCount);
void dispatchCompute(WREcommandList *list, uint32_t x, uint32_t y, uint32_t z);
void bindPipeline(WREcommandList *list, WREpipeline *pipeline);
void bindPushConstants(WREcommandList *list, uint32_t size, uint32_t offset, WREshaderStage stage, void *pushData);
void bindVertexBuffer(WREcommandList *list, WREBuffer buf, uint64_t offset);
void bindIndexBuffer(WREcommandList *list, WREBuffer buf, uint64_t offset);
#endif