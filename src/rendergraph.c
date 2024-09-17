#include <renderer.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// courtesy of https://github.com/haipome/fnv/blob/master/fnv.c <- even if its public domain it deserves credit :)
uint64_t fnv_64a_str(char *str, uint64_t hval)
{
    unsigned char *s = (unsigned char *)str; /* unsigned string */

    /*
     * FNV-1a hash each octet of the string
     */
    while (*s)
    {
        /* xor the bottom with the current octet */
        hval ^= (uint64_t)*s++;

        /* multiply by the 64 bit FNV magic prime mod 2^64 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_64_PRIME;
#else  /* NO_FNV_GCC_OPTIMIZATION */
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
                (hval << 7) + (hval << 8) + (hval << 40);
#endif /* NO_FNV_GCC_OPTIMIZATION */
    }

    /* return our new hash value */
    return hval;
}

// ------------ Pipeline Info ------------
typedef struct
{
    char *Name;
    Pipeline *pLine;
} pipelineInfo;
pipelineInfo *ap_Pipelines = NULL; // ! NOT THREAD SAFE
uint32_t pipelineCount = 0;

void cache_PipeLine(Pipeline *pLine, char *Name)
{
    pipelineInfo plInf = {0};
    plInf.Name = Name;
    plInf.pLine = pLine;

    ap_Pipelines = realloc(ap_Pipelines, sizeof(pipelineInfo) * pipelineCount + 1);
    ap_Pipelines[pipelineCount + 1] = plInf;

    pipelineCount += 1;
}

Pipeline find_Pipeline(char *Name)
{
    // worst case O(n), we could technically get away without iteration but the gain is not much, and the complexity it would add would make this much less readable
    for (uint32_t i = 0; i <= pipelineCount; i++)
    {
        if (ap_Pipelines[i].Name == Name)
        {
            return *ap_Pipelines[i].pLine;
        }
    }
    printf("Could not find specified pipeline %s\n", Name);
    Pipeline errpl = {0};
    return errpl;
}

void recordPass(VkExtent2D extent, RenderPass *pass, VkCommandBuffer cBuf)
{
    VkRenderingInfo renInf = {0};
    renInf.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renInf.pNext = NULL;

    renInf.layerCount = 1;
    renInf.renderArea = (VkRect2D){
        {0, 0},
        extent,
    };

    renInf.colorAttachmentCount = pass->cAttCount;
    renInf.pColorAttachments = pass->colorAttachments;
    renInf.pDepthAttachment = NULL;
    renInf.pStencilAttachment = NULL;

    vkCmdBeginRendering(cBuf, &renInf);

    pass->callBack(*pass, cBuf);

    vkCmdEndRendering(cBuf);
}

bool resEq(Resource rhs, Resource lhs)
{
    return rhs.value.buffer.buffer == lhs.value.buffer.buffer &&
           rhs.value.img->imgview == lhs.value.img->imgview &&
           rhs.type == lhs.type &&
           rhs.usage == lhs.usage;
}

bool resEqWoUsage(Resource rhs, Resource lhs)
{
    return rhs.type == lhs.type &&
           rhs.value.buffer.index == lhs.value.buffer.index &&
           rhs.value.img->image == lhs.value.img->image;
}

const VkImageSubresourceRange imgSRR = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    VK_REMAINING_MIP_LEVELS,
    0,
    VK_REMAINING_ARRAY_LAYERS,
};

const VkCommandBufferBeginInfo bInf = {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    NULL,
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    NULL,
};

void addResource(RenderPass *pass, Resource res)
{
    pass->resources = realloc(pass->resources, sizeof(Resource) * (pass->resourceCount + 1));
    pass->resources[pass->resourceCount] = res;
    pass->resourceCount += 1;
}

void addCatt(RenderPass *pass, VkRenderingAttachmentInfo att)
{
    pass->colorAttachments = realloc(pass->colorAttachments, sizeof(Resource) * (pass->cAttCount + 1));
    pass->colorAttachments[pass->cAttCount] = att;
    pass->cAttCount += 1;
}

