#include <renderer.h>
#include <stdio.h>

Image createImage(VulkanCore_t core, VkImageUsageFlags usage, VkFormat format, VkImageType type, VkImageTiling tiling, uint32_t width, uint32_t height, MemoryAccess access, VkImageAspectFlags aspects)
{
    Image img = {0};

    VkImageCreateInfo iCInf = {0};
    iCInf.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    iCInf.pNext = NULL;

    iCInf.arrayLayers = 1;
    iCInf.mipLevels = 1;
    iCInf.extent.width = width;
    iCInf.extent.height = height;
    iCInf.extent.depth = 1;

    iCInf.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    iCInf.imageType = type;
    iCInf.format = format;
    iCInf.imageType = type;
    iCInf.usage = usage;
    iCInf.tiling = tiling;
    iCInf.samples = VK_SAMPLE_COUNT_1_BIT;
    iCInf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(core.lDev, &iCInf, NULL, &img.image))
    {
        printf("could not create image");
        return (Image){0};
    }

    VkMemoryRequirements memReq = {0};
    vkGetImageMemoryRequirements(core.lDev, img.image, &memReq);

    VkPhysicalDeviceMemoryProperties memProps = {0};

    vkGetPhysicalDeviceMemoryProperties(core.pDev, &memProps);
    VkMemoryAllocateInfo memAllocInf = {0};
    memAllocInf.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInf.pNext = NULL;
    memAllocInf.allocationSize = memReq.size + memReq.alignment;
    int index = -1;
    for (int i = 0; i <= 31; i++)
    {
        if ((memProps.memoryTypes[i].propertyFlags & access) != 0)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        printf("could not find memory index for image");
        exit(1);
    }
    memAllocInf.memoryTypeIndex = index;
    vkAllocateMemory(core.lDev, &memAllocInf, NULL, &img.memory);

    vkBindImageMemory(core.lDev, img.image, img.memory, 0);

    VkImageViewCreateInfo iViewCI = {0};
    iViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    iViewCI.pNext = NULL;

    iViewCI.image = img.image;
    iViewCI.format = format;
    iViewCI.subresourceRange = (VkImageSubresourceRange){
        aspects,
        0,
        1,
        0,
        1,
    };
    iViewCI.viewType = (VkImageViewType)type;

    if (vkCreateImageView(core.lDev, &iViewCI, NULL, &img.imgview) != VK_SUCCESS)
    {
        printf("coult not create image view\n");
    }
    img.CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    img.accessMask = 0;
    return img;
}

void immediateSubmitBegin(VulkanCore_t core)
{
    vkWaitForFences(core.lDev, 1, &core.immediateFence, VK_TRUE, UINT64_MAX);
    vkResetFences(core.lDev, 1, &core.immediateFence);
    vkResetCommandBuffer(core.immediateSubmit, 0);
    vkBeginCommandBuffer(core.immediateSubmit, &cBufBeginInf);
}
void immediateSubmitEnd(VulkanCore_t core)
{
    vkEndCommandBuffer(core.immediateSubmit);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;

    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = 0;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &core.immediateSubmit;

    vkQueueSubmit(core.gQueue, 1, &submitInfo, core.immediateFence);
}

void transitionLayout(VulkanCore_t core, Image *img, VkImageLayout newLayout, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) // texture images will always be colorful :D
{
    VkImageMemoryBarrier memBarrier = {0};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.pNext = NULL;

    memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    memBarrier.oldLayout = img->CurrentLayout;
    memBarrier.newLayout = newLayout;

    memBarrier.srcAccessMask = img->accessMask;
    memBarrier.dstAccessMask = dstAccess;

    memBarrier.subresourceRange = (VkImageSubresourceRange){
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1,
    };

    memBarrier.image = img->image;
    vkCmdPipelineBarrier(core.immediateSubmit, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &memBarrier);
    img->CurrentLayout = newLayout;
    img->accessMask = dstAccess;
}

Texture createTexture(VulkanCore_t core, uint32_t width, uint32_t height)
{
    Texture tex = {0};
    tex.img = createImage(core, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, width, height, DEVICE_ONLY, VK_IMAGE_ASPECT_COLOR_BIT);
    return tex;
}

void copyDataToTextureImage(VulkanCore_t core, Image *image, Buffer *buffer, uint32_t width, uint32_t height)
{
    immediateSubmitBegin(core);
    transitionLayout(core, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    VkBufferImageCopy bICopy = {0};
    bICopy.bufferOffset = 0;
    bICopy.bufferRowLength = 0;
    bICopy.bufferImageHeight = 0;

    bICopy.imageSubresource = (VkImageSubresourceLayers){
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        0,
        1,
    };

    bICopy.imageExtent = (VkExtent3D){
        width,
        height,
        1,
    };
    bICopy.imageOffset = (VkOffset3D){0, 0, 0};

    vkCmdCopyBufferToImage(core.immediateSubmit, buffer->buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bICopy);
    transitionLayout(core, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    immediateSubmitEnd(core);
}

double clamp(double d, double min, double max)
{
    const double t = d < min ? min : d;
    return t > max ? max : t;
}
uint32_t packUnorm4x8(float t[4])
{
    uint8_t v1 = t[0];
    uint8_t v2 = t[1];
    uint8_t v3 = t[2];
    uint8_t v4 = t[3];

    uint32_t c = v1 | (v2 << 8) | (v3 << 16) | (v4 << 24);

    return clamp(c, 0, 1) * 255;
}

void submitTexture(renderer_t *renderer, Texture *tex, VkSampler sampler)
{
    write_textureDescriptorSet(renderer->vkCore, tex->img.imgview, sampler, renderer->vkCore.textureCount);
    tex->index = renderer->vkCore.textureCount;
    renderer->vkCore.textureCount += 1;
}