#include <renderer.h>
void terminateRenderer(WRErenderer *renderer)
{
    glfwDestroyWindow(renderer->window.window);
    glfwTerminate();
}

#ifdef WREUSEVULKAN
void initializeRenderer(WRErenderer *renderer)
{
    initializeVulkan(&renderer->core, &renderer->window.context, renderer->window.window);
}
#endif
