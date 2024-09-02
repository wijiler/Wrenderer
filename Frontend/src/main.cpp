#include <renderer.h>
#include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo{};

void loop()
{
}

void init()
{
    initRenderer(&renderer);

    Pipeline pl{};
    Pipeline pl2{};
    pl2.depthBiasEnable = VK_TRUE;
    cache_PipeLine(&pl, (char *)"dummy");
    cache_PipeLine(&pl2, (char *)"dummy2");
    Pipeline pl3 = find_Pipeline((char *)"dummy2");
    printf("%i\n", pl3.depthBiasEnable);
}
int main(void)
{
    wininfo.name = (char *)"DUMBSHIT";
    wininfo.w = 1920;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, (void *)loop, (void *)init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}