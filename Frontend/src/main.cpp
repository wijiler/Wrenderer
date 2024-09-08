#include <rendergraph.h>
#include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo{};
GraphBuilder builder{0, NULL, renderer};

void loop()
{
    Image scImg{
        renderer.vkCore.swapChainImages[0],
        renderer.vkCore.swapChainImageViews[0],
        VK_IMAGE_LAYOUT_UNDEFINED,
    };
    RenderGraph graph = buildGraph(&builder, scImg);
    destroyRenderGraph(&graph);
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

    addBufferResource(&pass1, buf.index, USAGE_TRANSFER_DST);
    addImageResource(&pass1, scImg, USAGE_TRANSFER_DST);

    // printf("%i\n", pass1.resources[1].access & ACCESS_TRANSFER_WRITE);

    addPass(&builder, &pass1, PASS_TYPE_GRAPHICS);
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