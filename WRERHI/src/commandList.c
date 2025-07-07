#include <commandList.h>
#include <context.h>

#ifdef WREUSEVULKAN
#include <backends/vulkan/commandList.h>
#endif

#define addCmd                                \
    list->commands[list->commandCount] = cmd; \
    list->commandCapacity -= 1;               \
    list->commandCount += 1;

#define allocCmd                                                                                   \
    if (list->commandCapacity == 0)                                                                \
    {                                                                                              \
        list->commands = realloc(list->commands, sizeof(WRECommand) * (list->commandCount + 256)); \
        list->commandCapacity = 256;                                                               \
    }

void initializeCommandList(WREcommandList *list)
{
    list->commandCapacity = 256;
    list->commandCount = 0;
    list->commands = malloc(sizeof(WRECommand) * 256);
}

void resetCommandList(WREcommandList *list)
{
    memset(list->commands, 0, sizeof(WRECommand) * list->commandCount);
    list->commandCapacity += list->commandCount;
    list->commandCount = 0;
}

void startRenderPass(WREcommandList *list, WREAttachment *frameBuffers, uint8_t frameBufCount)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_RENDERPASS_START;
    cmd.data.renderpass.frameBufCount = frameBufCount;
    cmd.data.renderpass.frameBuffers = frameBuffers;
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
    cmd.data.draw_call[0] = vertexCount;
    cmd.data.draw_call[1] = instanceCount;
    addCmd
}
void dispatchCompute(WREcommandList *list, uint32_t x, uint32_t y, uint32_t z)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_COMPUTE_DISPATCH;
    cmd.data.compute_dispatch[0] = x;
    cmd.data.compute_dispatch[1] = y;
    cmd.data.compute_dispatch[2] = z;
    addCmd
}
void bindPipeline(WREcommandList *list, WREpipeline *pipeline)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_SET_PIPELINE;
    cmd.data.pipeline = pipeline;
    addCmd
}

void bindPushConstants(WREcommandList *list, uint32_t size, uint32_t offset, WREshaderStage stage, void *pushData)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_PUSH_CONSTANTS;
    cmd.data.push_constant.stage = stage;
    cmd.data.push_constant.size = size;
    cmd.data.push_constant.offset = offset;
    cmd.data.push_constant.data = pushData;
    addCmd;
}

void submitCommandList(WREcontextObject *context, WREcommandList list)
{
    context->currentFrame = list;
}

void bindVertexBuffer(WREcommandList *list, WREBuffer buf, uint64_t offset)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_PUSH_CONSTANTS;
    cmd.data.buffer_binding.buf = buf.buffer;
    cmd.data.buffer_binding.offset = offset;
    addCmd;
}

void bindIndexBuffer(WREcommandList *list, WREBuffer buf, uint64_t offset)
{
    allocCmd;
    WRECommand cmd = {0};
    cmd.type = WRE_COMMAND_TYPE_PUSH_CONSTANTS;
    cmd.data.buffer_binding.buf = buf.buffer;
    cmd.data.buffer_binding.offset = offset;
    addCmd;
}

void executeCommandList(WREcontextObject *context)
{
#ifdef WREUSEVULKAN
    WREvkExecuteCommandList(&context->core, &context->window.context, &context->currentFrame);
#endif
}

#undef allocCmd
#undef addCmd