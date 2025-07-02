#ifndef WREVULKIMG_H__
#define WREVULKIMG_H__
#include <vulkan/vulkan.h>
typedef struct
{
    VkImage img;
    VkImageView imgview;
    VkDeviceMemory memory;
    VkFormat format;
    VkImageLayout Layout;
    VkExtent2D extent;
    VkAccessFlags access;
} WREVKImage;

WREVKImage createImage(VkFormat format, VkImageLayout layout, VkExtent2D extent, VkImageUsageFlags usage);
void createImageView(WREVKImage *img, VkImageAspectFlagBits aspect);
void transitionImage(WREVKImage *img, VkImageLayout newLayout, VkAccessFlags access);
void transitionImageInCmdBuf(VkCommandBuffer cBuf, WREVKImage *img, VkImageLayout newLayout, VkAccessFlags access);
#endif