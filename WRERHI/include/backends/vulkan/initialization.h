#ifndef WREVULKINIT_H__
#define WREVULKINIT_H__

#define GLFW_INCLUDE_VULKAN
#include <backends/vulkan/globals.h>
#include <backends/vulkan/image.h>
#include <deps/GLFW/glfw3.h>
#include <vulkan/vulkan.h>

typedef struct
{
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR surfacePresentMode;

    VkSwapchainKHR swapChain;
    uint32_t SCImgCount;
    uint32_t CSCImgIndex;
    WREVKImage *SCImgs;
    WREVKImage *CurrentSCImg;

    int w, h;
    int x, y;
} RendererWindowContext;

typedef struct
{
    VkCommandPool commandPool;
    VkSemaphore graphicsTimeline, imgAvailable[FramesInFlightCount], *renderFinished;
    VkCommandBuffer graphicsCommandBuffers[FramesInFlightCount];
} RendererCoreContext;

void initializeVulkan(RendererCoreContext *objects, RendererWindowContext *windowContext, GLFWwindow *window);
void immediateSubmitBegin();
void immediateSubmitEnd();
#endif