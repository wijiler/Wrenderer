#include <backends/vulkan/globals.h>
#include <backends/vulkan/image.h>
#include <stdio.h>

void CreateImageView(Image *img, VkImageAspectFlagBits aspect)
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

    if (vkCreateImageView(WREDevice, &imgViewCI, NULL, &img->imgview) != VK_SUCCESS)
    {
        printf("Could not create image view for image %p\n", img->img);
        exit(1);
    }
}