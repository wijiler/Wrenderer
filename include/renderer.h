#ifndef REND
#define REND
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#define MAXTEXTURES 4096
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
        NULL,
    };
    typedef enum
    {
        BUFFER_USAGE_VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR,
        BUFFER_USAGE_INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR,
        BUFFER_USAGE_TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        BUFFER_USAGE_TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        BUFFER_USAGE_STORAGE_BUFFER = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR,
        BUFFER_USAGE_UNIFORM_BUFFER = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR,
    } BufferUsage;

    typedef enum
    {
        SHADER_STAGE_VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
        SHADER_STAGE_FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
        SHADER_STAGE_ALL = VK_SHADER_STAGE_ALL,
    } shaderStage;

    typedef enum
    {
        CPU_ONLY = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        DEVICE_ONLY = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    } MemoryAccess;

    typedef struct
    {
        int dataSize;
        BufferUsage usage;
        MemoryAccess access;
    } BufferCreateInfo;

    typedef struct
    {
        BufferUsage type;
        uint32_t index;
        size_t size;
        VkBuffer buffer;
        VkDeviceMemory associatedMemory;
        void *mappedMemory;
        VkDeviceAddress gpuAddress;
    } Buffer;
    typedef struct
    {
        Buffer *buffer;
        uint64_t capacity, filled;
    } VirtualBufferParent;
    typedef struct
    {
        VirtualBufferParent parentBuffer;
        uint64_t offSet;
    } VirtualBuffer;

    typedef struct
    {
        Buffer verticies;
        Buffer indices;
        uint32_t instanceCount;
    } Mesh;

    typedef struct
    {
        uint32_t instancedmeshCount;
        Mesh *instancedMeshes;
    } MeshHandler;

    typedef struct
    {
        VkImage image;
        VkImageView imgview;
        // Do not set manually
        VkImageLayout CurrentLayout;
        VkImageAspectFlags aspectMask;
        VkFormat format;
        VkAccessFlags accessMask;
        VkDeviceMemory memory;
    } Image;
    typedef struct
    {
        Image img;
        uint64_t index;
    } Texture;
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

        VkBool32 colorBlending, logicOpEnable, depthTestEnable, depthBiasEnable, depthClampEnable, depthClipEnable, stencilTestEnable, depthWriteEnable, depthBoundsEnable, alphaToCoverageEnable, alphaToOneEnable, reasterizerDiscardEnable, primitiveRestartEnable;

        VkLogicOp logicOp;   // if logicOp is changed logicOpEnable must be true
        uint32_t sampleMask; // usually UINT32_MAX

        int pcRangeCount;
        VkPushConstantRange pcRange;
        void *PushConstants;

        VkPipelineLayout plLayout;

        int setLayoutCount;
        VkDescriptorSetLayout *setLayouts;

        int setCount;
        VkDescriptorSet *descriptorSets;

        VkCompareOp depthCompareOp;
        int minDepth, maxDepth; // must be set if depthTestEnable is VK_TRUE

    } graphicsPipeline;

    typedef struct
    {
        Shader shader;

        int pcRangeCount;
        VkPushConstantRange pcRange;
        void *PushConstants;

        VkPipelineLayout plLayout;

        int setLayoutCount;
        VkDescriptorSetLayout *setLayouts;

        int setCount;
        VkDescriptorSet *descriptorSets;
    } computePipeline;

    typedef enum
    {
        RES_TYPE_Arb,
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
        USAGE_UNDEFINED = 1 << 0,
        USAGE_SAMPLED = 1 << 1,
        USAGE_DEPTHSTENCILATTACHMENT = 1 << 2,
        USAGE_COLORATTACHMENT = 1 << 3,
        USAGE_TRANSFER_SRC = 1 << 4,
        USAGE_TRANSFER_DST = 1 << 5,
    } ResourceUsageFlags_t;
    typedef enum
    {
        PASS_TYPE_GRAPHICS = 0,
        PASS_TYPE_COMPUTE,
        PASS_TYPE_BLIT,
        PASS_TYPE_TRANSFER,
    } passType;
    typedef struct
    {
        Resourcetype type;
        accessFlags access;
        ResourceUsageFlags_t usage;
        int cAttIndex; // only used if the image is a color attachment but very useful
        passType stage;
        union resourceVal
        {
            Buffer buffer;
            Image img;
            Image *swapChainImage;
            void *arbitrary;
        } value;
    } Resource;

    typedef struct
    {
        VkOffset2D *offSet;
        VkExtent2D *extent;
    } drawArea;

    typedef struct RenderPass
    {
        char *name;
        uint64_t hash;

        graphicsPipeline gPl;
        computePipeline cPl;
        passType type;

        // for renderingBeginInfo
        int cAttCount;
        VkRenderingAttachmentInfo *colorAttachments;
        VkRenderingAttachmentInfo *depthAttachment;
        VkRenderingAttachmentInfo *stencilAttachment;

        int resourceCount;
        Resource *resources;

        void (*callBack)(struct RenderPass pass, VkCommandBuffer cBuf);
    } RenderPass;

    typedef struct
    {
        int imgPBCount;
        int bufPBCount;
        VkImageMemoryBarrier2 *imgMemBarriers;
        VkBufferMemoryBarrier2 *bufMemBarriers;
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
        uint32_t compQfi;
        VkSurfaceKHR surface;
        VkDevice lDev;
        VkQueue pQueue, gQueue;
        VkQueue compQueue;

        VkSwapchainKHR swapChain;
        VkExtent2D extent;
        VkSurfaceFormatKHR sFormat;
        VkPresentModeKHR sPresentMode;
        VkImage *swapChainImages;
        unsigned int imgCount;
        uint32_t currentImageIndex;
        VkImageView *swapChainImageViews;
        Image *currentScImg;

        VkSemaphore graphicsTimeline;
        VkSemaphore computeTimeline;
        VkFence immediateFence;
        VkSemaphore imageAvailable[FRAMECOUNT];
        VkSemaphore *renderFinished;

        VkCommandPool *commandPool;
        VkCommandBuffer commandBuffers[FRAMECOUNT];
        VkCommandBuffer computeCommandBuffers[FRAMECOUNT];
        VkCommandBuffer immediateSubmit;

        VkDescriptorPool tdescPool;
        int textureCount;
        VkDescriptorSetLayout tdSetLayout;
        VkDescriptorSet tdescriptorSet;

        VkSampler linearSampler;
        VkSampler nearestSampler;

        GLFWwindow *window;

    } VulkanCore_t;

    typedef struct
    {
        VulkanCore_t vkCore;
        MeshHandler meshHandler; // TODO: Remove
        GraphBuilder *rg;
    } renderer_t;

    void recreateSwapchain(renderer_t *renderer);

    void initRenderer(renderer_t *renderer);

    Buffer findBuffer(int index);
    void createBuffer(VulkanCore_t core, BufferCreateInfo createInfo, Buffer *buf);
    void pushDataToBuffer(void *data, size_t dataSize, Buffer buf, int offSet);
    void copyBuf(VulkanCore_t core, Buffer src, Buffer dest, size_t size, uint32_t srcOffset, uint32_t dstOffset);

    void destroyBuffer(Buffer buf, VulkanCore_t core);
    void destroyRenderer(renderer_t *renderer);

    void drawRenderer(renderer_t *renderer, int cBufIndex);

    // ---------------------------- RGFUNBEG

    void readShaderSPRV(const char *filePath, uint64_t *len, uint32_t **data);
    void setShaderGLSPRV(VulkanCore_t core, graphicsPipeline *pl, uint32_t *vFileContents, int vFileLen, uint32_t *fFileContents, int fFileLen);
    void setShaderSLSPRV(VulkanCore_t core, graphicsPipeline *pl, uint32_t *FileContents, int FileLen);
    void setCompShaderSPRV(VulkanCore_t core, computePipeline *pl, uint32_t *contents, int fileLen);
    void addVertexInput(graphicsPipeline *pl, int binding, int location, int stride, int offSet, VkVertexInputRate inputRate, VkFormat format);
    void setPushConstantRange(graphicsPipeline *pl, size_t size, shaderStage stage, uint32_t offset);
    void setComputePushConstantRange(computePipeline *pl, size_t size);
    void createPipelineLayout(VulkanCore_t core, graphicsPipeline *pl);
    void createComputePipelineLayout(VulkanCore_t core, computePipeline *pl);
    void addDescriptorSetToCPL(VkDescriptorSet *set, computePipeline *pl);
    void addDescriptorSetToGPL(VkDescriptorSet *set, graphicsPipeline *pl);
    void addSetLayoutToCPL(VkDescriptorSetLayout *layout, computePipeline *pl);
    void addSetLayoutToGPL(VkDescriptorSetLayout *layout, graphicsPipeline *pl);

    RenderPass newPass(char *name, passType type);

    void setGraphicsPipeline(graphicsPipeline pl, RenderPass *pass);
    void setComputePipeline(computePipeline pl, RenderPass *pass);
    void addImageResource(RenderPass *pass, Image *image, ResourceUsageFlags_t usage);
    void addSwapchainImageResource(RenderPass *pass, renderer_t renderer);
    void addBufferResource(RenderPass *pass, Buffer buf, ResourceUsageFlags_t usage);
    void addArbitraryResource(RenderPass *pass, void *data);
    void addColorAttachment(Image *img, RenderPass *pass, VkClearValue *clear);
    void setDepthStencilAttachment(Image img, RenderPass *pass);
    void setExecutionCallBack(RenderPass *pass, void (*callBack)(RenderPass pass, VkCommandBuffer cBuf));

    void addPass(GraphBuilder *builder, RenderPass *pass);
    void copyGraph(GraphBuilder *src, GraphBuilder *dst);
    void removePass(GraphBuilder *builder, const char *name);

    uint64_t fnv_64a_str(char *str, uint64_t hval);

    // ----------------------------------------- RGFUNEND

    // ----------------------------------------- MODELFUNBG

    Mesh createMesh(renderer_t renderer, uint32_t vertCount, void *vertices, uint32_t indexCount, uint32_t indices[], uint32_t instanceCount, size_t vertexSize);
    void submitMesh(Mesh mesh, MeshHandler *handler);
    void removeMesh(Mesh mesh, MeshHandler *handler, renderer_t renderer);
    RenderPass sceneDraw(renderer_t *renderer, MeshHandler *handler, char *name);
    // ----------------------------------------- MODELFUNEND

    // ----------------------------------------- IMGUTILBEG
    Image createImage(VulkanCore_t core, VkImageUsageFlags usage, VkFormat format, VkImageType type, uint32_t width, uint32_t height, VkImageAspectFlags aspects);
    Texture createTexture(VulkanCore_t core, uint32_t width, uint32_t height);
    void copyDataToTextureImage(VulkanCore_t core, Image *image, Buffer *buffer, uint32_t width, uint32_t height);
    void submitTexture(renderer_t *renderer, Texture *tex, VkSampler sampler);
    void write_textureDescriptorSet(VulkanCore_t core, VkImageView texture, VkSampler sampler, uint64_t textureIndex);
    void submitNormalMap(VulkanCore_t core, VkImageView texture);
    void markImageResizable(Image *img, uint32_t *width, uint32_t *height);
    void transitionLayout(VkCommandBuffer cBuf, Image *img, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
    // ----------------------------------------- IMGUTILEND

    void bindGraphicsPipeline(graphicsPipeline pline, RenderPass pass, VkCommandBuffer cBuf);
    void bindComputePipeline(computePipeline pline, VkCommandBuffer cBuf);
    void unbindGraphicsPipeline(VkCommandBuffer cBuf);
    void unbindComputePipeline(VkCommandBuffer cBuf);

    //
    void immediateSubmitBegin(VulkanCore_t core);
    void immediateSubmitEnd(VulkanCore_t core);

#ifdef __cplusplus
}
#endif
#endif