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
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
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

    uint64_t Len = 0;
    uint32_t *Shader = NULL;

    readShaderSPRV("./shaders/tri.spv", &Len, &Shader);

    graphicsPipeline pl = {0};

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

    setPushConstantRange(&pl, sizeof(pushConstants), SHADER_STAGE_VERTEX);

    createPipelineLayout(renderer.vkCore, &pl);

    setShaderSLSPRV(renderer.vkCore, &pl, Shader, Len);

    uint32_t indices[3] = {0, 1, 2};

    Mesh triangle = createMesh(renderer, 3, verts, 3, indices, 1);
    submitMesh(triangle, &renderer);

    RenderPass scenePass = sceneDraw(&renderer);
    scenePass.gPl = pl;
    VkOffset2D offSet = {0, 0};
    scenePass.drawArea = (drawArea){
        &offSet,
        &renderer.vkCore.extent,
    };
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