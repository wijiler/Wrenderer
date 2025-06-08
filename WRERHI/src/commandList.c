#include <commandList.h>
#include <context.h>

#ifdef WREUSEVULKAN
#include <backends/vulkan/commandList.h>
#endif

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

void initializeCommandList(WREcommandList *list)
{
    list->commandCapacity = 256;
    list->commandCount = 0;
    list->commands = malloc(sizeof(WRECommand) * 256);
}

typedef struct
{
    WREimage *frameBuffers[8];
    uint8_t frameBufCount;
} renPassInfo;

void startRenderPass(WREcommandList *list, WREimage *frameBuffers[8], uint8_t frameBufCount)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_RENDERPASS_START;
    cmd.data = malloc(sizeof(renPassInfo));
    renPassInfo *data = (renPassInfo *)cmd.data;
    data->frameBuffers[0] = frameBuffers[0];
    data->frameBuffers[1] = frameBuffers[1];
    data->frameBuffers[2] = frameBuffers[2];
    data->frameBuffers[3] = frameBuffers[3];
    data->frameBuffers[4] = frameBuffers[4];
    data->frameBuffers[5] = frameBuffers[5];
    data->frameBuffers[6] = frameBuffers[6];
    data->frameBuffers[7] = frameBuffers[7];
    data->frameBufCount = frameBufCount;
    addCmd
}
void endRenderPass(WREcommandList *list)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_RENDERPASS_END;
    addCmd
}
void drawCall(WREcommandList *list, uint32_t vertexCount, uint32_t instanceCount)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_DRAW;
    cmd.data = malloc(sizeof(uint32_t) * 2);
    cmd.data[0] = vertexCount;
    cmd.data[1] = instanceCount;
    addCmd
}
void dispatchCompute(WREcommandList *list, uint32_t x, uint32_t y, uint32_t z)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_COMPUTE_DISPATCH;
    cmd.data = malloc(sizeof(uint32_t) * 3);
    cmd.data[0] = x;
    cmd.data[1] = y;
    cmd.data[2] = z;
    addCmd
}
void bindPipeline(WREcommandList *list, WREpipeline pipeline)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_SET_PIPELINE;
    cmd.data = malloc(sizeof(WREpipeline));
    WREpipeline *p = (WREpipeline *)cmd.data;
    p[0] = pipeline;
    addCmd
}
typedef struct
{
    WREshaderStage stage;
    uint32_t size;
    uint32_t offset;
    void *data;
} pcData;
void bindPushConstants(WREcommandList *list, uint32_t size, uint32_t offset, WREshaderStage stage, void *pushData)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_PUSH_CONSTANTS;
    cmd.data = malloc(sizeof(pcData));
    *(pcData *)cmd.data = (pcData){
        stage,
        size,
        offset,
        pushData,
    };
    addCmd;
}

void submitCommandList(WREcontextObject *context, WREcommandList list)
{
    context->currentFrame = list;
}

void executeCommandList(WREcontextObject *context)
{
#ifdef WREUSEVULKAN
    vkExecuteCommandList(&context->core, &context->window.context, &context->currentFrame);
#endif
}

#undef allocCmd
#undef addCmd