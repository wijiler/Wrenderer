#include <stdbool.h>
#include <stdio.h>
#include <windowing.h>

static bool glfwInitialized = false;
static void error_callback(int error, const char *description)
{
    fprintf(stderr, "WREREN: GLFW ERROR:%i, %s\n", error, description);
}
// clang-format off
#define WREGLFWINIT                 \
    if (!glfwInitialized) {         \
            if (!glfwInit())        \
                exit(EXIT_FAILURE); \
            glfwInitialized = true; \
        }
// clang-format on
#ifdef WREUSEVULKAN
WREwindow openWindow(char *Title, int x, int y, int w, int h)
{
    WREwindow win = {
        {0},
        Title,
        NULL,
    };
    win.context.w = w;
    win.context.h = h;
    win.context.x = x;
    win.context.y = y;
    if (glfwVulkanSupported() != true)
        printf("Vulkan not supported on this machine\n");
    glfwSetErrorCallback(error_callback);
    WREGLFWINIT

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(w, h, Title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }
    glfwSetWindowPos(window, x, y);
    win.window = window;
    return win;
}
#endif
void updateWindowTitle(WREwindow *window, char *name)
{
    window->name = name;
    glfwSetWindowTitle(window->window, name);
}
void updateWindowPosition(WREwindow *window, int x, int y)
{
    glfwSetWindowPos(window->window, x, y);
#ifdef WREUSEVULKAN
    window->context.x = x;
    window->context.y = y;
#endif
}
void updateWindowSize(WREwindow *window, int w, int h)
{
    glfwSetWindowSize(window->window, w, h);
#ifdef WREUSEVULKAN
    window->context.w = w;
    window->context.h = h;
#endif
}
WREmonitor getMonitorInfo()
{
    WREGLFWINIT
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    return (WREmonitor){
        monitor,
        mode->width,
        mode->height,
        mode->refreshRate,
        (float)mode->width / mode->height,
    };
}
#undef WREGLFWINIT