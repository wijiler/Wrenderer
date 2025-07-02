#ifndef WRESHADER_H__
#define WRESHADER_H__
#include <backends/vulkan/descriptors.h>
#include <vulkan/vulkan.h>

typedef struct
{
    char *data;
    VkShaderModule Shader;
    uint32_t len;
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
    WREShaderObjects shaderObjects;
    WREVKpushConstants pushconstants;
    VkDescriptorSetLayout *sets;
    uint32_t pushConstantsSize;
    uint32_t descriptorSetCount;
    WREshaderStage shaderStage;
} WREshader;

typedef enum
{
    WRE_BINDING_INSTANCE = VK_VERTEX_INPUT_RATE_INSTANCE,
    WRE_BINDING_VERTEX = VK_VERTEX_INPUT_RATE_VERTEX,
} WREbindingType;

typedef struct
{
    WREbindingType inputRate;
    uint32_t binding;
    uint32_t elementSize;
} WREshaderBinding;

typedef struct
{
    VkFormat format;
    uint32_t binding;
    uint32_t slot;
    uint32_t offset;
} WREshaderAttribute;

typedef struct
{
    VkVertexInputBindingDescription *bindings;
    VkVertexInputAttributeDescription *attributes;
    uint32_t bindingCount;
    uint32_t attribCount;
} WREvertexFormat;

WREshader createShader(char *Filename, WREshaderStage stage);
void addShaderBinding(WREvertexFormat *format, WREshaderBinding binding);
void addShaderAttrib(WREvertexFormat *format, WREshaderAttribute attrib);
// ! pushconstant size must be less than or equal to 128 bytes
void setPushConstants(WREshader *shader, void *pushConstants, size_t pcSize);
void addShaderDescriptor(WREshader *shader, WREVKDescriptorSet set);
#endif