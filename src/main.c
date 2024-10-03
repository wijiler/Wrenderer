#include <inttypes.h>
#include <renderer.h>
#include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo = {0};
GraphBuilder builder = {0};

typedef struct
{
    VkDeviceAddress address;
} pushConstants;

typedef struct
{
    float position[3];
    float color[3];
} vertex;

vertex verts[3] = {
    {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
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
    renderer.meshHandler.vertexSize = sizeof(vertex);
    initRenderer(&renderer);
    uint64_t vLen, fLen = 0;
    uint32_t *vShader = NULL;
    uint32_t *fShader = NULL;

    readShaderSPRV("./shaders/vtri.spv", &vLen, &vShader);
    readShaderSPRV("./shaders/ftri.spv", &fLen, &fShader);

    Pipeline pl = {0};

    pl.value.graphics.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pl.value.graphics.colorBlending = VK_TRUE;
    pl.value.graphics.logicOp = VK_FALSE;
    pl.value.graphics.reasterizerDiscardEnable = VK_FALSE;
    pl.value.graphics.polyMode = VK_POLYGON_MODE_FILL;
    pl.value.graphics.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pl.value.graphics.primitiveRestartEnable = VK_FALSE;
    pl.value.graphics.depthBiasEnable = VK_FALSE;
    pl.value.graphics.depthTestEnable = VK_FALSE;
    pl.value.graphics.depthClampEnable = VK_FALSE;
    pl.value.graphics.depthClipEnable = VK_FALSE;
    pl.value.graphics.stencilTestEnable = VK_FALSE;
    pl.value.graphics.alphaToCoverageEnable = VK_FALSE;
    pl.value.graphics.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
    pl.value.graphics.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pl.value.graphics.cullMode = VK_CULL_MODE_NONE;
    pl.value.graphics.colorBlendEq = (VkColorBlendEquationEXT){
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
    };
    pl.value.graphics.depthBoundsEnable = VK_FALSE;
    pl.value.graphics.alphaToOneEnable = VK_TRUE;
    pl.value.graphics.sampleMask = UINT32_MAX;

    setShaderSPRV(renderer.vkCore, &pl, vShader, vLen, fShader, fLen);

    uint32_t indices[3] = {0, 1, 2};

    Mesh triangle = createMesh(renderer, 3, verts, 3, indices, 1);
    submitMesh(triangle, &renderer);

    setPushConstantRange(&pl, sizeof(pushConstants), SHADER_STAGE_VERTEX);

    createPipelineLayout(renderer.vkCore, &pl);
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