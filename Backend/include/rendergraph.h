#ifndef RENDERGRAPH_H_
#define RENDERGRAPH_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <renderer.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

    typedef struct
    {
        VkImage image;
        VkImageView imgview;
        // Do not set manually
        VkImageLayout CurrentLayout;
    } Image;

    typedef struct
    {
        VkVertexInputAttributeDescription2EXT attrDesc;
        VkVertexInputBindingDescription2EXT bindingDesc;
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
            depthClipEnable, stencilTestEnable, depthWriteEnable, depthBoundsEnable, alphaToCoverageEnable, alphaToOneEnable;

        VkLogicOp logicOp;    // if logicOp is changed logicOpEnable must be true
        uint32_t *sampleMask; // can be [0]

        VkCompareOp depthCompareOp;
        int minDepth, maxDepth; // must be set if depthTestEnable is VK_TRUE
    } Pipeline;

    void bindPipeline(Pipeline pline, VkCommandBuffer cBuf);

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
        union resourceVal
        {
            Buffer buffer; // used if RES_TYPE_Buffer
            Image img;
        } value;
    } Resource;
    typedef enum
    {
        PASS_TYPE_GRAPHICS,
        // compute will be added soon
        PASS_TYPE_TRANSFER,
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

        void (*callBack)(struct RenderPass pass, VkCommandBuffer cBuf); // allows us to use lambdas in the C++ wrapper
    } RenderPass;
    typedef struct
    {
        int resourceCount;
        int passCount;
        RenderPass *passes;
        Resource *resources;
    } Edge;
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
        renderer_t renderer;
    } GraphBuilder;

    void cache_PipeLine(Pipeline *pLine, char *Name);
    Pipeline find_Pipeline(char *Name);
    void bindPipeline(Pipeline pline, VkCommandBuffer cBuf);

    RenderPass newPass(char *name, passType type);

    void setPipeline(Pipeline pl, RenderPass *pass);
    void addImageResource(RenderPass *pass, Image image, ResourceUsageFlags_t usage);
    void addBufferResource(RenderPass *pass, Buffer buf, ResourceUsageFlags_t usage);
    void addColorAttachment(Image img, RenderPass *pass, VkClearValue *clear);
    void setDepthStencilAttachment(Image img, RenderPass *pass);
    void setExecutionCallBack(RenderPass *pass, void (*callBack)(RenderPass pass, VkCommandBuffer cBuf));

    void addPass(GraphBuilder *builder, RenderPass *pass);
    RenderGraph buildGraph(GraphBuilder *builder, Image scImage);
    void destroyRenderGraph(RenderGraph *graph);
    void executeGraph(RenderGraph *graph, VkCommandBuffer cBuf);

    uint64_t fnv_64a_str(char *str, uint64_t hval);

#ifdef __cplusplus
}
#endif
#endif