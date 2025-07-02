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
    WREshader shaders[2];
    WREpipelineType type;
    WREvertexFormat format;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    char *Name;
} WREVKPipeline;

typedef struct
{
    VkFormat colorAttFormats[8];
    WREvertexFormat vertFormat;
    WREshader *shaders;
    WREpipelineCullMode cullMode;
    WREpipelineWindingOrder windingOrder;
    uint32_t colorAttachmentsCount;
    // ! Up to 2 shaders
    uint8_t shaderCount;
} WREVkPipelineCI;
WREVKPipeline createPipeline(char *Name, const WREVkPipelineCI *create_info);

#endif