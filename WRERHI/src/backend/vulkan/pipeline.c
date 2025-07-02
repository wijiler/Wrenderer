#include <backends/vulkan/debug.h>
#include <backends/vulkan/globals.h>
#include <backends/vulkan/pipeline.h>
#include <stdio.h>

// is this ideal? no is it fine? ya
VkPipelineShaderStageCreateInfo *genCreateInfo(WREshader *shader)
{
    VkPipelineShaderStageCreateInfo *cInf = malloc(sizeof(VkPipelineShaderStageCreateInfo) * 2);
    if ((shader->shaderStage & WRE_SHADER_STAGE_COMPUTE) != 0)
    {
        VkPipelineShaderStageCreateInfo cshad = {0};
        cshad.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        cshad.pNext = NULL;
        cshad.stage = VK_SHADER_STAGE_COMPUTE_BIT;

        cshad.pName = "compMain";
        cshad.module = shader->shaderObjects.Shader;
        cInf[0] = cshad;
        return cInf;
    }
    if ((shader->shaderStage & WRE_SHADER_STAGE_VERTEX) != 0)
    {
        cInf[0] = (VkPipelineShaderStageCreateInfo){
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            NULL,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            shader->shaderObjects.Shader,
            "vertMain",
            NULL,
        };
    }
    if ((shader->shaderStage & WRE_SHADER_STAGE_FRAGMENT) != 0)
    {
        cInf[1] = (VkPipelineShaderStageCreateInfo){
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            NULL,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            shader->shaderObjects.Shader,
            "fragMain",
            NULL,
        };
    }
    return cInf;
}

WREVKPipeline createPipeline(char *Name, const WREVkPipelineCI *create_info)
{
    WREVKPipeline pipeline = {0};
    pipeline.Name = Name;

    VkPushConstantRange ranges[2] = {{0}, {0}};
    uint32_t pcRangeCount = 0;
    uint32_t combined_descriptor_count = create_info->shaders[0].descriptorSetCount + (create_info->shaderCount > 1 ? 0 : create_info->shaders[1].descriptorSetCount);
    VkDescriptorSetLayout *setLayouts = malloc(sizeof(VkDescriptorSetLayout) * (combined_descriptor_count + 1));

    for (uint32_t i = 0; i < create_info->shaderCount; i++)
    {
        WREshader shader = create_info->shaders[i];

        if (i == 0)
            memcpy(setLayouts, shader.sets, create_info->shaders[0].descriptorSetCount * sizeof(VkDescriptorSetLayout));
        else if (i == 1)
            memcpy(setLayouts + create_info->shaders[0].descriptorSetCount, shader.sets, create_info->shaders[1].descriptorSetCount * sizeof(VkDescriptorSetLayout));

        if (shader.pushConstantsSize > 0)
        {
            ranges[pcRangeCount] = (VkPushConstantRange){
                shader.shaderStage,
                0,
                128,
            };
            pcRangeCount += 1;
        }
    }
    setLayouts[combined_descriptor_count] = WREbindlessDescriptorLayout;
    VkPipelineLayoutCreateInfo pLineLayout = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = combined_descriptor_count + 1,
        .pSetLayouts = setLayouts,
        .pushConstantRangeCount = pcRangeCount,
        .pPushConstantRanges = ranges,
    };

    VkResult result = vkCreatePipelineLayout(WREdevice, &pLineLayout, NULL, &pipeline.layout);
    if (result != VK_SUCCESS)
    {
        printf("Wreren: Error: Could not create pipeline layout %s\n", Name);
    }

    VkGraphicsPipelineCreateInfo pLineCI = {0};
    pLineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pLineCI.stageCount = 2;
    VkPipelineShaderStageCreateInfo pShadCI[2] = {0};
    if (create_info->shaderCount == 1)
    {
        VkPipelineShaderStageCreateInfo *shad_sci = genCreateInfo(&create_info->shaders[0]);
        pShadCI[0] = shad_sci[0];
        pShadCI[1] = shad_sci[1];
        free(shad_sci);
    }
    else if (create_info->shaderCount > 1)
    {
        VkPipelineShaderStageCreateInfo *shad_sci1 = genCreateInfo(&create_info->shaders[0]);
        VkPipelineShaderStageCreateInfo *shad_sci2 = genCreateInfo(&create_info->shaders[1]);
        pShadCI[0] = shad_sci1[0];
        pShadCI[1] = shad_sci2[0];
        free(shad_sci1);
        free(shad_sci2);
    }
    pLineCI.pStages = pShadCI;

    const VkPipelineDynamicStateCreateInfo dynStateCI = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        NULL,
        0,
        2,
        (VkDynamicState[]){
            VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
            VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
        },
    };
    pLineCI.pDynamicState = &dynStateCI;

    VkPipelineColorBlendAttachmentState attStateCI = {
        VK_TRUE,
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo cBlendCI = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        NULL,
        0,
        VK_FALSE,
        VK_LOGIC_OP_COPY,
        1,
        &attStateCI,
        {VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
         VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
         VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
         VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT},
    };
    pLineCI.pColorBlendState = &cBlendCI;

    const VkPipelineVertexInputStateCreateInfo vInStateCI = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        NULL,
        0,
        create_info->vertFormat.bindingCount,
        create_info->vertFormat.bindings,
        create_info->vertFormat.attribCount,
        create_info->vertFormat.attributes,
    };

    pLineCI.pVertexInputState = &vInStateCI;

    const VkPipelineRasterizationStateCreateInfo rastStateCI = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        NULL,
        0,
        VK_FALSE,
        VK_FALSE,
        VK_POLYGON_MODE_FILL,
        create_info->cullMode,
        (VkFrontFace)create_info->windingOrder,
        VK_FALSE,
        0,
        0.f,
        0.f,
        1,
    };

    pLineCI.pRasterizationState = &rastStateCI;

    const VkPipelineInputAssemblyStateCreateInfo inAsmStateCI = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        NULL,
        0,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_FALSE,
    };

    const VkPipelineMultisampleStateCreateInfo msaaCI = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        NULL,
        0,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FALSE,
        1.0f,
        NULL,
        VK_FALSE,
        VK_FALSE,
    };
    pLineCI.pMultisampleState = &msaaCI;

    const VkPipelineRenderingCreateInfo renCI = {
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        NULL,
        0,
        create_info->colorAttachmentsCount,
        create_info->colorAttFormats,
        0,
        0,
    };

    pLineCI.pNext = &renCI;

    pLineCI.pInputAssemblyState = &inAsmStateCI;
    pLineCI.layout = pipeline.layout;
    result = vkCreateGraphicsPipelines(WREdevice, NULL, 1, &pLineCI, NULL, &pipeline.pipeline);
    if (result != VK_SUCCESS)
    {
        printf("Wreren: Error: could not create pipeline %s\n", Name);
    }
    setVkDebugName(Name, VK_OBJECT_TYPE_PIPELINE, (uint64_t)pipeline.pipeline);

    return pipeline;
}
