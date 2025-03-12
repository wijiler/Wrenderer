#ifndef WREVULKIMG_H__
#define WREVULKIMG_H__
#include <vulkan/vulkan.h>
typedef struct
{
    VkImage img;
    VkImageView imgview;
    VkFormat format;
    VkImageLayout Layout;
    VkExtent2D extent;
} Image;

void CreateImageView(Image *img, VkImageAspectFlagBits aspect);
#endif