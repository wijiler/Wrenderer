#include <renderer.h>
#include <windowing.h>
WRErenderer renderer = {0};
int main()
{
    Wremonitor monitor = getMonitorInfo();
    WREwindow window = openWindow("Wre2 Rendering Test Bench", monitor.w / 7, monitor.h / 7, 1920, 1080);
    renderer.window = window;
    initializeRenderer(&renderer);
    WREShader shader = createShader("./Test/shader/test.spv", WRE_SHADER_STAGE_VERTEX | WRE_SHADER_STAGE_FRAGMENT);
    WREpipeline pipeline = createPipeline("trianglePipeline", (WREvertexFormat){0, NULL, 0, NULL}, &shader, 1, WRE_BACK_CULLING, WRE_WINDING_CW);
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();
    }
    terminateRenderer(&renderer);

    return 0;
}