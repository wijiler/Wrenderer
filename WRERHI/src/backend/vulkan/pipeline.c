#include <backends/vulkan/debug.h>
#include <backends/vulkan/globals.h>
#include <backends/vulkan/pipeline.h>
#include <stdio.h>

WREVKPipeline createPipeline(char *Name, WREvertexFormat vertFormat, WREShader *shaders, int shaderCount, WREpipelineCullMode cullMode, WREpipelineWindingOrder windingOrder, VkFormat colorAttFormats[8], uint32_t colorAttachmentsCount)
{
    WREVKPipeline pipeline = {0};
    pipeline.Name = Name;

    VkPipelineLayoutCreateInfo pLineLayout = {0};
    pLineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pLineLayout.setLayoutCount = 0;
    pLineLayout.pushConstantRangeCount = 0;

    VkResult result = vkCreatePipelineLayout(WREDevice, &pLineLayout, NULL, &pipeline.layout);
    if (result != VK_SUCCESS)
    {
        printf("Wreren: Error: Could not create pipeline layout %s\n", Name);
    }

    VkGraphicsPipelineCreateInfo pLineCI = {0};
    pLineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pLineCI.stageCount = 2;
    VkPipelineShaderStageCreateInfo pShadCI[2] = {0};
    if (shaderCount == 1)
    {
        pShadCI[0] = shaders[0].shaderObjects.stageInfo[0];
        pShadCI[1] = shaders[0].shaderObjects.stageInfo[1];
    }
    else if (shaderCount > 1)
    {
        pShadCI[0] = shaders[0].shaderObjects.stageInfo[0];
        pShadCI[1] = shaders[1].shaderObjects.stageInfo[0];
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
        vertFormat.bindingCount,
        vertFormat.bindings,
        vertFormat.attribCount,
        vertFormat.attributes,
    };

    pLineCI.pVertexInputState = &vInStateCI;

    const VkPipelineRasterizationStateCreateInfo rastStateCI = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        NULL,
        0,
        VK_FALSE,
        VK_FALSE,
        VK_POLYGON_MODE_FILL,
        cullMode,
        (VkFrontFace)windingOrder,
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
        1,
        colorAttFormats,
        0,
        0,
    };

    pLineCI.pNext = &renCI;

    pLineCI.pInputAssemblyState = &inAsmStateCI;
    pLineCI.layout = pipeline.layout;
    result = vkCreateGraphicsPipelines(WREDevice, NULL, 1, &pLineCI, NULL, &pipeline.pipeline);
    if (result != VK_SUCCESS)
    {
        printf("Wreren: Error: could not create pipeline %s\n", Name);
    }
    setVkDebugName(Name, VK_OBJECT_TYPE_PIPELINE, (uint64_t)pipeline.pipeline);

    return pipeline;
}
