#ifndef WRESHADER_H__
#define WRESHADER_H__
#include <vulkan/vulkan.h>
typedef struct
{
    uint32_t len;
    char *data;

    VkShaderModule Shader;
    VkPipelineShaderStageCreateInfo stageInfo[2];
} WREShaderObjects;

typedef enum
{
    WRE_SHADER_STAGE_VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
    WRE_SHADER_STAGE_FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
    WRE_SHADER_STAGE_COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT,
} WREshaderStage;

typedef char *WREVKpushConstants;

typedef struct
{
    uint32_t pushConstantsSize;
    WREshaderStage shaderStage;
    WREShaderObjects shaderObjects;
    WREVKpushConstants pushconstants;
} WREshader;

typedef enum
{
    WRE_BINDING_INSTANCE = VK_VERTEX_INPUT_RATE_INSTANCE,
    WRE_BINDING_VERTEX = VK_VERTEX_INPUT_RATE_VERTEX,
} WREbindingType;

typedef struct
{
    uint32_t binding;
    uint32_t elementSize;
    WREbindingType inputRate;
} WREshaderBinding;

typedef struct
{
    uint32_t binding;
    uint32_t slot;
    VkFormat format;
    uint32_t offset;
} WREshaderAttribute;

typedef struct
{
    uint32_t bindingCount;
    VkVertexInputBindingDescription *bindings;
    uint32_t attribCount;
    VkVertexInputAttributeDescription *attributes;
} WREvertexFormat;

WREshader createShader(char *Filename, WREshaderStage stage);
void addShaderBinding(WREvertexFormat *format, WREshaderBinding binding);
void addShaderAttrib(WREvertexFormat *format, WREshaderAttribute attrib);
// ! pushconstant size must be less than or equal to 128 bytes
void setPushConstants(WREshader *shader, void *pushConstants, size_t pcSize);

#endif