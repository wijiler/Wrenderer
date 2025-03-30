#include <backends/vulkan/globals.h>
#include <backends/vulkan/pipeline.h>
#include <stdio.h>

WREpipeline createPipeline(char *Name, WREvertexFormat vertFormat, WREShader *shaders, int shaderCount, WREpipelineCullMode cullMode, WREpipelineWindingOrder windingOrder)
{
    WREpipeline pipeline = {0};
    pipeline.Name = Name;

    VkPipelineLayoutCreateInfo pLineLayout = {0};
    pLineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pLineLayout.setLayoutCount = 0;
    pLineLayout.pushConstantRangeCount = 0;

    VkResult result = vkCreatePipelineLayout(WREDevice, &pLineLayout, NULL, &pipeline.gpuObjects.layout);
    if (result != VK_SUCCESS)
    {
        printf("Wreren: Error: Could not create pipeline layout %s\n", Name);
    }

    VkGraphicsPipelineCreateInfo pLineCI = {0};
    pLineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pLineCI.stageCount = 2;
    if (shaderCount == 1)
    {
        pLineCI.pStages = (VkPipelineShaderStageCreateInfo[2]){shaders[0].shaderObjects.stageInfo[0], shaders[0].shaderObjects.stageInfo[1]};
    }
    else if (shaderCount > 1)
    {
        pLineCI.pStages = (VkPipelineShaderStageCreateInfo[2]){shaders[0].shaderObjects.stageInfo[0], shaders[1].shaderObjects.stageInfo[0]};
    }

    const VkPipelineDynamicStateCreateInfo dynStateCI = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        NULL,
        0,
        5,
        (VkDynamicState[]){
            VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
            VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
            VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
            VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
            VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT,
        },
    };

    pLineCI.pDynamicState = &dynStateCI;

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

    pLineCI.pInputAssemblyState = &inAsmStateCI;
    pLineCI.layout = pipeline.gpuObjects.layout;
    result = vkCreateGraphicsPipelines(WREDevice, NULL, 1, &pLineCI, NULL, &pipeline.gpuObjects.pipeline);
    if (result != VK_SUCCESS)
    {
        printf("Wreren: Error: could not create pipeline %s\n", Name);
    }

    return pipeline;
}
