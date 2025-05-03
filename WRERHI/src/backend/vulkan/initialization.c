#include <backends/vulkan/debug.h>
#include <backends/vulkan/initialization.h>
#include <stdio.h>

#define INSTEXTCOUNT 4
#define DEVEXTCOUNT 3

VkInstance WREvulkInstance = VK_NULL_HANDLE;
VkPhysicalDevice WREpDevice = VK_NULL_HANDLE;
VkDevice WREdevice = VK_NULL_HANDLE;
VkCommandPool WREcommandPool = VK_NULL_HANDLE;
VkQueue WREgraphicsQueue, WREpresentQueue, WREcomputeQueue, WREtransferQueue = VK_NULL_HANDLE;
VkFence WREinstantFence = VK_NULL_HANDLE;
VkCommandBuffer WREinstantCommandBuffer = VK_NULL_HANDLE;
VkDeviceMemory WREStagingMemory = VK_NULL_HANDLE;
void *WREstagingMappedMemory = NULL;
VkBuffer WREstagingBuffer = VK_NULL_HANDLE;
int32_t deviceLocalHeapIndex = -1, hostSharedHeapIndex = -1;

int graphicsQueueIndex = -1;

const char *instanceExtensions[INSTEXTCOUNT] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_XLIB_KHR
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};
const char *deviceExtensions[DEVEXTCOUNT] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
};

char *messageSevToString(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        return "ERROR";
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return "WARNING";
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        return "INFO";
    else
        return "";
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        printf("WRERen %s: Validation Layer: %s, Type: %i, %p\n", messageSevToString(messageSeverity), pCallbackData->pMessage, messageType, pUserData);
    }
    return VK_FALSE;
}

void createVulkanInstance()
{
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 290);
    appInfo.pEngineName = "Wrenderer";
    appInfo.pApplicationName = "Wrenderer";

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = INSTEXTCOUNT;
    createInfo.ppEnabledExtensionNames = instanceExtensions;

#ifdef DEBUG
    createInfo.enabledLayerCount = 1;

    createInfo.ppEnabledLayerNames = (const char *[1]){"VK_LAYER_KHRONOS_validation"};

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
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_CInf;
#endif
    VkResult result = vkCreateInstance(&createInfo, NULL, &WREvulkInstance);
    if (result != VK_SUCCESS)
    {
        printf("WREREN: Could not initialize vulkan\n");
        exit(1);
    }
}
void createDevice(VkSurfaceKHR surface)
{
    uint32_t devCount = 0;
    vkEnumeratePhysicalDevices(WREvulkInstance, &devCount, NULL);
    if (devCount == 0)
    {
        printf("WRERen ERROR: cant find any vulkan supporting GPU's\n");
        exit(1);
    }
    VkPhysicalDevice *physicalDevices = malloc(sizeof(VkPhysicalDevice) * devCount);
    vkEnumeratePhysicalDevices(WREvulkInstance, &devCount, physicalDevices);
    for (uint32_t i = 0; i < devCount; i++)
    {
        VkPhysicalDevice device = physicalDevices[i];

        VkPhysicalDeviceProperties devProp;

        VkPhysicalDeviceVulkan13Features devFeat13 = {0};
        devFeat13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        devFeat13.pNext = NULL;
        VkPhysicalDeviceVulkan12Features devFeat12 = {0};
        devFeat12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        devFeat12.pNext = &devFeat13;
        VkPhysicalDeviceFeatures2 devFeat2 = {0};
        devFeat2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        devFeat2.pNext = &devFeat12;

        vkGetPhysicalDeviceProperties(device, &devProp);
        vkGetPhysicalDeviceFeatures2(device, &devFeat2);

        if (devProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && devFeat12.timelineSemaphore == VK_TRUE &&
            devProp.limits.timestampPeriod != 0 && devFeat12.bufferDeviceAddress == VK_TRUE &&
            devFeat13.synchronization2 == VK_TRUE && devFeat13.dynamicRendering == VK_TRUE)
        {
            WREpDevice = device;
            break;
        }
    }
    if (WREpDevice == VK_NULL_HANDLE)
    {
        printf("WRERen Error: Could not pick a suitable device\n");
        exit(1);
    }

    VkQueueFamilyProperties qfamProps[8];

    unsigned int queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(WREpDevice, &queue_family_count, NULL);
    queue_family_count = min(queue_family_count, 8);
    vkGetPhysicalDeviceQueueFamilyProperties(WREpDevice, &queue_family_count, qfamProps);

    for (uint32_t j = 0; j < queue_family_count; j++)
    {
        VkBool32 supports_present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(WREpDevice, j, surface, &supports_present);
        if (qfamProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && qfamProps[j].queueFlags & VK_QUEUE_COMPUTE_BIT && qfamProps[j].queueFlags & VK_QUEUE_TRANSFER_BIT && supports_present == VK_TRUE && qfamProps[j].timestampValidBits != 0)
        {
            graphicsQueueIndex = j;
            break;
        }
    }
    free(physicalDevices);

    VkPhysicalDeviceVulkan13Features devFeat13 = {0};
    devFeat13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    devFeat13.pNext = NULL;
    VkPhysicalDeviceVulkan12Features devFeat12 = {0};
    devFeat12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    devFeat12.pNext = &devFeat13;
    VkPhysicalDeviceFeatures2 devFeat2 = {0};
    devFeat2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    devFeat2.pNext = &devFeat12;

    devFeat12.bufferDeviceAddress = VK_TRUE;
    devFeat12.timelineSemaphore = VK_TRUE;
    devFeat13.dynamicRendering = VK_TRUE;
    devFeat13.synchronization2 = VK_TRUE;

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo devCreateInf = {0};
    devCreateInf.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInf.pNext = &devFeat2;

    devCreateInf.pQueueCreateInfos = &queueCreateInfo;
    devCreateInf.queueCreateInfoCount = 1;
    devCreateInf.enabledExtensionCount = DEVEXTCOUNT;
    devCreateInf.ppEnabledExtensionNames = deviceExtensions;

    VkResult devResult;
    devResult = vkCreateDevice(WREpDevice, &devCreateInf, NULL, &WREdevice);
    if (devResult != VK_SUCCESS)
    {
        printf("WRERen Error: Could not create device because of %i", devResult);
        exit(1);
    }
    vkGetDeviceQueue(WREdevice, graphicsQueueIndex, 0, &WREgraphicsQueue);
    WREpresentQueue = WREgraphicsQueue;
    WREcomputeQueue = WREgraphicsQueue;
    WREtransferQueue = WREgraphicsQueue;
}

