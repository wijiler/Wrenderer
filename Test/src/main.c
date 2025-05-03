#include <context.h>
#include <pipeline.h>
#include <windowing.h>

WREcontextObject context = {0};
int main()
{
    WREmonitor monitor = getMonitorInfo();
    WREwindow window = openWindow("Wre2 Rendering Test Bench", monitor.w / 7, monitor.h / 7, 1920, 1080);
    context.window = window;
    initializeContext(&context);
    WREshader shader = createShader("A:\\projects\\WRE2\\Test\\shader\\test.spv", WRE_SHADER_STAGE_VERTEX | WRE_SHADER_STAGE_FRAGMENT);
    WREpipeline pipeline = createPipeline("trianglePipeline", (WREvertexFormat){0, NULL, 0, NULL}, &shader, 1, WRE_BACK_CULLING, WRE_WINDING_CW, (VkFormat[8]){VK_FORMAT_R8G8B8A8_SRGB}, 1);

    WREcommandList list = {0};
    initializeCommandList(&list);

    WREimage *frameBuffers[8] = {0};
    frameBuffers[0] = context.window.context.CurrentSCImg;

    bindPipeline(&list, pipeline);

    startRenderPass(&list, frameBuffers, 1);
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
