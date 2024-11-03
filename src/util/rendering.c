#include <util/util.h>

graphicsPipeline spritePipeline = {0};
static const int spriteIncrementAmount = 100;

void initializePipelines(renderer_t renderer)
{
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
        spritePipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        spritePipeline.colorBlending = VK_TRUE;
        spritePipeline.logicOpEnable = VK_FALSE;
        spritePipeline.reasterizerDiscardEnable = VK_FALSE;
        spritePipeline.polyMode = VK_POLYGON_MODE_FILL;
        spritePipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        spritePipeline.primitiveRestartEnable = VK_FALSE;
        spritePipeline.depthBiasEnable = VK_FALSE;
        spritePipeline.depthTestEnable = VK_FALSE;
        spritePipeline.depthClampEnable = VK_FALSE;
        spritePipeline.depthClipEnable = VK_FALSE;
        spritePipeline.stencilTestEnable = VK_FALSE;
        spritePipeline.alphaToCoverageEnable = VK_FALSE;
        spritePipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        spritePipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        spritePipeline.cullMode = VK_CULL_MODE_NONE;
        spritePipeline.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        spritePipeline.depthBoundsEnable = VK_FALSE;
        spritePipeline.alphaToOneEnable = VK_TRUE;
        spritePipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            VkDeviceAddress SpriteBuffer;
            VkDeviceAddress InstanceBuffer;
            VkDeviceAddress LightBuffer;
        } pc;
        setPushConstantRange(&spritePipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        createPipelineLayout(renderer.vkCore, &spritePipeline);
        setShaderSLSPRV(renderer.vkCore, &spritePipeline, Shader, Len);
    }
}
