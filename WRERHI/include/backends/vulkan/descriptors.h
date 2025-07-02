#ifndef WRE_DESCRIPTORS_H__
#define WRE_DESCRIPTORS_H__
#include <backends/vulkan/image.h>
#include <vulkan/vulkan.h>

typedef struct
{
    VkDescriptorSetLayout layout;
    VkDescriptorSet set;
    uint32_t binding;
} WREVKDescriptorSet;

typedef uint32_t WREVKDescriptorBinding;

typedef struct
{
    VkDescriptorBindingFlags flags;
    VkDescriptorType type;
    uint32_t descriptorCount;
} WREVkDescriptorDescription;

WREVKDescriptorSet allocate_descriptor_set(VkDescriptorPool *pool, WREVkDescriptorDescription in);
void write_descriptor_set(WREVKImage image, VkSampler sampler, uint32_t index);

#endif