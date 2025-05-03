#include <context.h>
// #ifdef WREUSEVULKAN
// #include <backends/vulkan/destructionQueue.h>
// #endif
void terminateContext(WREcontextObject *context)
{
    // #ifdef WREUSEVULKAN
    //     destroyVkObjects();
    // #endif
    glfwDestroyWindow(context->window.window);
    glfwTerminate();
}

#ifdef WREUSEVULKAN
void initializeContext(WREcontextObject *context)
{
    initializeVulkan(&context->core, &context->window.context, context->window.window);
}

#endif
