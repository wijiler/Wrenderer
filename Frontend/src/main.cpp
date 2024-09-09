#include <rendergraph.h>
#include <stdio.h>
#include <windowing.h>

RenderPass pass1;
RenderPass pass2;
RenderPass pass3;
RenderPass pass4;
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
    Buffer buf2;
    cInf.usage = BUFFER_USAGE_INDEX;
    createBuffer(renderer.vkCore, &cInf, &buf2);

    RenderPass pass1 = newPass((char *)"name1", PASS_TYPE_GRAPHICS);
    RenderPass pass2 = newPass((char *)"name2", PASS_TYPE_GRAPHICS);
    RenderPass pass3 = newPass((char *)"name3", PASS_TYPE_GRAPHICS);
    RenderPass pass4 = newPass((char *)"name4", PASS_TYPE_GRAPHICS);

    addBufferResource(&pass1, buf, USAGE_TRANSFER_DST);
    addBufferResource(&pass2, buf, USAGE_TRANSFER_SRC);
    addImageResource(&pass2, scImg, USAGE_TRANSFER_DST);

    addPass(&builder, &pass1);
    addPass(&builder, &pass2);
    addPass(&builder, &pass4);
    addPass(&builder, &pass3);

    RenderGraph graph = buildGraph(&builder, scImg);

    printf("%i\n", graph.passCount);

    destroyRenderGraph(&graph);
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