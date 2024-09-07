#include <rendergraph.h>
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