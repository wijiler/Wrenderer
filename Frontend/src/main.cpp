#include <rendergraph.h>
#include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo{};
GraphBuilder builder{0, NULL, renderer};

void loop()
{
}

void init()
{
    initRenderer(&renderer);

    Pipeline pl{};

    BufferCreateInfo cInf{};
    cInf.access = HOST_ACCESS;
    cInf.dataSize = 160;
    cInf.usage = BUFFER_USAGE_VERTEX;

    builder.renderer = renderer;
    Image scImg{
        renderer.vkCore.swapChainImages[0],
        renderer.vkCore.swapChainImageViews[0],
        VK_IMAGE_LAYOUT_UNDEFINED,
    };
    Buffer buf;
    createBuffer(renderer.vkCore, &cInf, &buf);

    RenderPass pass1 = newPass("name", pl);
    RenderPass pass2 = newPass("name2", pl);

    addBufferResource(&pass1, buf, USAGE_TRANSFER_DST);
    addBufferResource(&pass2, buf, USAGE_TRANSFER_SRC);
    addImageResource(&pass2, scImg, USAGE_TRANSFER_DST);

    // printf("%i\n", pass1.resources[1].access & ACCESS_TRANSFER_WRITE);

    addPass(&builder, &pass1, PASS_TYPE_GRAPHICS);
    addPass(&builder, &pass2, PASS_TYPE_GRAPHICS);

    RenderGraph graph = buildGraph(&builder, scImg);

    printf("pCount %i\n", graph.passCount);
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