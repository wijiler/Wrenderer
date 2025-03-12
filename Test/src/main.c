#include <renderer.h>
#include <windowing.h>
WRErenderer renderer = {0};
int main()
{
    Wremonitor monitor = getMonitorInfo();
    WREwindow window = openWindow("Name", monitor.w / 7, monitor.h / 7, 1920, 1080);
    renderer.window = window;
    initializeRenderer(&renderer);

    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();
    }
    terminateRenderer(&renderer);

    return 0;
}