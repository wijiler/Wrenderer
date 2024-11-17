#include <renderer.h>
#include <stdio.h>

void initializeDescriptor(VulkanCore_t core, WREDescriptor *desc, uint32_t descriptorCount, uint32_t setCount, VkDescriptorType type, shaderStage stage, bool bindless)
{
    desc->type = type;
    desc->setcount = setCount;
    desc->descriptorCount = descriptorCount;
    desc->bindless = bindless;
    VkDescriptorPoolSize poolSize = {0};

    poolSize.type = type;
    poolSize.descriptorCount = descriptorCount;

    VkDescriptorPoolCreateInfo dspCI = {0};
    dspCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dspCI.pNext = 0;

    dspCI.maxSets = setCount;
    dspCI.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    dspCI.poolSizeCount = 1;
    dspCI.pPoolSizes = &poolSize;

    vkCreateDescriptorPool(core.lDev, &dspCI, NULL, &desc->pool);

    VkDescriptorSetLayoutBinding UBindingInf = {0};
    UBindingInf.binding = 0;
    UBindingInf.descriptorType = type;
    UBindingInf.descriptorCount = descriptorCount;

    UBindingInf.stageFlags = stage;

    VkDescriptorBindingFlagsEXT slciFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT slciFlagsEXT = {0};
    slciFlagsEXT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    slciFlagsEXT.pNext = NULL;
    slciFlagsEXT.pBindingFlags = &slciFlags;
    slciFlagsEXT.bindingCount = setCount;

    VkDescriptorSetLayoutCreateInfo slci = {0};
    slci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    slci.pNext = bindless ? &slciFlagsEXT : NULL;

    slci.bindingCount = 1;
    slci.pBindings = &UBindingInf;
    slci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

    if (vkCreateDescriptorSetLayout(core.lDev, &slci, NULL, &desc->layout) != VK_SUCCESS)
    {
        printf("Could not create descriptor set layout 1");
        exit(1);
    }
}

void allocateDescriptorSets(VulkanCore_t core, WREDescriptor *desc)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countAllocInfoEXT = {0};
    countAllocInfoEXT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    countAllocInfoEXT.pNext = NULL;

    countAllocInfoEXT.descriptorSetCount = desc->setcount;
    uint32_t mDescCount = desc->descriptorCount;
    countAllocInfoEXT.pDescriptorCounts = &mDescCount;

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = desc->bindless ? &countAllocInfoEXT : NULL;

    allocInfo.descriptorPool = desc->pool;
    allocInfo.descriptorSetCount = desc->setcount;
    allocInfo.pSetLayouts = &desc->layout;

    desc->sets = malloc(sizeof(WREDescriptorSet) * desc->setcount);
    for (uint32_t i = 0; i < desc->setcount; i++)
    {
        if (vkAllocateDescriptorSets(core.lDev, &allocInfo, &desc->sets[i].set) != VK_SUCCESS)
            printf("Couldnt allocate descriptor sets");
    }
}

void writeDescriptorSet(VulkanCore_t core, WREDescriptor desc, uint32_t setIndex, uint32_t arrayIndex, VkImageView image, VkSampler sampler)
{
    VkWriteDescriptorSet dsWrite = {0};
    dsWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dsWrite.pNext = NULL;

    dsWrite.descriptorCount = 1;
    dsWrite.dstArrayElement = arrayIndex;
    dsWrite.descriptorType = desc.type;
    dsWrite.dstSet = desc.sets[setIndex].set;
    dsWrite.dstBinding = 0;

    VkDescriptorImageInfo descImgInfo = {0};
    descImgInfo.imageView = image;
    descImgInfo.sampler = sampler;
    descImgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    dsWrite.pImageInfo = &descImgInfo;

    vkUpdateDescriptorSets(core.lDev, 1, &dsWrite, 0, NULL);
}