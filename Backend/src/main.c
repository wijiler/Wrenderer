#include <renderer.h>
#include <windowing.h>

RenderPass pass1;
renderer_t renderer;
winf_t wininfo = {0};
GraphBuilder builder = {0};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;
    addPass(&builder, &pass1);

    drawRenderer(&renderer, Index);
}

void helloTriangleCallback(RenderPass pass, VkCommandBuffer cBuf)
{
}

void init()
{
    initRenderer(&renderer);

    Pipeline pl = {0};

    BufferCreateInfo cInf = {0};
    cInf.access = HOST_ACCESS;
    cInf.dataSize = 160;
    cInf.usage = BUFFER_USAGE_VERTEX;

    pass1 = newPass((char *)"name1", PASS_TYPE_GRAPHICS);

    addImageResource(&pass1, renderer.vkCore.currentScImg, USAGE_TRANSFER_DST);

    setExecutionCallBack(&pass1, helloTriangleCallback);
}
int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1920;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, (void *)loop, (void *)init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}