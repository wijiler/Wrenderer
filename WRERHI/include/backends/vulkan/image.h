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
    VkAccessFlags access;
} WREVKImage;

void CreateImageView(WREVKImage *img, VkImageAspectFlagBits aspect);
void transitionImage(WREVKImage *img, VkImageLayout newLayout, VkAccessFlags access);
#endif