#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <windowing.h>

RenderPass pass1;
renderer_t renderer;
winf_t wininfo = {0};
GraphBuilder builder = {0};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void helloTriangleCallback(RenderPass pass, VkCommandBuffer cBuf)
{
    bindPipeline(pass.pl, cBuf);

    vkCmdDraw(cBuf, 3, 1, 0, 0);

    unBindPipeline(cBuf);
}
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;

    addPass(&builder, &pass1);

    renderer.rg = &builder;

    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    uint64_t vLen, fLen = 0;
    readShaderSPRV("./shaders/vphong.spv", &vLen, NULL);
    readShaderSPRV("./shaders/fphong.spv", &fLen, NULL);
    uint32_t *vShader = malloc(sizeof(uint32_t) * vLen);
    uint32_t *fShader = malloc(sizeof(uint32_t) * fLen);

    readShaderSPRV("./shaders/vphong.spv", &vLen, vShader);
    readShaderSPRV("./shaders/fphong.spv", &fLen, fShader);

    // printf("%08" PRIx32 "\n", vShader[0]);

    Pipeline pl = {0};

    pl.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pl.colorBlending = VK_TRUE;
    pl.logicOp = VK_FALSE;
    pl.reasterizerDiscardEnable = VK_FALSE;
    pl.polyMode = VK_POLYGON_MODE_FILL;
    pl.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pl.primitiveRestartEnable = VK_FALSE;
    pl.depthBiasEnable = VK_FALSE;
    pl.depthTestEnable = VK_FALSE;
    pl.depthClampEnable = VK_FALSE;
    pl.depthClipEnable = VK_FALSE;
    pl.stencilTestEnable = VK_FALSE;
    pl.alphaToCoverageEnable = VK_FALSE;
    pl.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
    pl.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pl.cullMode = VK_CULL_MODE_NONE;
    pl.colorBlendEq = (VkColorBlendEquationEXT){
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
    };
    // pl.minDepth = 0;
    // pl.maxDepth = 1;
    pl.depthBoundsEnable = VK_FALSE;
    pl.alphaToOneEnable = VK_TRUE;
    pl.sampleMask = UINT32_MAX;

    setShaderSPRV(renderer.vkCore, &pl, vShader, vLen, fShader, fLen);

    BufferCreateInfo cInf = {0};
    cInf.access = HOST_ACCESS;
    cInf.dataSize = 160;
    cInf.usage = BUFFER_USAGE_VERTEX;

    pass1 = newPass((char *)"name1", PASS_TYPE_GRAPHICS);

    pass1.pl = pl;
    addImageResource(&pass1, renderer.vkCore.currentScImg, USAGE_COLORATTACHMENT);

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