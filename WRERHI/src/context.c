#include <context.h>
// #ifdef WREUSEVULKAN
// #include <backends/vulkan/destructionQueue.h>
// #endif
void terminateContext(WREContexObject *context)
{
    // #ifdef WREUSEVULKAN
    //     destroyVkObjects();
    // #endif
    glfwDestroyWindow(context->window.window);
    glfwTerminate();
}

#ifdef WREUSEVULKAN
void initializeContext(WREContexObject *context)
{
    initializeVulkan(&context->core, &context->window.context, context->window.window);
}

#endif
