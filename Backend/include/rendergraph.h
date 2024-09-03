#ifndef RENDERGRAPH_H_
#define RENDERGRAPH_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <vulkan/vulkan.h>

    typedef enum
    {
        RES_TYPE_Buffer,
        RES_TYPE_Image, // may be useful in future
    } Resourcetype;
    typedef enum
    {
        READ = 0x01,  // If only read is present assume readonly
        WRITE = 0x02, // same here but with write
    } ResourceUsageFlags_t;
    typedef struct
    {
        Resourcetype type;
        ResourceUsageFlags_t usage;

        uint8_t flags;   // BufferUsageFlags if RES_TYPE_Buffer ImageUsageFlags if RES_TYPE_Image
        int BufferIndex; // used if RES_TYPE_Buffer
        VkImageView image;
    } Resource;

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

        int colorAttCount;
        VkRenderingAttachmentInfo *ColorAttachments;
        VkRenderingAttachmentInfo depthAttachment;   // Assume Default Depth Attachment
        VkRenderingAttachmentInfo stencilAttachment; // Assume No Stencil Attachment

        int resourceCount;
        Resource *resources;

        void (*callBack)(VkCommandBuffer, struct RenderPass); // allows us to use lambdas in the C++ wrapper
    } RenderPass;

    typedef struct
    {
        int passCount;
        VkImageMemoryBarrier *imgMemoryBarriers;
        VkBufferMemoryBarrier *bufMemoryBarriers;
        RenderPass *passes;

    } RenderGraph;

    typedef struct
    {
        RenderPass *passes;

    } GraphBuilder;

    uint64_t
    fnv_64a_str(char *str, uint64_t hval);
    void recordPass(RenderPass *pass, VkCommandBuffer cBuf);

#ifdef __cplusplus
}
#endif
#endif