#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo = {0};
GraphBuilder builder = {0};

float verts[3][3] = {
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {0.0f, -1.0f, 1.0f},
};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;

    drawRenderer(&renderer, Index);
}

void init()
{
    initRenderer(&renderer);
    uint64_t vLen, fLen = 0;
    uint32_t *vShader = NULL;
    uint32_t *fShader = NULL;

    readShaderSPRV("./shaders/vtri.spv", &vLen, &vShader);
    readShaderSPRV("./shaders/ftri.spv", &fLen, &fShader);

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

    addVertexInput(&pl, 0, 0, sizeof(float) * 3, 0, VK_VERTEX_INPUT_RATE_VERTEX, VK_FORMAT_R32G32B32_SFLOAT);

    uint32_t indices[3] = {0, 1, 2};

    renderer.meshHandler.vertexSize = sizeof(float[3]);
    Mesh triangle = createMesh(renderer, 3, verts, 3, indices, 1);
    submitMesh(triangle, &renderer);

    RenderPass scenePass = sceneDraw(&renderer);
    scenePass.pl = pl;

    addPass(&builder, &scenePass);
    renderer.rg = &builder;
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