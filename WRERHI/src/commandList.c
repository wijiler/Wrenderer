#include <commandList.h>
#include <context.h>

#define addCmd                                \
    list->commands[list->commandCount] = cmd; \
    list->commandCapacity -= 1;               \
    list->commandCount += 1;

#define allocCmd                                                            \
    if (list->commandCapacity == 0)                                         \
    {                                                                       \
        list->commands = realloc(list->commands, sizeof(WRECommand) * 256); \
        list->commandCapacity = 256;                                        \
    }

void initializeCommandList(WRECommandList *list)
{
    list->commandCapacity = 256;
    list->commandCount = 0;
    list->commands = malloc(sizeof(WRECommand) * 256);
}

void startRenderPass(WRECommandList *list, WREImage frameBuffers[8])
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_RENDERPASS_START;
    cmd.data = malloc(sizeof(WREImage) * 8);
    memcpy(cmd.data, frameBuffers, sizeof(WREImage) * 8);
    addCmd
}
void endRenderPass(WRECommandList *list)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_RENDERPASS_END;
    addCmd
}
void drawCall(WRECommandList *list, uint32_t vertexCount, uint32_t instanceCount)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_DRAW;
    cmd.data = malloc(sizeof(uint32_t) * 2);
    cmd.data[0] = vertexCount;
    cmd.data[1] = instanceCount;
    addCmd
}
void dispatchCompute(WRECommandList *list, uint32_t x, uint32_t y, uint32_t z)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_COMPUTE_DISPATCH;
    cmd.data = malloc(sizeof(uint32_t) * 2);
    cmd.data[0] = x;
    cmd.data[1] = y;
    cmd.data[2] = z;
    addCmd
}
void bindPipeline(WRECommandList *list, WREpipeline pipeline)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_SET_PIPELINE;
    cmd.data = malloc(sizeof(WREpipeline));
    WREpipeline *p = (WREpipeline *)cmd.data;
    p[0] = pipeline;
    addCmd
}
void pushConstants(WRECommandList *list, void *pushData)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_PUSH_CONSTANTS;
    cmd.data = pushData;
    addCmd;
}

void submitCommandList(WREContexObject *renderer, WRECommandList list)
{
    renderer->commandQueue = realloc(renderer->commandQueue, sizeof(WRECommandList) * (renderer->commandListCount + 1));
    renderer->commandQueue[renderer->commandListCount] = list;
    renderer->commandListCount += 1;
}

#undef allocCmd
#undef addCmd