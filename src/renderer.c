#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <math.h>
#include <renderer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT_ = NULL;
PFN_vkCreateShadersEXT vkCreateShadersEXT_ = NULL;
PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT_ = NULL;
PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT_ = NULL;
PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT_ = NULL;
PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT_ = NULL;
PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT_ = NULL;
PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT_ = NULL;
PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT_ = NULL;
PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT_ = NULL;
PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT_ = NULL;
PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT_ = NULL;
PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT_ = NULL;
PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT_ = NULL;
PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT_ = NULL;
PFN_vkDestroyShaderEXT vkDestroyShaderEXT_ = NULL;

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
#ifdef VK_USE_PLATFORM_XLIB_KHR
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};
#ifdef DEBUG
char *instanceLayerNames[instLAYERCOUNT] = {"VK_LAYER_KHRONOS_validation"};
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
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
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 294);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pApplicationName = "Jrv2";
    appInfo.pEngineName = "Jrv2";

    VkInstanceCreateInfo instance_ci;
    instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_ci.pNext = NULL;
    instance_ci.flags = 0x0;

    instance_ci.enabledExtensionCount = instEXTENSIONCOUNT;
    instance_ci.ppEnabledExtensionNames = instanceExtensions;
    instance_ci.ppEnabledLayerNames = NULL;
    instance_ci.enabledLayerCount = instLAYERCOUNT;
