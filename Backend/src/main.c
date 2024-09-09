#include <rendergraph.h>
#include <stdio.h>
#include <time.h>
#include <windowing.h>

renderer_t renderer;
GraphBuilder builder = {0};
winf_t wininfo = {0};
Pipeline pl = {0};
RenderPass pass = {0};
RenderPass pass2 = {0};

void loop()
{
    Image scImg = {renderer.vkCore.swapChainImages[0], renderer.vkCore.swapChainImageViews[0], VK_IMAGE_LAYOUT_UNDEFINED};

    pass = newPass((char *)"pass1", PASS_TYPE_GRAPHICS);
    pass2 = newPass((char *)"pass2", PASS_TYPE_GRAPHICS);

    addImageResource(&pass, scImg, USAGE_COLORATTACHMENT);
    addImageResource(&pass2, scImg, USAGE_SAMPLED);

    addPass(&builder, &pass);
    addPass(&builder, &pass2);
    RenderGraph graph = buildGraph(&builder, scImg);

    free(pass.colorAttachments);
    free(pass2.colorAttachments);
    free(pass.resources);
    free(pass2.resources);
    free(graph.passes);
    graph.passCount = 0;
}

void init()
{
    initRenderer(&renderer);

    builder.renderer = renderer;
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