void createSwapchain(RendererWindowContext *windowContext, VkSwapchainKHR swapChain)
{
    VkSurfaceCapabilitiesKHR capabilities = {0};

    VkSurfaceFormatKHR *surfaceFormats = NULL;
    VkPresentModeKHR *surfacePresentModes = NULL;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(WREpDevice, windowContext->surface, &capabilities);

    uint32_t formatCount = 0;
    uint32_t pModeCount = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(WREpDevice, windowContext->surface, &formatCount, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(WREpDevice, windowContext->surface, &pModeCount, NULL);

    surfaceFormats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    surfacePresentModes = malloc(sizeof(VkPresentModeKHR) * formatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(WREpDevice, windowContext->surface, &formatCount, surfaceFormats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(WREpDevice, windowContext->surface, &pModeCount, surfacePresentModes);

    windowContext->surfaceFormat = surfaceFormats[0];
    for (unsigned int i = 0; i < formatCount; i++)
    {
        if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            windowContext->surfaceFormat = surfaceFormats[i];
            break;
        }
    }

    windowContext->surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (unsigned int i = 0; i < pModeCount; i++)
    {
        if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            windowContext->surfacePresentMode = surfacePresentModes[i];
            break;
        }
    }

    free(surfaceFormats);
    free(surfacePresentModes);

    VkSwapchainCreateInfoKHR swapchainCI;
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.pNext = NULL;
    swapchainCI.flags = 0;

    swapchainCI.imageExtent = (VkExtent2D){windowContext->w, windowContext->h};
    swapchainCI.imageFormat = windowContext->surfaceFormat.format;
    swapchainCI.imageColorSpace = windowContext->surfaceFormat.colorSpace;
    swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.minImageCount = capabilities.minImageCount + 1 < capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchainCI.preTransform = capabilities.currentTransform;

    swapchainCI.presentMode = windowContext->surfacePresentMode;

    swapchainCI.clipped = VK_TRUE;

    swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapchainCI.surface = windowContext->surface;

    swapchainCI.oldSwapchain = swapChain;

    if (vkCreateSwapchainKHR(WREdevice, &swapchainCI, NULL, &windowContext->swapChain) != VK_SUCCESS)
    {
        printf("WRERen ERROR: Swapchain could not be created");
        exit(1);
    }
    if (swapChain != NULL)
    {
        vkDestroySwapchainKHR(WREdevice, swapChain, NULL);
    }
    vkGetSwapchainImagesKHR(WREdevice, windowContext->swapChain, &windowContext->SCImgCount, NULL);
    VkImage *images = malloc(sizeof(VkImage) * windowContext->SCImgCount);
    windowContext->SCImgs = malloc(sizeof(WREVKImage) * windowContext->SCImgCount);
    vkGetSwapchainImagesKHR(WREdevice, windowContext->swapChain, &windowContext->SCImgCount, images);
    for (uint32_t i = 0; i < windowContext->SCImgCount; i++)
    {
        windowContext->SCImgs[i].img = images[i];
        windowContext->SCImgs[i].format = windowContext->surfaceFormat.format;
        windowContext->SCImgs[i].Layout = VK_IMAGE_LAYOUT_UNDEFINED;
        windowContext->SCImgs[i].extent = (VkExtent2D){windowContext->w, windowContext->h};
        windowContext->SCImgs[i].access = 0;
        CreateImageView(&windowContext->SCImgs[i], VK_IMAGE_ASPECT_COLOR_BIT);
    }
    free(images);
}

void createCommandBuffers(RendererCoreContext *objects, RendererWindowContext *windowContext)
{
    // timeline sema's
    {
        VkSemaphoreTypeCreateInfo typeInf = {
            VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            NULL,
            VK_SEMAPHORE_TYPE_TIMELINE,
            0,
        };
        VkSemaphoreCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            &typeInf,
            0,
        };
        vkCreateSemaphore(WREdevice, &createInfo, NULL, &objects->graphicsTimeline);
        setVkDebugName("WREGraphicsTimeline", VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)objects->graphicsTimeline);
    }
    // binary sema's
    {
        VkSemaphoreCreateInfo semaphoreCI = {0};
        semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCI.pNext = NULL;
        semaphoreCI.flags = 0;
        for (uint32_t i = 0; i < FramesInFlightCount; i++)
        {
            vkCreateSemaphore(WREdevice, &semaphoreCI, NULL, &objects->imgAvailable[i]);
            setVkDebugName("WREImgAvailableSema", VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)objects->imgAvailable[i]);
        }
        objects->renderFinished = malloc(sizeof(VkSemaphore) * windowContext->SCImgCount);
        for (uint32_t i = 0; i < windowContext->SCImgCount; i++)
        {
            vkCreateSemaphore(WREdevice, &semaphoreCI, NULL, &objects->renderFinished[i]);
            setVkDebugName("WRERenFinishedSema", VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)objects->renderFinished[i]);
        }
    }
    {
        if (WREcommandPool == VK_NULL_HANDLE)
        {
            VkCommandPoolCreateInfo cPoolCI = {0};
            cPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            cPoolCI.queueFamilyIndex = graphicsQueueIndex;
            if (vkCreateCommandPool(WREdevice, &cPoolCI, NULL, &WREcommandPool) != VK_SUCCESS)
            {
                printf("WRERen ERROR: Could not create commandPool\n");
                exit(1);
            }
            setVkDebugName("WRECommandPool", VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)WREcommandPool);
        }
        VkCommandBufferAllocateInfo commandBufferAllocInf = {0};
        commandBufferAllocInf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocInf.pNext = NULL;

        commandBufferAllocInf.commandPool = (VkCommandPool)WREcommandPool;
        commandBufferAllocInf.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocInf.commandBufferCount = FramesInFlightCount;
        VkResult gCbuf;
        gCbuf = vkAllocateCommandBuffers(WREdevice, &commandBufferAllocInf, objects->graphicsCommandBuffers);
        setVkDebugName("WREGCBuf1", VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)objects->graphicsCommandBuffers[0]);
        setVkDebugName("WREGCBuf2", VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)objects->graphicsCommandBuffers[1]);
        setVkDebugName("WREGCBuf3", VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)objects->graphicsCommandBuffers[2]);
        if (gCbuf != VK_SUCCESS)
        {
            printf("WRERen ERROR: could not allocate graphics command buffers\n");
            exit(1);
        }

        commandBufferAllocInf.commandBufferCount = 1;
        vkAllocateCommandBuffers(WREdevice, &commandBufferAllocInf, &WREinstantCommandBuffer);

        VkFenceCreateInfo fenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, VK_FENCE_CREATE_SIGNALED_BIT};
        vkCreateFence(WREdevice, &fenceCreateInfo, NULL, &WREinstantFence);
        setVkDebugName("InstantFence", VK_OBJECT_TYPE_FENCE, (uint64_t)WREinstantFence);
    }
}

