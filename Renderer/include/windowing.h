#ifndef WREWIN_H__
#define WREWIN_H__
#define GLFW_INCLUDE_VULKAN
#include <deps/GLFW/glfw3.h>
#ifdef WREUSEVULKAN
#include <backends/vulkan/WREVulk.h>
#endif
typedef struct
{
    RendererWindowContext context;
    char *name;
    GLFWwindow *window;
} WREwindow;

typedef struct
{
    GLFWmonitor *monitor;
    int w, h, refreshRate;
    float aspectRatio;
} Wremonitor;

WREwindow openWindow(char *Title, int x, int y, int w, int h);
Wremonitor getMonitorInfo();
void updateWindowTitle(WREwindow *window, char *name);
void updateWindowPosition(WREwindow *window, int x, int y);
void updateWindowSize(WREwindow *window, int w, int h);

#endif