uint64_t hash = 0;
RenderPass newPass(char *name, passType type)
{
    RenderPass p = {0};

    p.name = name;
    p.hash = fnv_64a_str(name, hash);
    hash = p.hash;
    p.type = type;
    p.resourceCount = 0;
    // p.resources = malloc(sizeof(Resource));
    p.cAttCount = 0;
    // p.colorAttachments = malloc(sizeof(VkRenderingAttachmentInfo));

    return p;
}

// attachments are always read | write
void addColorAttachment(Image *img, RenderPass *pass, VkClearValue *clear)
{
    VkRenderingAttachmentInfo rAttInfo = {0};
    rAttInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    rAttInfo.pNext = NULL;

    rAttInfo.imageView = img->imgview;
    rAttInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rAttInfo.loadOp = clear != NULL ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    rAttInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    if (clear)
        rAttInfo.clearValue = *clear;
    addCatt(pass, rAttInfo);
}

void setDepthStencilAttachment(Image img, RenderPass *pass)
{
    VkRenderingAttachmentInfo dAttInfo = {0};
    dAttInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    dAttInfo.pNext = NULL;

    dAttInfo.imageView = img.imgview;
    dAttInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    dAttInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    dAttInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    dAttInfo.clearValue.depthStencil.depth = 0;

    pass->depthAttachment = dAttInfo;
}
void addImageResource(RenderPass *pass, Image *image, ResourceUsageFlags_t usage)
{
    Resource res = {0};
    res.type = RES_TYPE_Image;
    res.value.img = image;
    res.usage = usage;

    if ((usage & USAGE_COLORATTACHMENT) != 0)
    {
        res.access = ACCESS_COLORATTACHMENT;
        res.value.img->CurrentLayout |= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        res.cAttIndex = pass->cAttCount;
        addColorAttachment(image, pass, NULL);
    }
    else if ((usage & USAGE_DEPTHSTENCILATTACHMENT) != 0)
    {
        res.access = ACCESS_DEPTHATTACHMENT;
        res.value.img->CurrentLayout |= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        setDepthStencilAttachment(*image, pass);
    }
    else if ((usage & USAGE_TRANSFER_SRC) != 0)
    {
        res.access = ACCESS_TRANSFER_READ;
        res.value.img->CurrentLayout |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }
    else if ((usage & USAGE_TRANSFER_DST) != 0)
    {
        res.access = ACCESS_TRANSFER_WRITE;
        res.value.img->CurrentLayout |= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
    else if ((usage & USAGE_SAMPLED) != 0)
    {
        res.access = ACCESS_READ;
        res.value.img->CurrentLayout |= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    else if ((usage & USAGE_UNDEFINED) != 0)
    {
        res.access = 0;
        res.value.img->CurrentLayout |= VK_IMAGE_LAYOUT_UNDEFINED;
    }

    addResource(pass, res);
}
// only transfers, and undefined are valid for buffers
void addBufferResource(RenderPass *pass, Buffer buf, ResourceUsageFlags_t usage)
{
    Resource res = {0};
    res.type = RES_TYPE_Buffer;
    res.value.buffer = buf;
    res.usage = usage;

    switch (res.usage)
    {
    case USAGE_TRANSFER_SRC:
        res.access = ACCESS_TRANSFER_READ;
        break;
    case USAGE_TRANSFER_DST:
        res.access = ACCESS_TRANSFER_WRITE;
        break;
    case USAGE_UNDEFINED:
        res.access = 0;
        break;
    default:
        res.access = 0;
        break;
    }

    addResource(pass, res);
}

void setPipeline(Pipeline pl, RenderPass *pass)
{
    pass->pl = pl;
}

void setExecutionCallBack(RenderPass *pass, void (*callBack)(struct RenderPass, VkCommandBuffer cBuf))
{
    pass->callBack = callBack;
}

void addPass(GraphBuilder *builder, RenderPass *pass)
{
    builder->passes = realloc(builder->passes, sizeof(RenderPass) * (builder->passCount + 1));
    builder->passes[builder->passCount] = *pass;
    builder->passCount += 1;
}
void optimizePasses(RenderGraph *graph, Image swapChainImg)
{
    int rootResourceCount = 0;
    Resource *rootResources = malloc(sizeof(Resource));

    int newPassCount = 0;
    RenderPass *newPasses = malloc(sizeof(RenderPass) * graph->passCount);

    for (int i = graph->passCount - 1; i >= 0; i--)
    {
        int imgBrrCount = 0;
        VkImageMemoryBarrier *imgMemBarriers = NULL;

        int bufBrrCount = 0;
        VkBufferMemoryBarrier *bufMemBarriers = NULL;
        RenderPass curPass = graph->passes[i];
        for (int r = 0; r <= curPass.resourceCount - 1; r++)
        {
            Resource cr = curPass.resources[r];
            if ((cr.access & ACCESS_COLORATTACHMENT) != 0 && cr.value.img->imgview == swapChainImg.imgview)
            {
                rootResources = realloc(rootResources, sizeof(Resource) * (rootResourceCount + curPass.resourceCount));
                memcpy(rootResources + rootResourceCount, curPass.resources, sizeof(Resource) * (curPass.resourceCount));
                rootResourceCount += curPass.resourceCount;

                newPasses[graph->passCount - newPassCount - 1] = curPass;
                newPassCount += 1;
                curPass.colorAttachments[cr.cAttIndex].imageView = cr.value.img->imgview;
                break;
            }
            else
            {
                for (int e = rootResourceCount; e >= 0; e--)
                {
                    if (resEqWoUsage(cr, rootResources[e]))
                    {
                        switch (cr.type)
                        {
                        case RES_TYPE_Image:
                            Resource oldImg = rootResources[e];

                            VkImageMemoryBarrier icBar;
                            icBar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                            icBar.pNext = NULL;
                            icBar.image = cr.value.img->image;

                            icBar.oldLayout = oldImg.value.img->CurrentLayout;
                            icBar.newLayout = cr.value.img->CurrentLayout;

                            icBar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                            icBar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                            icBar.srcAccessMask = oldImg.access;
                            icBar.dstAccessMask = cr.access;

                            icBar.subresourceRange = imgSRR;
                            imgMemBarriers = realloc(imgMemBarriers, sizeof(VkImageMemoryBarrier) * (imgBrrCount + 1));
                            imgMemBarriers[imgBrrCount] = icBar;
                            imgBrrCount += 1;
                            break;
                        case RES_TYPE_Buffer:
                            Resource oldBuf = rootResources[e];
                            VkBufferMemoryBarrier bcBar;
                            bcBar.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                            bcBar.pNext = NULL;

                            bcBar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                            bcBar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                            bcBar.buffer = cr.value.buffer.buffer;
                            bcBar.offset = 0;
                            bcBar.size = VK_WHOLE_SIZE;
                            bcBar.srcAccessMask = oldBuf.access;
                            bcBar.dstAccessMask = cr.access;

                            bufMemBarriers = realloc(bufMemBarriers, sizeof(VkImageMemoryBarrier) * (bufBrrCount + 1));
                            bufMemBarriers[bufBrrCount] = bcBar;
                            bufBrrCount += 1;
                            break;
                        }
                        if ((cr.usage & USAGE_COLORATTACHMENT) != 0 || (cr.usage & USAGE_TRANSFER_DST) != 0 || (cr.usage & USAGE_DEPTHSTENCILATTACHMENT) != 0)
                        {
                            rootResources = realloc(rootResources, sizeof(Resource) * (rootResourceCount + curPass.resourceCount));
                            memcpy(rootResources + rootResourceCount, curPass.resources, sizeof(Resource) * (curPass.resourceCount));
                            rootResourceCount += curPass.resourceCount;

                            newPasses[graph->passCount - newPassCount - 1] = curPass;
                            newPassCount += 1;

                            r = curPass.resourceCount + 1;
                        }
                        break;
                    }
                }
            }
        }
        graph->barriers[i] = (passBarrierInfo){
            imgBrrCount,
            bufBrrCount,
            imgMemBarriers,
            bufMemBarriers};
    }
    free(graph->passes);
    graph->passes = malloc(sizeof(RenderPass) * newPassCount);
    memcpy(graph->passes, &newPasses[graph->passCount - newPassCount], sizeof(RenderPass) * newPassCount);
    graph->passCount = newPassCount;
    free(newPasses);
    free(rootResources);
}

RenderGraph buildGraph(GraphBuilder *builder, Image scImage)
{
    RenderGraph rg = {0};
    rg.passes = builder->passes;
    rg.barriers = malloc(sizeof(passBarrierInfo) * builder->passCount);
    rg.passCount = builder->passCount;
    optimizePasses(&rg, scImage);
    builder->passes = NULL;
    builder->passCount = 0;
    return rg;
}

void executeGraph(VkExtent2D extent, RenderGraph *graph, VkCommandBuffer cBuf)
{
    for (int i = 0; i <= graph->passCount - 1; i++)
    {
        passBarrierInfo cb = graph->barriers[i];
        vkCmdPipelineBarrier(cBuf, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, NULL, cb.bufPBCount, cb.bufMemBarriers, cb.imgPBCount, cb.imgMemBarriers);
        recordPass(extent, &graph->passes[i], cBuf);
    }
}

void destroyRenderGraph(RenderGraph *graph)
{
    free(graph->barriers);
    free(graph->passes);
}
const VkClearColorValue clearValue = {{0.0f, 0.0f, 0.0f, 0.0f}};

void drawRenderer(renderer_t *renderer, int cBufIndex)
{
    VkCommandBuffer cbuf = renderer->vkCore.commandBuffers[cBufIndex];

    vkWaitForFences(renderer->vkCore.lDev, 1, &renderer->vkCore.fences[cBufIndex], VK_TRUE, UINT64_MAX);
    vkResetFences(renderer->vkCore.lDev, 1, &renderer->vkCore.fences[cBufIndex]);

    vkAcquireNextImageKHR(renderer->vkCore.lDev, renderer->vkCore.swapChain, UINT64_MAX, renderer->vkCore.imageAvailiable[cBufIndex], VK_NULL_HANDLE, &renderer->vkCore.currentImageIndex);

    *renderer->vkCore.currentScImg = (Image){
        renderer->vkCore.swapChainImages[renderer->vkCore.currentImageIndex],
        renderer->vkCore.swapChainImageViews[renderer->vkCore.currentImageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
    };

    vkResetCommandBuffer(cbuf, 0);
    vkBeginCommandBuffer(cbuf, &cBufBeginInf);

    RenderGraph rg = buildGraph(renderer->rg, *renderer->vkCore.currentScImg);

    // Change layout of image to be optimal for clearing
    VkImageMemoryBarrier imgMemoryBarrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        NULL,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        renderer->vkCore.currentScImg->image,
        imgSRR,
    };
    vkCmdPipelineBarrier(cbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imgMemoryBarrier);

    vkCmdClearColorImage(cbuf, renderer->vkCore.currentScImg->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &imgSRR);

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = renderer->vkCore.extent.width;
    scissor.extent.height = renderer->vkCore.extent.height;

    vkCmdSetScissorWithCount(cbuf, 1, &scissor);

    VkViewport viewport = {0};

    viewport.x = 0;
    viewport.y = 0;
    viewport.width = renderer->vkCore.extent.width + 1.0;
    viewport.height = renderer->vkCore.extent.height + 1.0;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    vkCmdSetViewportWithCount(cbuf, 1, &viewport);

    executeGraph(renderer->vkCore.extent, &rg, cbuf);

    imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imgMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    imgMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imgMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vkCmdPipelineBarrier(cbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, NULL, 0, NULL, 1, &imgMemoryBarrier);

    vkEndCommandBuffer(cbuf);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &renderer->vkCore.imageAvailiable[cBufIndex];
    submitInfo.pWaitDstStageMask = (VkPipelineStageFlags[1]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderer->vkCore.renderFinished[renderer->vkCore.currentImageIndex];

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cbuf;

    vkQueueSubmit(renderer->vkCore.gQueue, 1, &submitInfo, renderer->vkCore.fences[cBufIndex]);

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderer->vkCore.renderFinished[renderer->vkCore.currentImageIndex];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &renderer->vkCore.swapChain;
    presentInfo.pImageIndices = &renderer->vkCore.currentImageIndex;

    vkQueuePresentKHR(renderer->vkCore.pQueue, &presentInfo);

    destroyRenderGraph(&rg);
}