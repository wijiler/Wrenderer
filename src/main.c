#include <windowing.h>
#include <renderer.h>
#include <stdio.h>
renderer_t renderer;
winf_t wininfo = {0};

void loop()
{
}

void init()
{
    initRenderer(&renderer);
}
int main(void)
{
    wininfo.name = "DUMBSHIT";
    wininfo.w = 1920;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}