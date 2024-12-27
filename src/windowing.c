#include <stdio.h>
#include <stdlib.h>
#include <windowing.h>

double WREMouseX = 0;
double WREMouseY = 0;

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW ERROR:%i, %s\n", error, description);
}

static void resizeCallback(GLFWwindow *window, int w, int h)
{
    renderer_t *core = glfwGetWindowUserPointer(window);

    recreateSwapchain(&core->vkCore, w, h);
}

void (*inputcallback)(int key, int action) = NULL;
static void glfwInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    inputcallback(key, action);
}
static void glfwMouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    WREMouseX = xpos;
    WREMouseY = ypos;
}
void launch_window(winf_t wininfo, renderer_t *renderer, void (*update)(), void (*start)(), void (*input)(int key, int action))
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
    // glfwSetInputMode(renderer->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    start();
    glfwSetWindowUserPointer(renderer->window, (void *)renderer);
    glfwSetWindowSizeCallback(renderer->window, resizeCallback);
    glfwSetKeyCallback(renderer->window, glfwInputCallback);
    glfwSetCursorPosCallback(renderer->window, glfwMouseCallback);
    inputcallback = input;
    while (!glfwWindowShouldClose(renderer->window))
    {
        glfwPollEvents();
        update();
    }
}
