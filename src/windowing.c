#include <stdio.h>
#include <stdlib.h>
#include <windowing.h>

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW ERROR:%i, %s\n", error, description);
}

static void resizeCallback(GLFWwindow *window, int w, int h)
{
    renderer_t *core = glfwGetWindowUserPointer(window);

    recreateSwapchain(&core->vkCore, w, h);
}

void launch_window(winf_t wininfo, renderer_t *renderer, void (*PFN_update)(), void (*PFN_start)())
{
    glfwVulkanSupported();
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    renderer->window = glfwCreateWindow(wininfo.w, wininfo.h, wininfo.name, NULL, NULL);
    if (!renderer->window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    void (*update)() = PFN_update;
    void (*start)() = PFN_start;

    start();
    glfwSetWindowUserPointer(renderer->window, (void *)renderer);
    glfwSetWindowSizeCallback(renderer->window, resizeCallback);
    while (!glfwWindowShouldClose(renderer->window))
    {
        glfwPollEvents();
        update();
    }
}
