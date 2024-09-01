#ifndef REND
#define REND
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#define MAXTEXTURES 2048
#define FRAMECOUNT 3

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        VertexBuf,
        IndexBuf,
    } BUFFERTYPE;

    typedef enum
    {
        STAGE_VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
        STAGE_FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
    } shaderStage;

    typedef enum
    {
        HOST_ACCESS,   // CPU_ONLY
        DEVICE_ACCESS, // GPU_ONLY
    } BufferAccess;

    typedef struct
    {
        int dataSize;
        VkBufferUsageFlags usage;
        BufferAccess access;
    } BufferCreateInfo;

    typedef struct
    {
        BUFFERTYPE type;
        uint32_t index, size;
        VkBuffer buffer;
        VkDeviceMemory *associatedMemory;
    } Buffer;
    typedef struct
    {
    } UniformBuffer;

    // -------------------------------------------
    // rendergraph could be seperated out into a seperate headerfile
    // but its also a core part of the rendering system
    typedef struct
    {
        float worldMatrix[16];
    } pushConstants;

    typedef struct
    {
        VkColorComponentFlags colorWriteMask;
        VkBool32 colorBlending, logicOpEnable, depthTestEnable, depthBiasEnable, depthClampEnable,
            depthClipEnable, stencilTestEnable, depthWriteEnable, depthBoundsEnable, alphaToCoverageEnable;
        VkLogicOp logicOp;
        VkPolygonMode polyMode;
        VkPrimitiveTopology topology;
        uint32_t *sampleMask;
        VkSampleCountFlagBits rastSampleCount;
        VkFrontFace frontFace;
        VkCullModeFlags cullMode;
        VkVertexInputAttributeDescription attrDesc;
        VkVertexInputBindingDescription bindingDesc;
    } Pipeline;

    void cache_PipeLine(Pipeline *pLine, char *Name);
    Pipeline find_Pipeline(char *Name);

    typedef struct Rendergraph_leaf_t
    {
        char *passName; // actually a string
        // char *resources; // slew of bytes // Better way is likely available, is this needed? // Test and see
        int bufferCount, childCount;
        Buffer buffers[256];
        Pipeline pipeLine;
        VkShaderEXT vert, frag;
        pushConstants pc;
        struct Rendergraph_leaf_t *children;
    } Rendergraph_leaf_t;

    typedef struct
    {
        int rootPassCount;
        char *resources;
        Rendergraph_leaf_t *rootPasses;
        Rendergraph_leaf_t endpass;
    } Rendergraph_t;

    // -------------------------------------------

    typedef struct
    {
        VkInstance instance;

        VkPhysicalDevice pDev;
        int qfi;
        VkSurfaceKHR surface;
        VkDevice lDev;
        VkQueue pQueue, gQueue;

        VkSwapchainKHR swapChain;
        VkSurfaceFormatKHR sFormat;
        VkPresentModeKHR sPresentMode;
        VkImage *swapChainImages;
        unsigned int imgCount;
        VkImageView *swapChainImageViews;

        VkFence fences[FRAMECOUNT];
        VkSemaphore imageAvailiable[FRAMECOUNT];
        VkSemaphore renderFinished[FRAMECOUNT];

        int currentBuffer;
        VkCommandPool *commandPool;
        VkCommandBuffer commandBuffers[FRAMECOUNT];

        VkDescriptorPool descPool;
        VkDescriptorSetLayout dSetLayouts[MAXTEXTURES - 1];
        VkDescriptorSet descriptorSets[MAXTEXTURES - 1];

        GLFWwindow *window;

    } VulkanCore_t;

    typedef struct
    {
        Rendergraph_t *rendergraph;
        VulkanCore_t vkCore;
    } renderer_t;

    void createBuffer(VulkanCore_t core, BufferCreateInfo *createInfo, Buffer *buf);
    void initRenderer(renderer_t *renderer);
    void pushDataToBuffer(VulkanCore_t core, void *data, uint32_t dataSize,
                          Buffer buf);
    void copyBuf(VulkanCore_t core, Buffer src, Buffer dest);
    void destroyBuffer(Buffer buf, VulkanCore_t core);
    void destroyRenderer(renderer_t *renderer);
#ifdef __cplusplus
}
#endif
#endif