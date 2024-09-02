#include <renderer.h>
// #include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo = {0};

void loop()
{
}

void init()
{
    initRenderer(&renderer);

    // Pipeline pl = {0};
    // Pipeline pl2 = {0};
    // pl2.depthBiasEnable = VK_TRUE;
    // cache_PipeLine(&pl, "dummy");
    // cache_PipeLine(&pl2, "dummy2");
    // Pipeline pl3 = find_Pipeline("dummy2");
    // printf("%i\n", pl3.depthBiasEnable);
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