#ifdef DEBUG
    instance_ci.ppEnabledLayerNames = instanceLayerNames;
    instance_ci.enabledLayerCount = instLAYERCOUNT;

    VkDebugUtilsMessengerCreateInfoEXT debug_CInf = {0};
    debug_CInf.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_CInf.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_CInf.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
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

    vkCmdSetVertexInputEXT_ = (PFN_vkCmdSetVertexInputEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetVertexInputEXT");
    vkCreateShadersEXT_ = (PFN_vkCreateShadersEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCreateShadersEXT");
    vkCmdBindShadersEXT_ = (PFN_vkCmdBindShadersEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdBindShadersEXT");
    vkCmdSetColorBlendEnableEXT_ = (PFN_vkCmdSetColorBlendEnableEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetColorBlendEnableEXT");
    vkCmdSetColorWriteMaskEXT_ = (PFN_vkCmdSetColorWriteMaskEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetColorWriteMaskEXT");
    vkCmdSetDepthClampEnableEXT_ = (PFN_vkCmdSetDepthClampEnableEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetDepthClampEnableEXT");
    vkCmdSetPolygonModeEXT_ = (PFN_vkCmdSetPolygonModeEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetPolygonModeEXT");
    vkCmdSetLogicOpEnableEXT_ = (PFN_vkCmdSetLogicOpEnableEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetLogicOpEnableEXT");
    vkCmdSetRasterizationSamplesEXT_ = (PFN_vkCmdSetRasterizationSamplesEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetRasterizationSamplesEXT");
    vkCmdSetColorBlendEquationEXT_ = (PFN_vkCmdSetColorBlendEquationEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetColorBlendEquationEXT");
    vkCmdSetSampleMaskEXT_ = (PFN_vkCmdSetSampleMaskEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetSampleMaskEXT");
    vkCmdSetAlphaToCoverageEnableEXT_ = (PFN_vkCmdSetAlphaToCoverageEnableEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetAlphaToCoverageEnableEXT");
    vkCmdSetAlphaToOneEnableEXT_ = (PFN_vkCmdSetAlphaToOneEnableEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetAlphaToOneEnableEXT");
    vkCmdSetDepthClipEnableEXT_ = (PFN_vkCmdSetDepthClipEnableEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetDepthClipEnableEXT");
    vkCmdSetLogicOpEXT_ = (PFN_vkCmdSetLogicOpEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkCmdSetLogicOpEXT");
    vkDestroyShaderEXT_ = (PFN_vkDestroyShaderEXT)vkGetInstanceProcAddr(renderer->vkCore.instance, "vkDestroyShaderEXT");
}

VkPhysicalDevice find_valid_device(int deviceCount, VkPhysicalDevice devices[], unsigned int *graphicsFamilyIndex, unsigned int *computeFamilyIndex, VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties devProps = {0};

    VkPhysicalDeviceVulkan11Features devFeat11 = {0};
    devFeat11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    devFeat11.pNext = NULL;

    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertAttrDivFeats = {0};
    vertAttrDivFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT;
    vertAttrDivFeats.pNext = &devFeat11;

    VkPhysicalDeviceDepthClipEnableFeaturesEXT depthClipEnable = {0};
    depthClipEnable.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT;
    depthClipEnable.pNext = &vertAttrDivFeats;

    VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures = {0};
    shaderObjectFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
    shaderObjectFeatures.pNext = &depthClipEnable;

    VkPhysicalDeviceVulkan12Features devFeat12 = {0};
    devFeat12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    devFeat12.pNext = &shaderObjectFeatures;

    VkPhysicalDeviceVulkan13Features devFeat13 = {0};
    devFeat13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    devFeat13.pNext = &devFeat12;

    VkPhysicalDeviceFeatures2 devFeat2 = {0};
    devFeat2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    devFeat2.pNext = &devFeat13;

    unsigned int gfami = 0;
    unsigned int cfami = 0;
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
            // we want the graphics queue on the present queue, its faster, AKA
            // Exclusive Mode
            if (qfamProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && supports_present == VK_TRUE)
            {
                gfami = j;
                break;
            }
            if (qfamProps[j].queueFlags & VK_QUEUE_COMPUTE_BIT && qfamProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                cfami = j;
                break;
            }
        }

        if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && devFeat13.dynamicRendering == VK_TRUE &&
            devFeat12.bufferDeviceAddress == VK_TRUE && devFeat12.descriptorBindingUniformBufferUpdateAfterBind == VK_TRUE &&
            devFeat12.descriptorBindingPartiallyBound == VK_TRUE && devFeat12.descriptorBindingVariableDescriptorCount == VK_TRUE &&
            shaderObjectFeatures.shaderObject == VK_TRUE && devFeat2.features.alphaToOne == VK_TRUE && depthClipEnable.depthClipEnable == VK_TRUE &&
            vertAttrDivFeats.vertexAttributeInstanceRateZeroDivisor == VK_TRUE && devFeat12.descriptorBindingPartiallyBound == VK_TRUE &&
            devFeat12.runtimeDescriptorArray == VK_TRUE && devFeat12.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE && devFeat2.features.shaderInt64 &&
            devFeat12.scalarBlockLayout == VK_TRUE && devFeat11.variablePointers == VK_TRUE && devFeat11.variablePointersStorageBuffer == VK_TRUE &&
            devFeat2.features.samplerAnisotropy == VK_TRUE && devFeat2.features.shaderInt16)
        {
            graphicsFamilyIndex = &gfami;
            computeFamilyIndex = &cfami;
            return devices[i];
        }
    }
    printf("could not find a suitable gpu\n");
    exit(1);
    return NULL;
}

#define DEVICEEXTENSIONSCOUNT 6
const char *deviceExtensions[DEVICEEXTENSIONSCOUNT] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
    VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME,
    VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,
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
    VkPhysicalDeviceVulkan11Features devFeat11 = {0};
    devFeat11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    devFeat11.pNext = NULL;

    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertAttrDivFeats = {0};
    vertAttrDivFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT;
    vertAttrDivFeats.pNext = &devFeat11;

    VkPhysicalDeviceDepthClipEnableFeaturesEXT depthClipEnable = {0};
    depthClipEnable.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT;
    depthClipEnable.pNext = &vertAttrDivFeats;

    VkPhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures = {0};
    shaderObjectFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
    shaderObjectFeatures.pNext = &depthClipEnable;

    VkPhysicalDeviceVulkan12Features devFeatures12 = {0};
    devFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    devFeatures12.pNext = &shaderObjectFeatures;

    VkPhysicalDeviceVulkan13Features devFeatures13 = {0};
    devFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    devFeatures13.pNext = &devFeatures12;

    VkPhysicalDeviceFeatures2 devFeatures2 = {0};
    devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    unsigned int gfi = 0;
    unsigned int cfi = 0;

    core->pDev = find_valid_device(deviceCount, devices, &gfi, &cfi, core->surface);
    core->qfi = gfi;
    core->compQfi = cfi;
    devFeatures2.pNext = &devFeatures13;

    devFeatures2.features.alphaToOne = VK_TRUE;
    devFeatures2.features.shaderInt64 = VK_TRUE;
    devFeatures2.features.shaderInt16 = VK_TRUE;

    devFeatures13.dynamicRendering = VK_TRUE;

    devFeatures12.bufferDeviceAddress = VK_TRUE;

    devFeatures12.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    devFeatures12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    devFeatures12.descriptorBindingPartiallyBound = VK_TRUE;
    devFeatures12.descriptorBindingVariableDescriptorCount = VK_TRUE;
    devFeatures12.runtimeDescriptorArray = VK_TRUE;
    devFeatures12.scalarBlockLayout = VK_TRUE;

    shaderObjectFeatures.shaderObject = VK_TRUE;

    depthClipEnable.depthClipEnable = VK_TRUE;

    vertAttrDivFeats.vertexAttributeInstanceRateZeroDivisor = VK_TRUE;

    devFeat11.variablePointers = VK_TRUE;
    devFeat11.variablePointersStorageBuffer = VK_TRUE;

    devFeatures2.features.samplerAnisotropy = VK_TRUE;

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
    vkGetDeviceQueue(core->lDev, gfi, 0, &core->pQueue); // why do we store the same queue in two different things? Orginization + ordering + Synchronization
    // We already know that we can present on our window surface because we check for that while finding a valid queue
    vkGetDeviceQueue(core->lDev, cfi, 0, &core->compQueue);
}

VkImageView get_image_view(VkImage image, VulkanCore_t core)
{
    VkImageViewCreateInfo imgViewCI;
    imgViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewCI.pNext = NULL;
    imgViewCI.flags = 0x0;

    imgViewCI.image = image;

    imgViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewCI.format = core.sFormat.format;

    imgViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imgViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imgViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imgViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imgViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imgViewCI.subresourceRange.baseMipLevel = 0;
    imgViewCI.subresourceRange.levelCount = 1;
    imgViewCI.subresourceRange.baseArrayLayer = 0;
    imgViewCI.subresourceRange.layerCount = 1;

    VkImageView view;
    if (vkCreateImageView(core.lDev, &imgViewCI, NULL, &view) != VK_SUCCESS)
    {
        printf("Could not create image view for image %p\n", image);
        exit(1);
    }
    return view;
}

void create_swapchain(VulkanCore_t *core)
{
    VkSurfaceCapabilitiesKHR capabilities;

    VkSurfaceFormatKHR aFormats[4];
    VkPresentModeKHR aPresentModes[4];

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(core->pDev, core->surface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(core->pDev, core->surface, &formatCount, NULL);
    vkGetPhysicalDeviceSurfaceFormatsKHR(core->pDev, core->surface, &formatCount, aFormats);

    uint32_t pModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(core->pDev, core->surface, &pModeCount, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(core->pDev, core->surface, &pModeCount, aPresentModes);

    core->sFormat = aFormats[0];
    for (unsigned int i = 0; i < formatCount; i++)
    {
        if (aFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && aFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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

    core->extent = extent;

    VkSwapchainCreateInfoKHR swapchainCI;
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.pNext = NULL;
    swapchainCI.flags = 0;

    swapchainCI.imageExtent = extent;
    swapchainCI.imageFormat = core->sFormat.format;
    swapchainCI.imageColorSpace = core->sFormat.colorSpace;
    swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCI.imageArrayLayers = 1; // TODO: If I want to implement vr, change this
    swapchainCI.minImageCount = capabilities.minImageCount + 1 < capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

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

    uint32_t imagecount = 0;
    VkImage *images;
    vkGetSwapchainImagesKHR(core->lDev, core->swapChain, &imagecount, NULL);
    images = malloc(sizeof(VkImage) * imagecount);
    vkGetSwapchainImagesKHR(core->lDev, core->swapChain, &imagecount, images);

    core->swapChainImages = images;
    core->swapChainImageViews = malloc(sizeof(VkImageView) * imagecount);
    for (uint32_t i = 0; i < imagecount; i++)
    {
        core->swapChainImageViews[i] = get_image_view(images[i], *core);
    }
    core->imgCount = imagecount;
}

void recreateSwapchain(renderer_t *renderer)
{
    vkDeviceWaitIdle(renderer->vkCore.lDev);

    vkDestroySwapchainKHR(renderer->vkCore.lDev, renderer->vkCore.swapChain, NULL);
    for (uint32_t i = 0; i < renderer->vkCore.imgCount; i++)
    {
        vkDestroyImageView(renderer->vkCore.lDev, renderer->vkCore.swapChainImageViews[i], NULL);
    }

    free(renderer->vkCore.swapChainImages);
    free(renderer->vkCore.swapChainImageViews);
    renderer->vkCore.swapChainImages = NULL;
    renderer->vkCore.swapChainImageViews = NULL;

    create_swapchain(&renderer->vkCore);
}

void create_CommandBuffers(VulkanCore_t *core)
{
    VkCommandPoolCreateInfo comPoolCI = {0};
    comPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    comPoolCI.pNext = NULL;

    comPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    comPoolCI.queueFamilyIndex = core->qfi;
    if (vkCreateCommandPool(core->lDev, &comPoolCI, NULL, (VkCommandPool *)&core->commandPool) != VK_SUCCESS)
    {
        printf("Could not issue commandPool\n");
        exit(1);
    }

    VkCommandBufferAllocateInfo cbAI = {0};
    cbAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAI.pNext = NULL;

    cbAI.commandPool = (VkCommandPool)core->commandPool;
    cbAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAI.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(core->lDev, &cbAI, &core->immediateSubmit) != VK_SUCCESS)
    {
        printf("Could not create command buffers\n");
        exit(1);
    }
    if (vkAllocateCommandBuffers(core->lDev, &cbAI, &core->computeCommandBuffer) != VK_SUCCESS)
    {
        printf("Could not create command buffers\n");
        exit(1);
    }

    for (int i = 0; i < FRAMECOUNT; i++)
    {
        if (vkAllocateCommandBuffers(core->lDev, &cbAI, &core->commandBuffers[i]) != VK_SUCCESS) // could we batch this?
        {
            printf("Could not create command buffers\n");
            exit(1);
        }
        VkSemaphoreCreateInfo semaphoreCI = {0};
        semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCI.pNext = NULL;
        semaphoreCI.flags = 0;

        if (vkCreateSemaphore(core->lDev, &semaphoreCI, NULL, &core->imageAvailable[i]) != VK_SUCCESS)
        {
            printf("Could not create semaphore\n");
            exit(1);
        }

        VkFenceCreateInfo fenceCI = {0};
        fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCI.pNext = NULL;
        fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(core->lDev, &fenceCI, NULL, &core->fences[i]) != VK_SUCCESS)
        {
            printf("Could not create fence\n");
            exit(1);
        }
        if (vkCreateFence(core->lDev, &fenceCI, NULL, &core->computeFences[i]) != VK_SUCCESS)
        {
            printf("Could not create fence\n");
            exit(1);
        }
    }

    core->renderFinished = malloc(sizeof(VkSemaphore) * core->imgCount + 1);
    core->computeFinished = malloc(sizeof(VkSemaphore) * core->imgCount + 1);
    for (uint32_t i = 0; i < core->imgCount; i++)
    {
        VkSemaphoreCreateInfo semaphoreCI = {0};
        semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCI.pNext = NULL;
        semaphoreCI.flags = 0;
        if (vkCreateSemaphore(core->lDev, &semaphoreCI, NULL, &core->renderFinished[i]) != VK_SUCCESS)
        {
            printf("Could not create semaphore\n");
            exit(1);
        }
        if (vkCreateSemaphore(core->lDev, &semaphoreCI, NULL, &core->computeFinished[i]) != VK_SUCCESS)
        {
            printf("Could not create semaphore\n");
            exit(1);
        }
    }

    VkFenceCreateInfo immFenceCI = {0};
    immFenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    immFenceCI.pNext = NULL;
    immFenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(core->lDev, &immFenceCI, NULL, &core->immediateFence))
    {
        printf("Could not create fence\n");
        exit(1);
    }
}

typedef struct
{
    Buffer buf;
    bool active;
} bufferInfo_t;

bufferInfo_t bufferInfo[256];
uint32_t bufferCount = 0;
void MarkDestroyableBuffer(Buffer *buf)
{
    buf->index = bufferCount;
    bufferInfo_t info = {0};
    info.buf = *buf;
    info.active = true;

    for (uint32_t i = 0; i < bufferCount; i++)
    {
        if (bufferInfo[i].active == false)
        {
            buf->index = i;
            bufferInfo[i] = info;
            return;
        }
    }
    bufferInfo[bufferCount] = info;
    bufferCount += 1;
}

void createBuffer(VulkanCore_t core, BufferCreateInfo createInfo, Buffer *buf)
{
    buf->size = createInfo.dataSize;
    buf->type = createInfo.usage;

    VkBufferCreateInfo bufferCI = {0};
    bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCI.pNext = NULL;

    bufferCI.size = createInfo.dataSize;
    bufferCI.usage = createInfo.usage;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(core.lDev, &bufferCI, NULL, &buf->buffer) != VK_SUCCESS)
    {
        printf("Could not create buffer\n");
        exit(-1);
    }

    //
    //      Memory
    // -----------------

    VkPhysicalDeviceMemoryProperties memProps = {0};

    vkGetPhysicalDeviceMemoryProperties(core.pDev, &memProps);
    int index = -1;
    for (int i = 0; i <= 31; i++)
    {
        if ((memProps.memoryTypes[i].propertyFlags & createInfo.access) != 0)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        printf("Could not find suitable memory for buffer");
        exit(1);
    }

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    VkMemoryAllocateFlagsInfo flags = {0};
    flags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    flags.pNext = NULL;

    flags.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

    allocInfo.pNext = &flags;

    VkMemoryRequirements vkMemReq = {0};
    vkGetBufferMemoryRequirements(core.lDev, buf->buffer, &vkMemReq);

    allocInfo.allocationSize = vkMemReq.size + createInfo.dataSize;
    allocInfo.memoryTypeIndex = index;

    if (vkAllocateMemory(core.lDev, &allocInfo, NULL, &buf->associatedMemory) != VK_SUCCESS)
    {
        printf("Could not allocate memory\n");
        exit(1);
    }

    if (vkBindBufferMemory(core.lDev, buf->buffer, buf->associatedMemory, 0) != VK_SUCCESS)
    {
        printf("Could not bind memory\n");
        exit(1);
    }
    buf->mappedMemory = malloc(buf->size);
    if ((createInfo.access & CPU_ONLY) != 0)
    {
        vkMapMemory(core.lDev, buf->associatedMemory, 0, buf->size, 0, &buf->mappedMemory);
    }

    if ((createInfo.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR) != 0)
    {
        VkBufferDeviceAddressInfoKHR bufferAddrInf = {0};
        bufferAddrInf.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
        bufferAddrInf.pNext = NULL;
        bufferAddrInf.buffer = buf->buffer;
        buf->gpuAddress = vkGetBufferDeviceAddress(core.lDev, &bufferAddrInf);
    }

    MarkDestroyableBuffer(buf);
}

void destroyBuffer(Buffer buf, VulkanCore_t core)
{
    vkWaitForFences(core.lDev, 1, &core.immediateFence, VK_TRUE, UINT64_MAX);
    vkDestroyBuffer(core.lDev, buf.buffer, NULL);
    vkFreeMemory(core.lDev, buf.associatedMemory, NULL);

    bufferInfo[buf.index].active = false;
}

void pushDataToBuffer(void *data, size_t dataSize, Buffer buf, int offSet)
{
    memcpy((char*)buf.mappedMemory + offSet, data, dataSize);
}

void copyBuf(VulkanCore_t core, Buffer src, Buffer dest, size_t size, uint32_t srcOffset, uint32_t dstOffset)
{
    vkWaitForFences(core.lDev, 1, &core.immediateFence, VK_TRUE, UINT64_MAX);
    vkResetFences(core.lDev, 1, &core.immediateFence);
    vkResetCommandBuffer(core.immediateSubmit, 0);
    vkBeginCommandBuffer(core.immediateSubmit, &cBufBeginInf);
    VkBufferCopy copyData = {0};
    copyData.dstOffset = dstOffset;
    copyData.srcOffset = srcOffset;
    copyData.size = size;

    vkCmdCopyBuffer(core.immediateSubmit, src.buffer, dest.buffer, 1, &copyData);
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

void create_dsp(VulkanCore_t *core)
{
    VkDescriptorPoolSize poolSize = {0};

    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = MAXTEXTURES;

    VkDescriptorPoolCreateInfo dspCI = {0};
    dspCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dspCI.pNext = 0;

    dspCI.maxSets = MAXTEXTURES;
    dspCI.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    dspCI.poolSizeCount = 1;
    dspCI.pPoolSizes = &poolSize;

    vkCreateDescriptorPool(core->lDev, &dspCI, NULL, &core->tdescPool);

    VkDescriptorSetLayoutBinding UBindingInf = {0};
    UBindingInf.binding = 0;
    UBindingInf.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    UBindingInf.descriptorCount = MAXTEXTURES;

    UBindingInf.stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorBindingFlagsEXT slciFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT slciFlagsEXT = {0};
    slciFlagsEXT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    slciFlagsEXT.pNext = NULL;
    slciFlagsEXT.pBindingFlags = &slciFlags;
    slciFlagsEXT.bindingCount = 1;

    VkDescriptorSetLayoutCreateInfo slci = {0};
    slci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    slci.pNext = &slciFlagsEXT;

    slci.bindingCount = 1;
    slci.pBindings = &UBindingInf;
    slci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

    if (vkCreateDescriptorSetLayout(core->lDev, &slci, NULL, &core->tdSetLayout) != VK_SUCCESS)
    {
        printf("Could not create descriptor set layout 1");
        exit(1);
    }
}

void allocate_textureDescriptorSet(VulkanCore_t *core)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countAllocInfoEXT = {0};
    countAllocInfoEXT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    countAllocInfoEXT.pNext = NULL;

    countAllocInfoEXT.descriptorSetCount = 1;
    uint32_t mDescCount = MAXTEXTURES;
    countAllocInfoEXT.pDescriptorCounts = &mDescCount;

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = &countAllocInfoEXT;

    allocInfo.descriptorPool = core->tdescPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &core->tdSetLayout;

    if (vkAllocateDescriptorSets(core->lDev, &allocInfo, &core->tdescriptorSet) != VK_SUCCESS)
        printf("Couldnt allocate descriptor sets");
}

void write_textureDescriptorSet(VulkanCore_t core, VkImageView texture, VkSampler sampler, uint64_t textureIndex)
{
    VkWriteDescriptorSet dsWrite = {0};
    dsWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dsWrite.pNext = NULL;

    dsWrite.descriptorCount = 1;
    dsWrite.dstArrayElement = textureIndex;
    dsWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dsWrite.dstSet = core.tdescriptorSet;
    dsWrite.dstBinding = 0;

    VkDescriptorImageInfo descImgInfo = {0};
    descImgInfo.imageView = texture;
    descImgInfo.sampler = sampler;
    descImgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    dsWrite.pImageInfo = &descImgInfo;

    vkUpdateDescriptorSets(core.lDev, 1, &dsWrite, 0, NULL);
}

void createSamplers(VulkanCore_t *core)
{
    VkSamplerCreateInfo samplerCI = {0};
    samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCI.pNext = NULL;

    samplerCI.magFilter = VK_FILTER_LINEAR;
    samplerCI.minFilter = VK_FILTER_LINEAR;

    samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties props = {0};
    vkGetPhysicalDeviceProperties(core->pDev, &props);

    samplerCI.anisotropyEnable = VK_TRUE;
    samplerCI.maxAnisotropy = props.limits.maxSamplerAnisotropy;
    samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

    samplerCI.compareEnable = VK_FALSE;
    samplerCI.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCI.mipLodBias = 0.0f;
    samplerCI.minLod = 0.0f;
    samplerCI.maxLod = 0.0f;

    vkCreateSampler(core->lDev, &samplerCI, NULL, &core->linearSampler);
}

void destroyRenderer(renderer_t *renderer)
{
    vkDeviceWaitIdle(renderer->vkCore.lDev);
    vkDestroyDescriptorPool(renderer->vkCore.lDev, renderer->vkCore.tdescPool, NULL);
    vkDestroyDescriptorSetLayout(renderer->vkCore.lDev, renderer->vkCore.tdSetLayout, NULL);
    for (uint32_t i = 0; i < FRAMECOUNT; i++)
    {
        vkDestroySemaphore(renderer->vkCore.lDev, renderer->vkCore.imageAvailable[i], NULL);
        vkDestroyFence(renderer->vkCore.lDev, renderer->vkCore.fences[i], NULL);
        vkDestroyFence(renderer->vkCore.lDev, renderer->vkCore.computeFences[i], NULL);
    }
    vkDestroyFence(renderer->vkCore.lDev, renderer->vkCore.immediateFence, NULL);
    vkDestroyCommandPool(renderer->vkCore.lDev, (VkCommandPool)renderer->vkCore.commandPool, NULL);
    for (uint32_t i = 0; i <= bufferCount; i++)
    {
        if (!bufferInfo[i].active)
            continue;
        vkDestroyBuffer(renderer->vkCore.lDev, bufferInfo[i].buf.buffer, NULL);
        vkFreeMemory(renderer->vkCore.lDev, bufferInfo[i].buf.associatedMemory, NULL);
        free(bufferInfo[i].buf.mappedMemory);
    }
    for (uint32_t i = 0; i < renderer->vkCore.imgCount; i++)
    {
        vkDestroyImageView(renderer->vkCore.lDev, renderer->vkCore.swapChainImageViews[i], NULL);
        vkDestroySemaphore(renderer->vkCore.lDev, renderer->vkCore.renderFinished[i], NULL);
        vkDestroySemaphore(renderer->vkCore.lDev, renderer->vkCore.computeFinished[i], NULL);
    }
    vkDestroySampler(renderer->vkCore.lDev, renderer->vkCore.linearSampler, NULL);
    vkDestroySwapchainKHR(renderer->vkCore.lDev, renderer->vkCore.swapChain, NULL);
    vkDestroyDevice(renderer->vkCore.lDev, NULL);
    vkDestroySurfaceKHR(renderer->vkCore.instance, renderer->vkCore.surface, NULL);
    vkDestroyInstance(renderer->vkCore.instance, NULL);
}

const uint64_t maxVerts = 1000000;

void initRenderer(renderer_t *renderer)
{
    create_instance(renderer);
    create_device(&renderer->vkCore);
    create_swapchain(&renderer->vkCore);
    renderer->vkCore.currentScImg = malloc(sizeof(Image));
    *renderer->vkCore.currentScImg = (Image){
        renderer->vkCore.swapChainImages[0],
        renderer->vkCore.swapChainImageViews[0],
        VK_IMAGE_LAYOUT_UNDEFINED,
        0,
        NULL,
    };
    renderer->vkCore.currentImageIndex = 0;
    create_CommandBuffers(&renderer->vkCore);
    create_dsp(&renderer->vkCore);
    allocate_textureDescriptorSet(&renderer->vkCore);
    createSamplers(&renderer->vkCore);
    renderer->meshHandler.instancedMeshes = NULL;

    // BufferCreateInfo BCI = {0};
    // BCI.access = DEVICE_ONLY;
    // BCI.dataSize = maxVerts * sizeof(renderer->meshHandler.vertexSize);
    // BCI.usage = BUFFER_USAGE_VERTEX | BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_TRANSFER_SRC;
    // createBuffer(renderer->vkCore, BCI, &renderer->meshHandler.unifiedVerts);
    // BCI.usage = BUFFER_USAGE_INDEX | VK_BUFFER_USAGE_TRANSFER_DST_BIT | BUFFER_USAGE_TRANSFER_SRC;
    // createBuffer(renderer->vkCore, BCI, &renderer->meshHandler.unifiedIndices);
    // renderer->meshHandler.unifiedVertexCapacity = maxVerts * sizeof(renderer->meshHandler.vertexSize);
    // renderer->meshHandler.unifiedIndexCapacity = maxVerts * sizeof(uint32_t);
}

void bindGraphicsPipeline(graphicsPipeline pline, VkCommandBuffer cBuf)
{
    VkBool32 cbEnable = pline.colorBlending;
    vkCmdSetColorWriteMaskEXT_(cBuf, 0, 1, &pline.colorWriteMask);

    vkCmdSetColorBlendEnableEXT_(cBuf, 0, 1, &cbEnable);
    vkCmdSetLogicOpEnableEXT_(cBuf, pline.logicOpEnable);

    vkCmdSetDepthTestEnable(cBuf, pline.depthTestEnable);
    vkCmdSetDepthBiasEnable(cBuf, pline.depthBiasEnable);
    vkCmdSetDepthClampEnableEXT_(cBuf, pline.depthClampEnable);
    vkCmdSetDepthClipEnableEXT_(cBuf, pline.depthClipEnable);
    vkCmdSetStencilTestEnable(cBuf, pline.stencilTestEnable);
    vkCmdSetDepthWriteEnable(cBuf, pline.depthWriteEnable);
    vkCmdSetDepthBoundsTestEnable(cBuf, pline.depthBoundsEnable);
    vkCmdSetAlphaToCoverageEnableEXT_(cBuf, pline.alphaToCoverageEnable);
    vkCmdSetAlphaToOneEnableEXT_(cBuf, pline.alphaToOneEnable);

    vkCmdSetColorWriteMaskEXT_(cBuf, 0, 1, &pline.colorWriteMask);
    vkCmdSetPolygonModeEXT_(cBuf, pline.polyMode);
    vkCmdSetPrimitiveTopology(cBuf, pline.topology);
    vkCmdSetRasterizerDiscardEnable(cBuf, pline.reasterizerDiscardEnable);
    vkCmdSetPrimitiveRestartEnable(cBuf, pline.primitiveRestartEnable);
    vkCmdSetRasterizationSamplesEXT_(cBuf, pline.rastSampleCount);
    vkCmdSetFrontFace(cBuf, pline.frontFace);
    vkCmdSetCullMode(cBuf, pline.cullMode);

    if (pline.colorBlending == VK_TRUE)
        vkCmdSetColorBlendEquationEXT_(cBuf, 0, 1, &pline.colorBlendEq);

    if (pline.logicOpEnable == VK_TRUE)
        vkCmdSetLogicOpEXT_(cBuf, pline.logicOp);
    if (pline.depthTestEnable == VK_TRUE)
    {
        vkCmdSetDepthBounds(cBuf, pline.minDepth, pline.maxDepth);
        vkCmdSetDepthCompareOp(cBuf, pline.depthCompareOp);
    }
    vkCmdSetSampleMaskEXT_(cBuf, 1, &pline.sampleMask);
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_VERTEX_BIT}, &pline.vert.shader);
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_FRAGMENT_BIT}, &pline.frag.shader);
    int vertexDescCount = pline.vert.VertexDescriptons;
    vkCmdSetVertexInputEXT_(cBuf, vertexDescCount, pline.vert.bindingDesc, vertexDescCount, pline.vert.attrDesc);

    vkCmdBindDescriptorSets(cBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pline.plLayout, 0, pline.setCount, pline.descriptorSets, 0, NULL);
}

void bindComputePipeline(computePipeline pline, VkCommandBuffer cBuf)
{
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_COMPUTE_BIT}, &pline.shader.shader);
}

void unbindGraphicsPipeline(VkCommandBuffer cBuf)
{
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_VERTEX_BIT}, VK_NULL_HANDLE);
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_FRAGMENT_BIT}, VK_NULL_HANDLE);
}

