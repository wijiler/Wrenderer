#include <util/util.h>

SpritePipeline spritePipeline = {0};
static const int spriteIncrementAmount = 100;

void initializePipelines(renderer_t renderer)
{
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
        spritePipeline.gbufferPass.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        spritePipeline.gbufferPass.colorBlending = VK_TRUE;
        spritePipeline.gbufferPass.logicOpEnable = VK_FALSE;
        spritePipeline.gbufferPass.reasterizerDiscardEnable = VK_FALSE;
        spritePipeline.gbufferPass.polyMode = VK_POLYGON_MODE_FILL;
        spritePipeline.gbufferPass.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        spritePipeline.gbufferPass.primitiveRestartEnable = VK_FALSE;
        spritePipeline.gbufferPass.depthBiasEnable = VK_FALSE;
        spritePipeline.gbufferPass.depthTestEnable = VK_FALSE;
        spritePipeline.gbufferPass.depthClampEnable = VK_FALSE;
        spritePipeline.gbufferPass.depthClipEnable = VK_FALSE;
        spritePipeline.gbufferPass.stencilTestEnable = VK_FALSE;
        spritePipeline.gbufferPass.alphaToCoverageEnable = VK_FALSE;
        spritePipeline.gbufferPass.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        spritePipeline.gbufferPass.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        spritePipeline.gbufferPass.cullMode = VK_CULL_MODE_NONE;
        spritePipeline.gbufferPass.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        spritePipeline.gbufferPass.depthBoundsEnable = VK_FALSE;
        spritePipeline.gbufferPass.alphaToOneEnable = VK_TRUE;
        spritePipeline.gbufferPass.sampleMask = UINT32_MAX;
        typedef struct
        {
            VkDeviceAddress SpriteBuffer;
            VkDeviceAddress InstanceBuffer;
        } pc;
        setPushConstantRange(&spritePipeline.gbufferPass, sizeof(pc), SHADER_STAGE_ALL, 0);
        addSetLayoutToGPL(&renderer.vkCore.tdSetLayout, &spritePipeline.gbufferPass);
        addDescriptorSetToGPL(&renderer.vkCore.tdescriptorSet, &spritePipeline.gbufferPass);

        setShaderSLSPRV(renderer.vkCore, &spritePipeline.gbufferPass, Shader, Len);

        createPipelineLayout(renderer.vkCore, &spritePipeline.gbufferPass);
    }
    {
    }
}
