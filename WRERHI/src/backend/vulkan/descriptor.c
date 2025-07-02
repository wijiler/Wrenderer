#include <backends/vulkan/descriptors.h>
#include <backends/vulkan/globals.h>

#include <stdio.h>

uint32_t binding_count = 0;

WREVKDescriptorSet allocate_descriptor_set(VkDescriptorPool *pool, WREVkDescriptorDescription in)
{
    WREVKDescriptorSet descriptor_set = {0};
    descriptor_set.binding = binding_count;

    VkDescriptorSetLayoutBinding binding = {
        binding_count,
        in.type,
        in.descriptorCount,
        VK_SHADER_STAGE_ALL,
        NULL,
    };

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT ext_flags = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,
        NULL,
        1,
        &in.flags,
    };

    VkDescriptorSetLayoutCreateInfo layout_CI = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        &ext_flags,
        VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
        1,
        &binding,
    };
    VkResult result = vkCreateDescriptorSetLayout(WREdevice, &layout_CI, NULL, &descriptor_set.layout);
    if (result != VK_SUCCESS)
    {
        printf("WREREN: Error: Could not create descriptor set layout\n");
        exit(1);
    }
    uint32_t max_binding = in.descriptorCount - 1;
    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT desc_alloc_count = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
        NULL,
        1,
        &max_binding,
    };

    VkDescriptorSetAllocateInfo set_alloc_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        &desc_alloc_count,
        *pool,
        1,
        &descriptor_set.layout,
    };

    result = vkAllocateDescriptorSets(WREdevice, &set_alloc_info, &descriptor_set.set);
    if (result != VK_SUCCESS)
    {
        printf("WREREN: Error: Could not allocate descriptor set\n");
        exit(1);
    }
    binding_count += 1;
    return descriptor_set;
}

void write_descriptor_set(WREVKImage image, VkSampler sampler, uint32_t index)
{
    VkDescriptorImageInfo img_inf = {
        sampler,
        image.imgview,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet ds_write_info = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        NULL,
        WREimagedescriptorSet,
        0,
        index,
        1,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        &img_inf,
        NULL,
        NULL,
    };
    vkUpdateDescriptorSets(WREdevice, 1, &ds_write_info, 0, NULL);
}