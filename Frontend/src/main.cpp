#include <renderer.h>
#include <stdio.h>
#include <windowing.h>

RenderPass pass1;
RenderPass pass2;
RenderPass pass3;
RenderPass pass4;
renderer_t renderer;
winf_t wininfo{};
GraphBuilder builder{};

void loop()
{
}

void init()
{
    Shader Vert, Frag;

    Pipeline pl{

    };
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