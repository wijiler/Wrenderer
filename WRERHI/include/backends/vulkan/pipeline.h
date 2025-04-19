#ifndef WREVULKPIPELINE_H__
#define WREVULKPIPELINE_H__
#include <backends/vulkan/shader.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct
{
    VkPipelineLayout layout;
    VkPipeline pipeline;
} WREPipelineObjects;

typedef enum
{
    COMPUTE = VK_PIPELINE_BIND_POINT_COMPUTE,
    RENDER = VK_PIPELINE_BIND_POINT_GRAPHICS,
    // HWRT
} WREpipelineType;

typedef enum
{
    WRE_FRONT_CULLING = VK_CULL_MODE_FRONT_BIT,
    WRE_BACK_CULLING = VK_CULL_MODE_BACK_BIT,
    WRE_NONE_CULLING = VK_CULL_MODE_NONE,
    WRE_FRONT_AND_BACK_CULLING = VK_CULL_MODE_FRONT_AND_BACK,
} WREpipelineCullMode;

typedef enum
{
    WRE_WINDING_CW = VK_FRONT_FACE_CLOCKWISE,
    WRE_WINDING_CCW = VK_FRONT_FACE_COUNTER_CLOCKWISE,
} WREpipelineWindingOrder;

typedef struct
{
    WREpipelineType type;
    WREShader shaders[2];
    WREvertexFormat format;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    char *Name;
} WREVKPipeline;

WREVKPipeline createPipeline(char *Name, WREvertexFormat vertFormat, WREShader *shaders, int shaderCount, WREpipelineCullMode cullMode, WREpipelineWindingOrder windingOrder, VkFormat colorAttFormats[8], uint32_t colorAttachmentCount);

#endif