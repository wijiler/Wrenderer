#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <windowing.h>

typedef struct
{
    float screensize[2];
} pushConstants;

RenderPass pass1;
renderer_t renderer;
winf_t wininfo = {0};
pushConstants pc = {0};
GraphBuilder builder = {0};

float verts[3][3] = {
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {0.0f, -1.0f, 1.0f},
};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void helloTriangleCallback(RenderPass pass, VkCommandBuffer cBuf)
{
    bindPipeline(pass.pl, cBuf);

    pc.screensize[0] = renderer.vkCore.extent.width;
    pc.screensize[1] = renderer.vkCore.extent.height;

    vkCmdPushConstants(cBuf, pass.pl.plLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, pass.pl.pcRange.size, pass.pl.PushConstants);

    vkCmdDraw(cBuf, 3, 1, 0, 0);

    unBindPipeline(cBuf);
}
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;

    renderer.rg = &builder;

    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    uint64_t vLen = 0;
    uint64_t fLen = 0;
    readShaderSPRV("./shaders/vfractal.spv", &vLen, NULL);
    readShaderSPRV("./shaders/ffractal.spv", &fLen, NULL);
    uint32_t *vShader = malloc(sizeof(uint32_t) * vLen);
    uint32_t *fShader = malloc(sizeof(uint32_t) * fLen);

    readShaderSPRV("./shaders/vfractal.spv", &vLen, vShader);
    readShaderSPRV("./shaders/ffractal.spv", &fLen, fShader);

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
    pl.depthBoundsEnable = VK_FALSE;
    pl.alphaToOneEnable = VK_TRUE;
    pl.sampleMask = UINT32_MAX;

    setShaderSPRV(renderer.vkCore, &pl, vShader, vLen, fShader, fLen);

    setPushConstantRange(&pl, sizeof(pushConstants), SHADER_STAGE_FRAGMENT);

    pl.PushConstants = &pc;

    VkPipelineLayoutCreateInfo plcInf = {0};
    plcInf.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plcInf.pNext = NULL;

    plcInf.setLayoutCount = 0;
    plcInf.pSetLayouts = NULL;

    plcInf.pPushConstantRanges = &pl.pcRange;
    plcInf.pushConstantRangeCount = pl.pcRangeCount;
    vkCreatePipelineLayout(renderer.vkCore.lDev, &plcInf, NULL, &pl.plLayout);

    pass1 = newPass((char *)"name1", PASS_TYPE_GRAPHICS);

    pass1.pl = pl;
    addImageResource(&pass1, renderer.vkCore.currentScImg, USAGE_COLORATTACHMENT);

    setExecutionCallBack(&pass1, helloTriangleCallback);
    addPass(&builder, &pass1);
}
int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1920;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}