#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <renderer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define instEXTENSIONCOUNT 4
#ifdef DEBUG
#define instLAYERCOUNT 1
#else
#define instLAYERCOUNT 0
#endif

char *instanceExtensions[instEXTENSIONCOUNT] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};
#ifdef DEBUG
char *instanceLayerNames[instLAYERCOUNT] = {
    "VK_LAYER_KHRONOS_validation"};
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        printf("Validation Layer: %s\n", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

void create_instance(renderer_t *renderer)
{

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;

    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pApplicationName = "Jr2";
    appInfo.pEngineName = "Jr2";

    VkInstanceCreateInfo instance_ci;
    instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_ci.pNext = NULL;

    instance_ci.enabledExtensionCount = instEXTENSIONCOUNT;
    instance_ci.ppEnabledExtensionNames = instanceExtensions;
    instance_ci.ppEnabledLayerNames = NULL;
    instance_ci.enabledLayerCount = instLAYERCOUNT;
#ifdef DEBUG
    instance_ci.ppEnabledLayerNames = instanceLayerNames;
    instance_ci.enabledLayerCount = instLAYERCOUNT;

    VkDebugUtilsMessengerCreateInfoEXT debug_CInf = {0};
    debug_CInf.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_CInf.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_CInf.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_CInf.pfnUserCallback = debugCallback;
    debug_CInf.pUserData = NULL;
    instance_ci.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_CInf;
#endif

    instance_ci.pApplicationInfo = &appInfo;

    VkInstance instance;
    VkResult result = vkCreateInstance(&instance_ci, NULL, &instance);
    if (result != VK_SUCCESS)
    {
        printf("Could not create a Vulkan instance\n");
    }
    renderer->vkCore.instance = instance;
}

VkPhysicalDevice find_valid_device(int deviceCount, VkPhysicalDevice devices[], VkPhysicalDeviceFeatures2 *devFeatures, unsigned int *graphicsFamilyIndex, VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties devProps = {0};

    VkPhysicalDeviceVulkan13Features devFeat13 = {0};
    devFeat13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    devFeat13.pNext = NULL;

    VkPhysicalDeviceFeatures2 devFeat2 = {0};
    devFeat2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    devFeat2.pNext = &devFeat13;

    unsigned int gfami = 0;
    for (int i = 0; i < deviceCount; i++)
    {
        vkGetPhysicalDeviceProperties(devices[i], &devProps);
        vkGetPhysicalDeviceFeatures2(devices[i], &devFeat2);

        VkQueueFamilyProperties qfamProps[8];

        unsigned int queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count, NULL);
        queue_family_count = min(queue_family_count, 8);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count, qfamProps);

        for (int j = 0; j < 8; j++)
        {
            VkBool32 supports_present = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, surface, &supports_present);
            // we want the graphics queue on the present queue, its faster, AKA Exclusive Mode
            if (qfamProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && supports_present == VK_TRUE)
            {
                gfami = j;
                break;
            }
        }

        if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && devFeat13.dynamicRendering == VK_TRUE)
        {
            devFeatures = &devFeat2;
            graphicsFamilyIndex = &gfami;
            return devices[i];
        }
    }
    printf("could not find a suitable gpu\n");
    exit(1);
    return NULL;
}

#define DEVICEEXTENSIONSCOUNT 3
char *deviceExtensions[DEVICEEXTENSIONSCOUNT] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
};

