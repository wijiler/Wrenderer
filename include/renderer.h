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

    // ----------------------------------------- RGBEG
    static const VkCommandBufferBeginInfo cBufBeginInf = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        NULL};
    typedef enum
    {
        BUFFER_USAGE_VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        BUFFER_USAGE_INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        BUFFER_USAGE_TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        BUFFER_USAGE_TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        BUFFER_USAGE_STORAGE_BUFFER = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    } BufferUsage;

    typedef enum
    {
        SHADER_STAGE_VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
        SHADER_STAGE_FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
    } shaderStage;

    typedef enum
    {
        CPU_ONLY = 0x01,
        DEVICE_ONLY = 0x02,
    } BufferAccess;

    typedef struct
    {
        int dataSize;
        BufferUsage usage;
        BufferAccess access;
    } BufferCreateInfo;

    typedef struct
    {
        BufferUsage type;
        uint32_t index;
        size_t size;
        VkBuffer buffer;
        VkDeviceMemory associatedMemory;
        void *mappedMemory;
    } Buffer;

    typedef struct
    {
        Buffer verticies;
        Buffer indices;
        uint32_t instanceCount;
    } Mesh;

    typedef struct
    {
        uint64_t unifiedVertexCapacity;
        uint64_t unifiedIndexCapacity;
        size_t unifiedVertexSize;
        size_t unifiedIndexSize;
        Buffer unifiedVerts;
        Buffer unifiedIndices;

        uint32_t instancedmeshCount;
        Mesh *instancedMeshes;
    } MeshHandler;

    typedef struct
    {
        VkImage image;
        VkImageView imgview;
        // Do not set manually
        VkImageLayout CurrentLayout;
    } Image;

    typedef struct
    {
        int VertexDescriptons; // max vertex descriptions is 256, why the fuck would you want more you sick fuck
        VkVertexInputAttributeDescription2EXT attrDesc[256];
        VkVertexInputBindingDescription2EXT bindingDesc[256];
        VkShaderEXT shader;
    } Shader;

    typedef struct
    {
        Shader vert, frag;

        VkColorComponentFlags colorWriteMask;
        VkColorBlendEquationEXT colorBlendEq;
        VkPolygonMode polyMode;
        VkPrimitiveTopology topology;
        VkSampleCountFlagBits rastSampleCount;
        VkFrontFace frontFace;
        VkCullModeFlags cullMode;

        VkBool32 colorBlending, logicOpEnable, depthTestEnable, depthBiasEnable, depthClampEnable,
            depthClipEnable, stencilTestEnable, depthWriteEnable, depthBoundsEnable, alphaToCoverageEnable, alphaToOneEnable,
            reasterizerDiscardEnable, primitiveRestartEnable;

        VkLogicOp logicOp;   // if logicOp is changed logicOpEnable must be true
        uint32_t sampleMask; // usually UINT32_MAX

        int pcRangeCount;
        VkPushConstantRange pcRange;
        void *PushConstants;

        VkPipelineLayout plLayout;

        int setLayoutCount;
        VkDescriptorSetLayout *setLayouts;

        size_t uboSize;
        void *ubo;

        VkCompareOp depthCompareOp;
        int minDepth, maxDepth; // must be set if depthTestEnable is VK_TRUE
    } Pipeline;

    void bindPipeline(Pipeline pline, VkCommandBuffer cBuf);
    void unBindPipeline(VkCommandBuffer cBuf);

    typedef enum
    {
        RES_TYPE_Buffer,
        RES_TYPE_Image,
    } Resourcetype;
    typedef enum
    {
        ACCESS_READ = VK_ACCESS_SHADER_READ_BIT,
        ACCESS_WRITE = VK_ACCESS_SHADER_WRITE_BIT,
        ACCESS_TRANSFER_READ = VK_ACCESS_TRANSFER_READ_BIT,
        ACCESS_TRANSFER_WRITE = VK_ACCESS_TRANSFER_WRITE_BIT,
        ACCESS_COLORATTACHMENT = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        ACCESS_DEPTHATTACHMENT = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,

    } accessFlags;
    typedef enum
    {
        USAGE_UNDEFINED,
        USAGE_SAMPLED,
        USAGE_DEPTHSTENCILATTACHMENT,
        USAGE_COLORATTACHMENT,
        USAGE_TRANSFER_SRC,
        USAGE_TRANSFER_DST,
    } ResourceUsageFlags_t;
    typedef struct
    {
        Resourcetype type;
        accessFlags access;
        ResourceUsageFlags_t usage;
        int cAttIndex; // only used if the image is a color attachment but very useful
        union resourceVal
        {
            Buffer buffer;
            Image *img;
            Mesh mesh;
        } value;
    } Resource;
    typedef enum
    {
        PASS_TYPE_GRAPHICS,
        // compute will be added soon
        PASS_TYPE_BLIT,
    } passType;
    /*
     * We are able to tell if another pass is dependent of the other by the resources it uses
     * Ex.
     *      Pass 1 has a write handle to ColorAttachmenta
     *      Pass 2 needs to read from ColorAttachmenta
     *      We should place a memory barrier before
     *      pass 2 executes to stop it from reading
     *      Old memory
     */
    typedef struct RenderPass
    {
        char *name;
        uint64_t hash;

        Pipeline pl;
        passType type;

        // for renderingBeginInfo
        int cAttCount;
        VkRenderingAttachmentInfo *colorAttachments;
        VkRenderingAttachmentInfo depthAttachment;
        VkRenderingAttachmentInfo stencilAttachment;

        int resourceCount;
        Resource *resources;

        void (*callBack)(struct RenderPass pass, VkCommandBuffer cBuf);
    } RenderPass;

    typedef struct
    {
        int imgPBCount;
        int bufPBCount;
        VkImageMemoryBarrier *imgMemBarriers;
        VkBufferMemoryBarrier *bufMemBarriers;
    } passBarrierInfo;
    typedef struct
    {
        int passCount;
        passBarrierInfo *barriers;
        RenderPass *passes;

    } RenderGraph;

    typedef struct
    {
        int passCount;
        RenderPass *passes;
    } GraphBuilder;
    // ----------------------------------------- RGEND

    typedef struct
    {
        VkInstance instance;

        VkPhysicalDevice pDev;
        uint32_t qfi;
        VkSurfaceKHR surface;
        VkDevice lDev;
        VkQueue pQueue, gQueue;

        VkSwapchainKHR swapChain;
        VkExtent2D extent;
        VkSurfaceFormatKHR sFormat;
        VkPresentModeKHR sPresentMode;
        VkImage *swapChainImages;
        unsigned int imgCount;
        uint32_t currentImageIndex;
        VkImageView *swapChainImageViews;
        Image *currentScImg;

        VkFence fences[FRAMECOUNT];
        VkFence immediateFence;
        VkSemaphore imageAvailiable[FRAMECOUNT];
        VkSemaphore *renderFinished;

        VkCommandPool *commandPool;
        VkCommandBuffer commandBuffers[FRAMECOUNT];
        VkCommandBuffer immediateSubmit;

        VkDescriptorPool tdescPool;
        VkDescriptorSetLayout tdSetLayout;
        VkDescriptorSet tdescriptorSet;

        GLFWwindow *window;

    } VulkanCore_t;

    typedef struct
    {
        VulkanCore_t vkCore;
        MeshHandler meshHandler;
        GraphBuilder *rg;
    } renderer_t;

    void recreateSwapchain(renderer_t *renderer);

    void initRenderer(renderer_t *renderer);

    Buffer findBuffer(int index);
    void createBuffer(VulkanCore_t core, BufferCreateInfo createInfo, Buffer *buf);
    void pushDataToBuffer(void *data, size_t dataSize, Buffer buf, int offSet);
    void copyBuf(VulkanCore_t core, Buffer src, Buffer dest, size_t size, uint32_t srcOffset, uint32_t dstOffset);

    void write_textureDescriptorSet(VulkanCore_t core, VkImageView texture, VkSampler sampler, uint64_t textureIndex);

    void destroyBuffer(Buffer buf, VulkanCore_t core);
    void destroyRenderer(renderer_t *renderer);

    void drawRenderer(renderer_t *renderer, int cBufIndex);

    // ---------------------------- RGFUNBEG

    void cache_PipeLine(Pipeline *pLine, char *Name);
    Pipeline find_Pipeline(char *Name);
    void bindPipeline(Pipeline pline, VkCommandBuffer cBuf);
    void readShaderSPRV(const char *filePath, uint64_t *len, uint32_t **data);
    void setShaderSPRV(VulkanCore_t core, Pipeline *pl, uint32_t *vFileContents, int vFileLen, uint32_t *fFileContents, int fFileLen);
    void addVertexInput(Pipeline *pl, int binding, int location, int stride, int offSet, VkVertexInputRate inputRate, VkFormat format);
    void setPushConstantRange(VulkanCore_t core, Pipeline *pl, size_t size, shaderStage stage);

    RenderPass newPass(char *name, passType type);

    void setPipeline(Pipeline pl, RenderPass *pass);
    void addImageResource(RenderPass *pass, Image *image, ResourceUsageFlags_t usage);
    void addBufferResource(RenderPass *pass, Buffer buf, ResourceUsageFlags_t usage);
    void addColorAttachment(Image *img, RenderPass *pass, VkClearValue *clear);
    void setDepthStencilAttachment(Image img, RenderPass *pass);
    void setExecutionCallBack(RenderPass *pass, void (*callBack)(RenderPass pass, VkCommandBuffer cBuf));

    void addPass(GraphBuilder *builder, RenderPass *pass);
    RenderGraph buildGraph(GraphBuilder *builder, Image scImage);
    void destroyRenderGraph(RenderGraph *graph);
    void executeGraph(VkExtent2D extent, RenderGraph *graph, VkCommandBuffer cBuf);

    uint64_t fnv_64a_str(char *str, uint64_t hval);

    // ---------------------------- RGFUNEND

    // ----------------------------------------- MODELFUNBG

    void addMeshResource(RenderPass *pass, Mesh mesh, ResourceUsageFlags_t usage);

    Mesh createMesh(renderer_t renderer, uint32_t vertCount, float vertices[], uint32_t indexCount, uint32_t indices[], uint32_t instanceCount);
    void submitMesh(Mesh mesh, renderer_t *renderer);
    RenderPass sceneDraw(renderer_t *renderer);
    // ----------------------------------------- MODELFUNEND

#ifdef __cplusplus
}
#endif
#endif