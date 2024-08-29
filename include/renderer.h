#ifndef REND
#define REND
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <rendergraph.h>
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        VkInstance instance;
        VkPhysicalDevice pDev;
        VkSurfaceKHR surface;
        VkDevice lDev;
        VkQueue pQueue, gQueue;
        VkSwapchainKHR swapChain;
        VkSurfaceFormatKHR sFormat;
        VkPresentModeKHR sPresentMode;
        VkImage swapChainImages[3];
        GLFWwindow *window;
    } VulkanCore_t;

    typedef struct
    {
        // rendergraph_t *rendergraph;
        VulkanCore_t vkCore;
    } renderer_t;

    void initRenderer(renderer_t *renderer);
    void destroyRenderer(renderer_t *renderer);
#ifdef __cplusplus
}
#endif
#endif