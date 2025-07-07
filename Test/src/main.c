#include <buffer.h>
#include <context.h>
#include <pipeline.h>
#include <texturing.h>
#include <windowing.h>

typedef struct
{
    float pos[3];
} vertex;
typedef struct
{
    VkDeviceAddress vBuf;
} pushconstants;

const vertex verts[3] = {
    {1.f, 1.f, 0.0f},
    {-1.f, 1.f, 0.0f},
    {0.f, -1.f, 0.0f},
};

pushconstants pc = {
    0,
};

WREcontextObject context = {0};
int main()
{
    WREmonitor monitor = getMonitorInfo();
    WREwindow window = openWindow("Wre2 Rendering Test Bench", monitor.w / 7, monitor.h / 7, 1920, 1080);
    context.window = window;
    initializeContext(&context);

    WREBuffer vBuf = createBuffer(sizeof(vertex) * 3, GPU_LOCAL, GENERAL | TRANSFER_DST);
    pushDatatoBuffer(vBuf, (vertex *)verts, sizeof(verts));
    pc.vBuf = vBuf.address;
    WREshader shader = createShader("A:\\projects\\WRE2\\Test\\shader\\test.spv", WRE_SHADER_STAGE_VERTEX | WRE_SHADER_STAGE_FRAGMENT);
    setPushConstants(&shader, &pc, sizeof(VkDeviceAddress));
    WREVkPipelineCI ci = {
        {VK_FORMAT_R8G8B8A8_SRGB},
        {0, NULL, 0, NULL},
        &shader,
        WRE_BACK_CULLING,
        WRE_WINDING_CW,
        1,
        1,
    };
    WREpipeline pipeline = createPipeline("trianglePipeline", &ci);

    WREcommandList list = {0};
    initializeCommandList(&list);

    WREAttachment frameBuffers[8] = {0};
    frameBuffers[0] = (WREAttachment){context.window.context.CurrentSCImg, WRE_COLOR_ATTACHMENT};
    upload_texture("A:/projects/WRE2/Test/assets/colored_bars.png");
    bindPipeline(&list, pipeline);

    startRenderPass(&list, frameBuffers, 1);
    bindPushConstants(&list, 128, 0, WRE_SHADER_STAGE_VERTEX | WRE_SHADER_STAGE_FRAGMENT, shader.pushconstants);
    drawCall(&list, 3, 1);
    endRenderPass(&list);

    submitCommandList(&context, list);

    while (!glfwWindowShouldClose(window.window))
    {
        executeCommandList(&context);
        glfwPollEvents();
    }
    terminateContext(&context);

    return 0;
}
