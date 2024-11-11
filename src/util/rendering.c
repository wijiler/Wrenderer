#include <util/util.h>

SpritePipeline spritePipeline = {0};
static const int spriteIncrementAmount = 100;

void initializePipelines(renderer_t renderer)
{
    {
        initializeDescriptor(renderer, &spritePipeline.gBuffer, 1, 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, SHADER_STAGE_ALL, true);
        spritePipeline.Albedo = createImage(renderer.vkCore, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TYPE_2D, renderer.vkCore.extent.width, renderer.vkCore.extent.height, VK_IMAGE_ASPECT_COLOR_BIT);
        // markImageResizable(&spritePipeline.Albedo, &renderer.vkCore.extent.width, &renderer.vkCore.extent.height);
        allocateDescriptorSets(renderer, &spritePipeline.gBuffer);
        writeDescriptorSet(renderer, spritePipeline.gBuffer, 0, 0, spritePipeline.Albedo.imgview, renderer.vkCore.linearSampler);

        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
        spritePipeline.gbufferPipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        spritePipeline.gbufferPipeline.colorBlending = VK_TRUE;
        spritePipeline.gbufferPipeline.logicOpEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.reasterizerDiscardEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.polyMode = VK_POLYGON_MODE_FILL;
        spritePipeline.gbufferPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        spritePipeline.gbufferPipeline.primitiveRestartEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.depthBiasEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.depthTestEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.depthClampEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.depthClipEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.stencilTestEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.alphaToCoverageEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        spritePipeline.gbufferPipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        spritePipeline.gbufferPipeline.cullMode = VK_CULL_MODE_NONE;
        spritePipeline.gbufferPipeline.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        spritePipeline.gbufferPipeline.depthBoundsEnable = VK_FALSE;
        spritePipeline.gbufferPipeline.alphaToOneEnable = VK_TRUE;
        spritePipeline.gbufferPipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            VkDeviceAddress SpriteBuffer;
            VkDeviceAddress InstanceBuffer;
        } pc;
        setPushConstantRange(&spritePipeline.gbufferPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addSetLayoutToGPL(&renderer.vkCore.tdSetLayout, &spritePipeline.gbufferPipeline);
        addDescriptorSetToGPL(&renderer.vkCore.tdescriptorSet, &spritePipeline.gbufferPipeline);

        setShaderSLSPRV(renderer.vkCore, &spritePipeline.gbufferPipeline, Shader, Len);

        createPipelineLayout(renderer.vkCore, &spritePipeline.gbufferPipeline);
    }
    {
        uint64_t Len = 0;
        uint32_t *Shader = NULL;

        readShaderSPRV("./shaders/spriteLighting.spv", &Len, &Shader);
        spritePipeline.lightPipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        spritePipeline.lightPipeline.colorBlending = VK_TRUE;
        spritePipeline.lightPipeline.logicOpEnable = VK_FALSE;
        spritePipeline.lightPipeline.reasterizerDiscardEnable = VK_FALSE;
        spritePipeline.lightPipeline.polyMode = VK_POLYGON_MODE_FILL;
        spritePipeline.lightPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        spritePipeline.lightPipeline.primitiveRestartEnable = VK_FALSE;
        spritePipeline.lightPipeline.depthBiasEnable = VK_FALSE;
        spritePipeline.lightPipeline.depthTestEnable = VK_FALSE;
        spritePipeline.lightPipeline.depthClampEnable = VK_FALSE;
        spritePipeline.lightPipeline.depthClipEnable = VK_FALSE;
        spritePipeline.lightPipeline.stencilTestEnable = VK_FALSE;
        spritePipeline.lightPipeline.alphaToCoverageEnable = VK_FALSE;
        spritePipeline.lightPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
        spritePipeline.lightPipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        spritePipeline.lightPipeline.cullMode = VK_CULL_MODE_NONE;
        spritePipeline.lightPipeline.colorBlendEq = (VkColorBlendEquationEXT){
            VK_BLEND_FACTOR_SRC_ALPHA,
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
        };
        spritePipeline.lightPipeline.depthBoundsEnable = VK_FALSE;
        spritePipeline.lightPipeline.alphaToOneEnable = VK_TRUE;
        spritePipeline.lightPipeline.sampleMask = UINT32_MAX;
        typedef struct
        {
            uint32_t lightCount;
            VkDeviceAddress lightBuf;
            VkDeviceAddress instanceBuf;
        } pc;
        setPushConstantRange(&spritePipeline.lightPipeline, sizeof(pc), SHADER_STAGE_ALL, 0);
        addSetLayoutToGPL(&spritePipeline.gBuffer.layout, &spritePipeline.lightPipeline);
        addDescriptorSetToGPL(&spritePipeline.gBuffer.sets[0].set, &spritePipeline.lightPipeline);

        setShaderSLSPRV(renderer.vkCore, &spritePipeline.lightPipeline, Shader, Len);

        createPipelineLayout(renderer.vkCore, &spritePipeline.lightPipeline);
    }
}
