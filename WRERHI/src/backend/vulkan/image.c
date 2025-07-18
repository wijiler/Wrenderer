#include <backends/vulkan/globals.h>
#include <backends/vulkan/image.h>
#include <stdio.h>

WREVKImage createImage(VkFormat format, VkImageLayout layout, VkExtent2D extent, VkImageUsageFlags usage)
{
    WREVKImage image = {
        NULL,
        NULL,
        NULL,
        format,
        layout,
        extent,
        0,
    };
    VkImageCreateInfo img_ci = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        NULL,
        0,
        VK_IMAGE_TYPE_2D,
        format,
        {extent.width, extent.height, 1},
        1,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_LINEAR,
        usage,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL,
        layout,
    };
    VkResult result = vkCreateImage(WREdevice, &img_ci, NULL, &image.img);
    if (result != VK_SUCCESS)
    {
        printf("WRERen: Error: Could not create image\n");
        exit(1);
    }
    VkMemoryRequirements memReq = {0};
    vkGetImageMemoryRequirements(WREdevice, image.img, &memReq);

    VkPhysicalDeviceMemoryProperties memProps = {0};

    vkGetPhysicalDeviceMemoryProperties(WREpDevice, &memProps);
    VkMemoryAllocateInfo memAllocInf = {0};
    memAllocInf.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInf.pNext = NULL;
    memAllocInf.allocationSize = memReq.size + memReq.alignment;
    memAllocInf.memoryTypeIndex = deviceLocalHeapIndex;

    result = vkAllocateMemory(WREdevice, &memAllocInf, NULL, &image.memory);
    if (result != VK_SUCCESS)
    {
        printf("WRERen: Error: Could not allocate the proper memory for image\n");
        exit(1);
    }
    result = vkBindImageMemory(WREdevice, image.img, image.memory, 0);

    if (result != VK_SUCCESS)
    {
        printf("WRERen: Error: Could not bind memory to image\n");
        exit(1);
    }

    return image;
}

void createImageView(WREVKImage *img, VkImageAspectFlagBits aspect)
{
    VkImageViewCreateInfo imgViewCI = {0};
    imgViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewCI.image = img->img;
    imgViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewCI.format = img->format;

    imgViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imgViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imgViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imgViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imgViewCI.subresourceRange.aspectMask = aspect;
    imgViewCI.subresourceRange.baseMipLevel = 0;
    imgViewCI.subresourceRange.levelCount = 1;
    imgViewCI.subresourceRange.baseArrayLayer = 0;
    imgViewCI.subresourceRange.layerCount = 1;

    if (vkCreateImageView(WREdevice, &imgViewCI, NULL, &img->imgview) != VK_SUCCESS)
    {
        printf("Could not create image view for image %p\n", img->img);
        exit(1);
    }
}

void transitionImage(WREVKImage *img, VkImageLayout newLayout, VkAccessFlags access)
{
    VkCommandBufferBeginInfo inf = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        0,
        NULL,
    };
    VkImageMemoryBarrier2 imgMemBarr = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        NULL,
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        img->access,
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        access,
        img->Layout,
        newLayout,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        img->img,
        (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
    };

    VkDependencyInfo depInf = {
        VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        NULL,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &imgMemBarr,
    };
    vkResetCommandBuffer(WREinstantCommandBuffer, 0);
    vkBeginCommandBuffer(WREinstantCommandBuffer, &inf);
    vkCmdPipelineBarrier2(WREinstantCommandBuffer, &depInf);
    vkEndCommandBuffer(WREinstantCommandBuffer);
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;

    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = 0;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &WREinstantCommandBuffer;

    vkQueueSubmit(WREgraphicsQueue, 1, &submitInfo, NULL);
    img->access = access;
    img->Layout = newLayout;
}

void transitionImageInCmdBuf(VkCommandBuffer cBuf, WREVKImage *img, VkImageLayout newLayout, VkAccessFlags access)
{
    VkImageMemoryBarrier2 imgMemBarr = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        NULL,
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        img->access,
        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        access,
        img->Layout,
        newLayout,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        img->img,
        (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
    };

    VkDependencyInfo depInf = {
        VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        NULL,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &imgMemBarr,
    };
    vkCmdPipelineBarrier2(cBuf, &depInf);
    img->access = access;
    img->Layout = newLayout;
}