void create_device(VulkanCore_t *core)
{
    unsigned int deviceCount = 0;

    vkEnumeratePhysicalDevices(core->instance, &deviceCount, NULL);
    VkPhysicalDevice devices[8];
    vkEnumeratePhysicalDevices(core->instance, &deviceCount, devices);

    if (deviceCount == 0)
    {
        printf("Could not find a physical device\n");
        exit(1);
    }

    {
        VkResult result = glfwCreateWindowSurface(core->instance, core->window, NULL, &core->surface);
        if (result != VK_SUCCESS)
        {
            printf("Could not create window surface %i\n", result);
            exit(1);
        }
    }

    VkPhysicalDeviceVulkan13Features devFeatures13 = {0};
    devFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    devFeatures13.pNext = NULL;

    VkPhysicalDeviceFeatures2 devFeatures2 = {0};
    devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    unsigned int gfi = 0;

    core->pDev = find_valid_device(deviceCount, devices, &devFeatures2, &gfi, core->surface);

    devFeatures2.pNext = &devFeatures13;

    devFeatures2.features.geometryShader = VK_FALSE;
    devFeatures2.features.tessellationShader = VK_FALSE;

    devFeatures13.dynamicRendering = VK_TRUE;

    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = gfi;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDevice dev;

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &devFeatures2;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.ppEnabledExtensionNames = deviceExtensions;
    createInfo.enabledExtensionCount = DEVICEEXTENSIONSCOUNT;

    if (vkCreateDevice(core->pDev, &createInfo, NULL, &dev) != VK_SUCCESS)
    {
        printf("could not create the logical device\n");
        exit(1);
    }
    core->lDev = dev;
    // Exclusive mode
    vkGetDeviceQueue(core->lDev, gfi, 0, &core->gQueue);
    vkGetDeviceQueue(core->lDev, gfi, 0, &core->pQueue);
    // We already know that we can present on our window surface because we check for that while finding a valid queue
}

void create_swapchain(VulkanCore_t *core)
{
    VkSurfaceCapabilitiesKHR capabilities;

    VkSurfaceFormatKHR aFormats[4];
    VkPresentModeKHR aPresentModes[4];

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(core->pDev, core->surface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(core->pDev, core->surface, &formatCount, NULL);
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(core->pDev, core->surface, &formatCount, aFormats);

    uint32_t pModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(core->pDev, core->surface, &pModeCount, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(core->pDev, core->surface, &pModeCount, aPresentModes);

    core->sFormat = aFormats[0];
    for (unsigned int i = 0; i < formatCount; i++)
    {
        if (aFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && aFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // these are just the best ones :kekew:
        {
            core->sFormat = aFormats[i];
            break;
        }
    }

    core->sPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (unsigned int i = 0; i < pModeCount; i++)
    {
        if (aPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            core->sPresentMode = aPresentModes[i];
            break;
        }
    }

    int w, h = 0;
    glfwGetWindowSize(core->window, &w, &h);

    VkExtent2D extent;
    extent.height = h;
    extent.width = w;

    VkSwapchainCreateInfoKHR swapchainCI;
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.pNext = NULL;

    swapchainCI.imageExtent = extent;
    swapchainCI.imageFormat = core->sFormat.format;
    swapchainCI.imageColorSpace = core->sFormat.colorSpace;
    swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCI.imageArrayLayers = 1; // TODO: If I want to implement vr, change this
    swapchainCI.minImageCount = capabilities.minImageCount + 1 < capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchainCI.preTransform = capabilities.currentTransform;

    swapchainCI.presentMode = core->sPresentMode;

    swapchainCI.clipped = VK_TRUE;

    swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapchainCI.surface = core->surface;

    swapchainCI.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(core->lDev, &swapchainCI, NULL, &core->swapChain) != VK_SUCCESS)
    {
        printf("Swapchain could not be created");
        exit(1);
    }
}

void destroyRenderer(renderer_t *renderer)
{
    vkDestroySwapchainKHR(renderer->vkCore.lDev, renderer->vkCore.swapChain, NULL);
    vkDestroyDevice(renderer->vkCore.lDev, NULL);
    vkDestroySurfaceKHR(renderer->vkCore.instance, renderer->vkCore.surface, NULL);
    vkDestroyInstance(renderer->vkCore.instance, NULL);
}

void initRenderer(renderer_t *renderer)
{
    create_instance(renderer);
    create_device(&renderer->vkCore);
    create_swapchain(&renderer->vkCore);
}