void unbindComputePipeline(VkCommandBuffer cBuf)
{
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_VERTEX_BIT}, VK_NULL_HANDLE);
    vkCmdBindShadersEXT_(cBuf, 1, (VkShaderStageFlagBits[1]){VK_SHADER_STAGE_FRAGMENT_BIT}, VK_NULL_HANDLE);
}

void readShaderSPRV(const char *filePath, uint64_t *len, uint32_t **data)
{
    FILE *file;
    errno_t err = fopen_s(&file, filePath, "rb");
    if (err != 0)
    {
        printf("could not open file\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    unsigned long file_size = ftell(file);
    rewind(file);
    *len = file_size;
    *data = malloc(file_size);
    if (fread(*data, 1, file_size, file) != file_size)
    {
        printf("Error reading file\n");
        fclose(file);
        return;
    }
    fclose(file);
}

void setShaderGLSPRV(VulkanCore_t core, graphicsPipeline *pl, uint32_t *vFileContents, int vFileLen, uint32_t *fFileContents, int fFileLen)
{
    VkShaderCreateInfoEXT vCI = {0};

    vCI.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    vCI.pNext = NULL;

    vCI.flags = 0;

    vCI.setLayoutCount = pl->setLayoutCount;
    vCI.pSetLayouts = pl->setLayouts;
    vCI.pushConstantRangeCount = pl->pcRangeCount;
    vCI.pPushConstantRanges = &pl->pcRange;

    vCI.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    vCI.codeSize = vFileLen;
    vCI.pCode = vFileContents;
    vCI.pName = "main";
    vCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vCI.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vCI.pSpecializationInfo = NULL;

    VkShaderCreateInfoEXT fCI = {0};

    fCI.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    fCI.pNext = NULL;

    fCI.flags = 0;

    fCI.setLayoutCount = pl->setLayoutCount;
    fCI.pSetLayouts = pl->setLayouts;
    fCI.pushConstantRangeCount = pl->pcRangeCount;
    fCI.pPushConstantRanges = &pl->pcRange;

    fCI.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    fCI.codeSize = fFileLen;
    fCI.pCode = fFileContents;
    fCI.pName = "main";
    fCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fCI.nextStage = 0;
    fCI.pSpecializationInfo = NULL;

    VkShaderCreateInfoEXT sCi[2] = {
        vCI,
        fCI,
    };
    vkCreateShadersEXT_(core.lDev, 1, &sCi[0], NULL, &pl->vert.shader);
    vkCreateShadersEXT_(core.lDev, 1, &sCi[1], NULL, &pl->frag.shader);
}

void setShaderSLSPRV(VulkanCore_t core, graphicsPipeline *pl, uint32_t *FileContents, int FileLen)
{
    VkShaderCreateInfoEXT vCI = {0};

    vCI.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    vCI.pNext = NULL;

    vCI.flags = 0;

    vCI.setLayoutCount = pl->setLayoutCount;
    vCI.pSetLayouts = pl->setLayouts;
    vCI.pushConstantRangeCount = pl->pcRangeCount;
    vCI.pPushConstantRanges = &pl->pcRange;

    vCI.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    vCI.codeSize = FileLen;
    vCI.pCode = FileContents;
    vCI.pName = "vertMain";
    vCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vCI.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vCI.pSpecializationInfo = NULL;

    VkShaderCreateInfoEXT fCI = {0};

    fCI.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    fCI.pNext = NULL;

    fCI.flags = 0;

    fCI.setLayoutCount = pl->setLayoutCount;
    fCI.pSetLayouts = pl->setLayouts;
    fCI.pushConstantRangeCount = pl->pcRangeCount;
    fCI.pPushConstantRanges = &pl->pcRange;

    fCI.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    fCI.codeSize = FileLen;
    fCI.pCode = FileContents;
    fCI.pName = "fragMain";
    fCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fCI.nextStage = 0;
    fCI.pSpecializationInfo = NULL;

    VkShaderCreateInfoEXT sCi[2] = {
        vCI,
        fCI,
    };
    vkCreateShadersEXT_(core.lDev, 1, &sCi[0], NULL, &pl->vert.shader);
    vkCreateShadersEXT_(core.lDev, 1, &sCi[1], NULL, &pl->frag.shader);
}

void setCompShaderSPRV(VulkanCore_t core, computePipeline *pl, uint32_t *contents, int fileLen)
{
    VkShaderCreateInfoEXT cCI = {0};

    cCI.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    cCI.pNext = NULL;

    cCI.flags = 0;

    cCI.setLayoutCount = pl->setLayoutCount;
    cCI.pSetLayouts = pl->setLayouts;
    cCI.pushConstantRangeCount = pl->pcRangeCount;
    cCI.pPushConstantRanges = &pl->pcRange;

    cCI.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    cCI.codeSize = fileLen;
    cCI.pCode = contents;
    cCI.pName = "compMain";
    cCI.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cCI.nextStage = 0;
    cCI.pSpecializationInfo = NULL;

    vkCreateShadersEXT_(core.lDev, 1, (VkShaderCreateInfoEXT[1]){cCI}, NULL, &pl->shader.shader);
}

void addVertexInput(graphicsPipeline *pl, int binding, int location, int stride, int offSet, VkVertexInputRate inputRate, VkFormat format)
{
    int index = pl->vert.VertexDescriptons;
    VkVertexInputAttributeDescription2EXT attrDesc = {
        VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
        NULL,
        location,
        binding,
        format,
        offSet,
    };
    VkVertexInputBindingDescription2EXT bindingDesc = {
        VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        NULL,
        binding,
        stride,
        inputRate,
        0,
    };
    pl->vert.attrDesc[index] = attrDesc;
    pl->vert.bindingDesc[index] = bindingDesc;
    pl->vert.VertexDescriptons += 1;
}

void setPushConstantRange(graphicsPipeline *pl, size_t size, shaderStage stage)
{
    VkPushConstantRange pcRange = {0};
    pcRange.offset = 0;
    pcRange.size = size;
    pcRange.stageFlags = stage;

    pl->pcRange = pcRange;
    pl->pcRangeCount = 1;
}

void setComputePushConstantRange(computePipeline *pl, size_t size)
{
    VkPushConstantRange pcRange = {0};
    pcRange.offset = 0;
    pcRange.size = size;
    pcRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    pl->pcRange = pcRange;
    pl->pcRangeCount = 1;
}

void createPipelineLayout(VulkanCore_t core, graphicsPipeline *pl)
{
    VkPipelineLayoutCreateInfo plcInf = {0};
    plcInf.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plcInf.pNext = NULL;

    VkDescriptorSetLayout *setLayouts = malloc(sizeof(VkDescriptorSetLayout) * (pl->setLayoutCount + 1));
    setLayouts[0] = core.tdSetLayout;
    if (pl->setLayoutCount >= 1)
    {
        memcpy(setLayouts + 1, pl->setLayouts, sizeof(VkDescriptorSetLayout) * (pl->setLayoutCount));
    }
    plcInf.setLayoutCount = pl->setLayoutCount + 1;
    plcInf.pSetLayouts = setLayouts;
    if (pl->setLayouts != NULL)
    {
        free(pl->setLayouts);
    };
    pl->setLayoutCount += 1;
    pl->setLayouts = setLayouts;

    VkDescriptorSet *sets = malloc(sizeof(VkDescriptorSet) * (pl->setCount + 1));
    sets[0] = core.tdescriptorSet;
    if (pl->setCount >= 1)
    {
        memcpy(sets + 1, pl->descriptorSets, sizeof(VkDescriptorSetLayout) * (pl->setCount));
    }
    if (pl->descriptorSets != NULL)
    {
        free(pl->descriptorSets);
    }
    pl->descriptorSets = sets;
    pl->setCount += 1;

    plcInf.pPushConstantRanges = &pl->pcRange;
    plcInf.pushConstantRangeCount = pl->pcRangeCount;
    vkCreatePipelineLayout(core.lDev, &plcInf, NULL, &pl->plLayout);
}

void createComputePipelineLayout(VulkanCore_t core, computePipeline *pl)
{
    VkPipelineLayoutCreateInfo plcInf = {0};
    plcInf.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plcInf.pNext = NULL;
    VkDescriptorSetLayout *setLayouts = malloc(sizeof(VkDescriptorSetLayout) * (pl->setLayoutCount + 1));
    memcpy(setLayouts, pl->setLayouts, sizeof(VkDescriptorSetLayout) * (pl->setLayoutCount));
    setLayouts[pl->setLayoutCount] = core.tdSetLayout;

    plcInf.setLayoutCount = 0;
    plcInf.pSetLayouts = NULL;

    plcInf.pPushConstantRanges = &pl->pcRange;
    plcInf.pushConstantRangeCount = pl->pcRangeCount;
    vkCreatePipelineLayout(core.lDev, &plcInf, NULL, &pl->plLayout);
}