void initializeVulkan(RendererCoreContext *objects, RendererWindowContext *windowContext, GLFWwindow *window)
{
    if (WREvulkInstance == NULL)
    {
        createVulkanInstance();
    }
    VkResult surfaceResult;
    surfaceResult = glfwCreateWindowSurface(WREvulkInstance, window, NULL, &windowContext->surface);
    if (surfaceResult != VK_SUCCESS)
    {
        printf("WRERen: could not create surface\n");
    }
    if (WREpDevice == NULL)
    {
        createDevice(windowContext->surface);
        setVkDebugName("WrendererVulkanInstance", VK_OBJECT_TYPE_INSTANCE, (uint64_t)WREvulkInstance);
        setVkDebugName("WrendererWindowSurface", VK_OBJECT_TYPE_SURFACE_KHR, (uint64_t)windowContext->surface);
        setVkDebugName("WrendererPhysicalDevice", VK_OBJECT_TYPE_PHYSICAL_DEVICE, (uint64_t)WREpDevice);
        setVkDebugName("WrendererDevice", VK_OBJECT_TYPE_DEVICE, (uint64_t)WREdevice);
        setVkDebugName("WrendererGPQueue", VK_OBJECT_TYPE_QUEUE, (uint64_t)WREgraphicsQueue);
    }
    createSwapchain(windowContext, NULL);
    windowContext->CurrentSCImg = malloc(sizeof(WREVKImage));
    *windowContext->CurrentSCImg = windowContext->SCImgs[0];
    setVkDebugName("WrenderSwapchain", VK_OBJECT_TYPE_SWAPCHAIN_KHR, (uint64_t)windowContext->swapChain);
    for (uint32_t i = 0; i < windowContext->SCImgCount; i++)
    {
        setVkDebugName("WrenderSCImg", VK_OBJECT_TYPE_IMAGE, (uint64_t)windowContext->SCImgs[i].img);
        setVkDebugName("WrenderSCImgView", VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)windowContext->SCImgs[i].imgview);
    }
    createCommandBuffers(objects, windowContext);
    if (WREstagingBuffer == VK_NULL_HANDLE)
    {
        VkPhysicalDeviceMemoryProperties physicalMemProps = {0};

        vkGetPhysicalDeviceMemoryProperties(WREpDevice, &physicalMemProps);
        for (uint32_t i = 0; i <= physicalMemProps.memoryTypeCount; i++)
        {
            if ((physicalMemProps.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0)
                hostSharedHeapIndex = i;
            else if ((physicalMemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)
                deviceLocalHeapIndex = i;
            if (hostSharedHeapIndex != -1 && deviceLocalHeapIndex != -1)
                break;
        }

        VkMemoryAllocateInfo vkMemAlloc = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            NULL,
            1000000,
            hostSharedHeapIndex,
        };
        vkAllocateMemory(WREdevice, &vkMemAlloc, NULL, &WREStagingMemory);

        VkBufferCreateInfo vkBufCInf = {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            NULL,
            0,
            1000000,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            0,
            NULL,
        };
        VkResult result;
        if ((result = vkCreateBuffer(WREdevice, &vkBufCInf, NULL, &WREstagingBuffer)) != VK_SUCCESS)
        {
            printf("WRERen Error: Coult not create buffer\n");
        }

        vkBindBufferMemory(WREdevice, WREstagingBuffer, WREStagingMemory, 0);
        vkMapMemory(WREdevice, WREStagingMemory, 0, 1000000, 0, &WREstagingMappedMemory);
        setVkDebugName("WREStagingBuffer", VK_OBJECT_TYPE_BUFFER, (uint64_t)WREstagingBuffer);
        setVkDebugName("WREStagingBufferMemory", VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t)WREStagingMemory);
    }
}

#undef INSTEXTCOUNT
#undef DEVEXTCOUNT
