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
        VERTEX,
        FRAGMENT,
    } shaderType;

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
    // typedef struct
    // {
    // } UniformBuffer;

    typedef struct
    {
        float worldMatrix[16];
    } pushConstants;

    typedef struct
    {
        VkColorComponentFlags colorWriteMask;
        VkColorBlendEquationEXT colorBlendEq;
        VkPolygonMode polyMode;
        VkPrimitiveTopology topology;
        VkSampleCountFlagBits rastSampleCount;
        VkFrontFace frontFace;
        VkCullModeFlags cullMode;

        VkBool32 colorBlending, logicOpEnable, depthTestEnable, depthBiasEnable, depthClampEnable,
            depthClipEnable, stencilTestEnable, depthWriteEnable, depthBoundsEnable, alphaToCoverageEnable, alphaToOneEnable;

        VkLogicOp logicOp;    // if logicOp is changed logicOpEnable must be true
        uint32_t *sampleMask; // can be [0]

        VkCompareOp depthCompareOp;
        int minDepth, maxDepth; // must be set if depthTestEnable is VK_TRUE
    } Pipeline;

    typedef struct
    {
        VkVertexInputAttributeDescription2EXT attrDesc;
        VkVertexInputBindingDescription2EXT bindingDesc;
        shaderType type;
        VkShaderEXT shader;
    } Shader;

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

        VkDescriptorPool tdescPool;
        VkDescriptorSetLayout tdSetLayout;
        VkDescriptorSet tdescriptorSets[MAXTEXTURES - 1];

        GLFWwindow *window;

    } VulkanCore_t;

    typedef struct
    {
        VulkanCore_t vkCore;
    } renderer_t;

    void initRenderer(renderer_t *renderer);

    Buffer findBuffer(int index);
    void createBuffer(VulkanCore_t core, BufferCreateInfo *createInfo, Buffer *buf);
    void pushDataToBuffer(VulkanCore_t core, void *data, uint32_t dataSize, Buffer buf);
    void copyBuf(VulkanCore_t core, Buffer src, Buffer dest);

    void allocate_textureDescriptorSets(VulkanCore_t *core, uint64_t setCount);

    void cache_PipeLine(Pipeline *pLine, char *Name);
    Pipeline find_Pipeline(char *Name);

    void destroyBuffer(Buffer buf, VulkanCore_t core);
    void destroyRenderer(renderer_t *renderer);
#ifdef __cplusplus
}
#endif
#endif