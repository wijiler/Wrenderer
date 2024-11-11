#ifndef WRE_DESCRIPTORS_H__
#define WRE_DESCRIPTORS_H__

#include <renderer.h>
#include <stdbool.h>

typedef struct
{
    VkDescriptorSet set;
    uint32_t binding;
} WREDescriptorSet;

typedef struct
{
    VkDescriptorType type;
    VkDescriptorPool pool;
    VkDescriptorSetLayout layout;
    WREDescriptorSet *sets;
    uint32_t setcount, descriptorCount;
    bool bindless;
} WREDescriptor;

void initializeDescriptor(renderer_t renderer, WREDescriptor *desc, uint32_t descriptorCount, uint32_t setCount, VkDescriptorType type, shaderStage stage, bool bindless);
void allocateDescriptorSets(renderer_t renderer, WREDescriptor *desc);
void writeDescriptorSet(renderer_t renderer, WREDescriptor desc, uint32_t setIndex, uint32_t arrayIndex, VkImageView image, VkSampler sampler);

#endif