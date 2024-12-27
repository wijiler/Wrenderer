#include <math.h>
#include <renderer.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

uint64_t passTypeToVulkanStage(passType type)
{
    uint64_t stages[] = {
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,   // PASS_TYPE_GRAPHICS
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, // PASS_TYPE_COMPUTE
        VK_PIPELINE_STAGE_2_BLIT_BIT,           // PASS_TYPE_BLIT
        VK_PIPELINE_STAGE_2_TRANSFER_BIT,       // PASS_TYPE_TRANSFER
    };
    return stages[type];
}
#define CATTINFO(i)                                   \
    {                                                 \
        VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,  \
            NULL,                                     \
            pass->colorAttachments[i]->imgview,       \
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, \
            0,                                        \
            NULL,                                     \
            0,                                        \
            VK_ATTACHMENT_LOAD_OP_LOAD,               \
            VK_ATTACHMENT_STORE_OP_STORE,             \
            {{{0, 0, 0, 0}}},                         \
    }
typedef struct
{
    VkRenderingAttachmentInfo attinf[8];
} renInfo;
renInfo genCattInfo(RenderPass *pass)
{
    switch (pass->cAttCount)
    {
    case 0:
        return (renInfo){0};
    case 1:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
        }};
        return cattInfo;
    }
    case 2:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
        }};
        return cattInfo;
    }
    case 3:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(2),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
        }};
        return cattInfo;
    }
    case 4:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(2),
            CATTINFO(3),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
        }};
        return cattInfo;
    }
    case 5:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(2),
            CATTINFO(3),
            CATTINFO(4),
            CATTINFO(0),
            CATTINFO(0),
            CATTINFO(0),
        }};
        return cattInfo;
    }
    case 6:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(2),
            CATTINFO(3),
            CATTINFO(4),
            CATTINFO(5),
            CATTINFO(0),
            CATTINFO(0),
        }};
        return cattInfo;
    }
    case 7:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(2),
            CATTINFO(3),
            CATTINFO(4),
            CATTINFO(5),
            CATTINFO(6),
            CATTINFO(0),
        }};
        return cattInfo;
    }

    case 8:
    {
        renInfo cattInfo = {{
            CATTINFO(0),
            CATTINFO(1),
            CATTINFO(2),
            CATTINFO(3),
            CATTINFO(4),
            CATTINFO(5),
            CATTINFO(6),
            CATTINFO(7),
        }};
        return cattInfo;
    }

    default:
        return (renInfo){0};
    }
}

void recordPass(RenderPass *pass, renderer_t *renderer, VkCommandBuffer cBuf)
{
    VkRenderingInfo frameBuf = {0};
    frameBuf.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    frameBuf.pNext = NULL;

    frameBuf.layerCount = 1;
    frameBuf.renderArea = (VkRect2D){
        {0, 0},
        renderer->vkCore.extent,
    };

    frameBuf.colorAttachmentCount = pass->cAttCount;
    renInfo cattInfo = genCattInfo(pass);
    frameBuf.pColorAttachments = cattInfo.attinf;

    VkRenderingAttachmentInfo depthInfo = {0};
    if (pass->depthAttachment)
    {
        depthInfo = (VkRenderingAttachmentInfo){
            VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            NULL,
            pass->depthAttachment->imgview,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            0,
            NULL,
            0,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            {{{0, 0, 0, 0}}},
        };
        frameBuf.pDepthAttachment = &depthInfo;
    }
    if (pass->stencilAttachment)
    {
        frameBuf.pStencilAttachment = NULL;
    }

    vkCmdBeginRendering(cBuf, &frameBuf);

    pass->callBack(*pass, cBuf);

    vkCmdEndRendering(cBuf);
}

bool resEq(Resource rhs, Resource lhs)
{
    return rhs.value.buffer.buffer == lhs.value.buffer.buffer &&
           rhs.value.img.handle->imgview == lhs.value.img.handle->imgview &&
           rhs.type == lhs.type &&
           rhs.usage == lhs.usage;
}

bool resEqWoUsage(Resource rhs, Resource lhs)
{
    return rhs.type == lhs.type &&
           rhs.value.buffer.index == lhs.value.buffer.index &&
           rhs.value.img.handle->image == lhs.value.img.handle->image;
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
    Resource t = res;
    t.stage = pass->type;
    pass->resources = realloc(pass->resources, sizeof(Resource) * (pass->resourceCount + 1));
    pass->resources[pass->resourceCount] = t;
    pass->resourceCount += 1;
}

void addCatt(RenderPass *pass, Image *img)
{
    pass->colorAttachments[pass->cAttCount] = img;
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
    p.resources = NULL;
    p.cAttCount = 0;

    return p;
}

void addSwapchainImageResource(RenderPass *pass, renderer_t renderer)
{
    Resource res = {0};
    res.type = RES_TYPE_Image;
    res.value.swapChainImage = currentScImg;
    res.usage = USAGE_COLORATTACHMENT;
    addResource(pass, res);
    addColorAttachment(currentScImg, pass, NULL);
}

const VkColorComponentFlags colorFlags = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
const VkBool32 colorBlendingEnable = VK_TRUE;
const VkColorBlendEquationEXT colorBlenEq = {
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD,
};

void addColorAttachment(Image *img, RenderPass *pass, VkClearValue *clear)
{
    pass->cbEnable[pass->cAttCount] = colorBlendingEnable;
    pass->colorflags[pass->cAttCount] = colorFlags;
    pass->colorBlend[pass->cAttCount] = colorBlenEq;

    addCatt(pass, img);
}

void setDepthAttachment(Image *img, RenderPass *pass)
{
    pass->depthAttachment = img;
}
void addImageResource(RenderPass *pass, Image *image, ResourceUsageFlags_t usage)
{
    Resource res = {0};
    res.type = RES_TYPE_Image;
    res.value.img.handle = image;
    res.usage = usage;

    if ((usage & USAGE_COLORATTACHMENT) != 0)
    {
        res.access = ACCESS_COLORATTACHMENT;
        res.value.img.layout |= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        res.cAttIndex = pass->cAttCount;
        res.value.img.handle->accessMask |= ACCESS_COLORATTACHMENT;
        addColorAttachment(image, pass, NULL);
    }
    else if ((usage & USAGE_DEPTHSTENCILATTACHMENT) != 0)
    {
        res.access = ACCESS_DEPTHATTACHMENT;
        res.value.img.layout |= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        res.value.img.accessMask |= ACCESS_DEPTHATTACHMENT;
        setDepthAttachment(image, pass);
    }
    else if ((usage & USAGE_TRANSFER_SRC) != 0)
    {
        res.access = ACCESS_TRANSFER_READ;
        res.value.img.layout |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        res.value.img.accessMask |= ACCESS_TRANSFER_READ;
    }
    else if ((usage & USAGE_TRANSFER_DST) != 0)
    {
        res.access = ACCESS_TRANSFER_WRITE;
        res.value.img.layout |= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        res.value.img.accessMask |= ACCESS_TRANSFER_WRITE;
    }
    else if ((usage & USAGE_SAMPLED) != 0)
    {
        res.access = ACCESS_READ;
        res.value.img.layout |= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        res.value.img.accessMask |= ACCESS_READ;
    }
    else if ((usage & USAGE_UNDEFINED) != 0)
    {
        res.access = 0;
        res.value.img.layout |= VK_IMAGE_LAYOUT_UNDEFINED;
        res.value.img.accessMask = 0;
    }

    addResource(pass, res);
}

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

void addArbitraryResource(RenderPass *pass, void *data)
{
    Resource res = {0};
    res.type = RES_TYPE_Arb;
    res.value.arbitrary = data;

    addResource(pass, res);
}

void setGraphicsPipeline(graphicsPipeline pl, RenderPass *pass)
{
    pass->gPl = pl;
}
void setComputePipeline(computePipeline pl, RenderPass *pass)
{
    pass->cPl = pl;
}

void setExecutionCallBack(RenderPass *pass, void (*callBack)(struct RenderPass, VkCommandBuffer cBuf))
{
    pass->callBack = callBack;
}

void optimizePasses(RenderGraph *graph, Image *swapChainImg)
{
    int rootResourceCount = 0;
    Resource *rootResources = NULL;

    int newPassCount = 0;
    RenderPass *newPasses = malloc(sizeof(RenderPass) * graph->passCount);
    graph->barriers = malloc(sizeof(passBarrierInfo) * graph->passCount);

    for (int i = graph->passCount - 1; i >= 0; i--)
    {
        int imgBrrCount = 0;
        VkImageMemoryBarrier2 *imgMemBarriers = NULL;

        int bufBrrCount = 0;
        VkBufferMemoryBarrier2 *bufMemBarriers = NULL;
        RenderPass curPass = graph->passes[i];
        for (int r = curPass.resourceCount - 1; r >= 0; r--)
        {
            Resource cr = curPass.resources[r];
            if (cr.usage == USAGE_COLORATTACHMENT)
                curPass.colorAttachments[cr.cAttIndex]->imgview = cr.value.img.handle->imgview;
            if (cr.value.swapChainImage == swapChainImg)
            {
                cr.value.swapChainImage = swapChainImg;
                rootResources = realloc(rootResources, sizeof(Resource) * (rootResourceCount + curPass.resourceCount));
                memcpy(rootResources + rootResourceCount, curPass.resources, sizeof(Resource) * (curPass.resourceCount));
                rootResourceCount += curPass.resourceCount;

                newPasses[graph->passCount - newPassCount - 1] = curPass;
                newPassCount += 1;
                curPass.colorAttachments[cr.cAttIndex]->imgview = cr.value.swapChainImage->imgview;
                break;
            }
            else
            {
                for (int e = rootResourceCount - 1; e >= 0; e--)
                {
                    if (resEqWoUsage(cr, rootResources[e]))
                    {
                        switch (cr.type)
                        {
                        case RES_TYPE_Image:
                        {
                            Resource oldImg = rootResources[e];

                            VkImageMemoryBarrier2 icBar;
                            icBar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                            icBar.pNext = NULL;

                            icBar.image = cr.value.img.handle->image;
                            icBar.subresourceRange = imgSRR;

                            icBar.oldLayout = cr.value.img.layout;
                            icBar.newLayout = oldImg.value.img.layout;

                            icBar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                            icBar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                            icBar.srcStageMask = passTypeToVulkanStage(cr.stage);
                            icBar.srcAccessMask = cr.access;
                            icBar.dstStageMask = passTypeToVulkanStage(oldImg.stage);
                            icBar.dstAccessMask = oldImg.access;

                            imgMemBarriers = realloc(imgMemBarriers, sizeof(VkImageMemoryBarrier2) * (imgBrrCount + 1));
                            imgMemBarriers[imgBrrCount] = icBar;
                            imgBrrCount += 1;
                            e = -1;
                        }
                        break;
                        case RES_TYPE_Buffer:
                        {
                            Resource oldBuf = rootResources[e];
                            VkBufferMemoryBarrier2 bcBar;
                            bcBar.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
                            bcBar.pNext = NULL;

                            bcBar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                            bcBar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                            bcBar.buffer = cr.value.buffer.buffer;
                            bcBar.offset = 0;
                            bcBar.size = VK_WHOLE_SIZE;
                            bcBar.srcStageMask = passTypeToVulkanStage(oldBuf.stage);
                            bcBar.srcAccessMask = oldBuf.access;
                            bcBar.dstStageMask = passTypeToVulkanStage(cr.stage);
                            bcBar.dstAccessMask = cr.access;

                            bufMemBarriers = realloc(bufMemBarriers, sizeof(VkBufferMemoryBarrier2) * (bufBrrCount + 1));
                            bufMemBarriers[bufBrrCount] = bcBar;
                            bufBrrCount += 1;
                            e = -1;
                        }
                        break;
                        default:
                            break;
                        }
                        if ((cr.usage & USAGE_COLORATTACHMENT) != 0 || (cr.usage & USAGE_TRANSFER_DST) != 0 || (cr.usage & USAGE_DEPTHSTENCILATTACHMENT) != 0)
                        {
                            rootResources = realloc(rootResources, sizeof(Resource) * (rootResourceCount + 1));
                            rootResources[rootResourceCount] = cr;
                            rootResourceCount += 1;
                        }
                    }
                }
            }
        }
        int barrierIndex = (graph->passCount - i) - 1;
        graph->barriers[barrierIndex].bufMemBarriers = NULL;
        graph->barriers[barrierIndex].imgMemBarriers = NULL;
        graph->barriers[barrierIndex].bufPBCount = bufBrrCount;
        graph->barriers[barrierIndex].imgPBCount = imgBrrCount;
        if (bufMemBarriers != NULL || imgMemBarriers != NULL)
        {
            newPasses[graph->passCount - newPassCount - 1] = curPass;
            newPassCount += 1;
        }
        if (bufMemBarriers != NULL)
        {
            graph->barriers[barrierIndex].bufMemBarriers = malloc(sizeof(VkBufferMemoryBarrier2) * bufBrrCount);
            memcpy(graph->barriers[barrierIndex].bufMemBarriers, bufMemBarriers, sizeof(VkBufferMemoryBarrier2) * bufBrrCount);
            free(bufMemBarriers);
        }

        if (imgMemBarriers != NULL)
        {
            graph->barriers[barrierIndex].imgMemBarriers = malloc(sizeof(VkImageMemoryBarrier2) * imgBrrCount);
            memcpy(graph->barriers[barrierIndex].imgMemBarriers, imgMemBarriers, sizeof(VkImageMemoryBarrier2) * imgBrrCount);
            free(imgMemBarriers);
        }
    }
    free(graph->passes);
    graph->passes = malloc(sizeof(RenderPass) * newPassCount);
    memcpy(graph->passes, &newPasses[graph->passCount - newPassCount], sizeof(RenderPass) * newPassCount);
    graph->passCount = newPassCount;

    passBarrierInfo lastBarrier = graph->barriers[graph->passCount - 1];
    passBarrierInfo resetInfo = {0};
    resetInfo.imgPBCount = lastBarrier.imgPBCount;
    resetInfo.bufMemBarriers = NULL;
    resetInfo.imgMemBarriers = malloc(sizeof(VkImageMemoryBarrier2) * lastBarrier.imgPBCount);
    memcpy(resetInfo.imgMemBarriers, lastBarrier.imgMemBarriers, sizeof(VkImageMemoryBarrier2) * lastBarrier.imgPBCount);
    for (int i = 0; i < resetInfo.imgPBCount; i++)
    {
        VkImageLayout oldLayout = resetInfo.imgMemBarriers[i].oldLayout;
        VkImageLayout newLayout = resetInfo.imgMemBarriers[i].newLayout;
        resetInfo.imgMemBarriers[i].oldLayout = newLayout;
        resetInfo.imgMemBarriers[i].newLayout = oldLayout;
    }
    graph->barriers[0] = resetInfo;
    free(newPasses);
    free(rootResources);
}

RenderGraph buildGraph(GraphBuilder *builder, Image *scImage)
{
    RenderGraph rg = {0};
    rg.passes = malloc(sizeof(RenderPass) * builder->passCount);
    memcpy(rg.passes, builder->passes, sizeof(RenderPass) * builder->passCount);
    rg.barriers = malloc(sizeof(passBarrierInfo) * builder->passCount);
    rg.passCount = builder->passCount;
    optimizePasses(&rg, scImage);

    return rg;
}

void addPass(GraphBuilder *builder, RenderPass *pass)
{
    builder->passes = realloc(builder->passes, sizeof(RenderPass) * (builder->passCount + 1));
    builder->passes[builder->passCount] = *pass;
    builder->passCount += 1;
    builder->graph = buildGraph(builder, currentScImg);
}

void executeGraph(RenderGraph *graph, renderer_t *renderer, uint32_t cBufIndex)
{
    for (int i = 0; i < graph->passCount; i++)
    {
        VkCommandBuffer cbuffer = NULL;
        if (graph->passes[i].type == PASS_TYPE_COMPUTE)
        {
            cbuffer = renderer->vkCore.computeCommandBuffers[cBufIndex];
        }
        else
        {
            cbuffer = renderer->vkCore.commandBuffers[cBufIndex];
        }
        {
            passBarrierInfo cb = graph->barriers[i];
            VkDependencyInfo depInfo = {
                VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                NULL,
                0,
                0,
                NULL,
                cb.bufPBCount,
                cb.bufMemBarriers,
                cb.imgPBCount,
                cb.imgMemBarriers,
            };
            vkCmdPipelineBarrier2(cbuffer, &depInfo);
        }
        recordPass(&graph->passes[i], renderer, cbuffer);
    }
}

void destroyRenderGraph(RenderGraph *graph)
{
    free(graph->barriers);
    free(graph->passes);
}
const VkClearColorValue clearValue = {{0.0f, 0.0f, 0.0f, 0.0f}};
bool swapchainCorrect = true;
uint64_t submitValue = 0;
float elapsed = {0};
uint64_t frames = 0;
void drawRenderer(renderer_t *renderer, int cBufIndex)
{
    clock_t initTime = clock();
    VkCommandBuffer gcbuf = renderer->vkCore.commandBuffers[cBufIndex];
    VkCommandBuffer ccbuf = renderer->vkCore.computeCommandBuffers[cBufIndex];
    {
        VkSemaphoreWaitInfo semaWaitInf = {
            VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            NULL,
            0,
            2,
            (VkSemaphore[2]){renderer->vkCore.computeTimeline, renderer->vkCore.graphicsTimeline},
            (uint64_t[2]){submitValue, submitValue},
        };
        vkWaitSemaphores(renderer->vkCore.lDev, &semaWaitInf, UINT64_MAX);
        submitValue++;
    }

    VkResult result;
    if (swapchainCorrect)
    {
        result = vkAcquireNextImageKHR(renderer->vkCore.lDev, WREswapChain, UINT64_MAX, renderer->vkCore.imageAvailable[cBufIndex], VK_NULL_HANDLE, &currentSCImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            swapchainCorrect = false;
    }
    if (!swapchainCorrect)
        return;

    *currentScImg = (Image){
        WREswapChainImages[currentSCImageIndex],
        WREswapChainImageViews[currentSCImageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        0,
        NULL,
    };

    vkResetCommandBuffer(gcbuf, 0);
    vkResetCommandBuffer(ccbuf, 0);
    vkBeginCommandBuffer(gcbuf, &cBufBeginInf);
    vkBeginCommandBuffer(ccbuf, &cBufBeginInf);
    vkCmdResetQueryPool(gcbuf, renderer->vkCore.timestampPool, 0, 2);

    RenderGraph rg = renderer->rg->graph;

    VkImageMemoryBarrier imgMemoryBarrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        NULL,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        currentScImg->image,
        imgSRR,
    };
    vkCmdPipelineBarrier(gcbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imgMemoryBarrier);
    currentScImg->CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    currentScImg->accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdClearColorImage(gcbuf, currentScImg->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &imgSRR);

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = renderer->vkCore.extent.width;
    scissor.extent.height = renderer->vkCore.extent.height;

    vkCmdSetScissorWithCount(gcbuf, 1, &scissor);

    VkViewport viewport = {0};

    viewport.x = 0;
    viewport.y = 0;
    viewport.width = renderer->vkCore.extent.width;
    viewport.height = renderer->vkCore.extent.height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    vkCmdSetViewportWithCount(gcbuf, 1, &viewport);
    vkCmdWriteTimestamp(gcbuf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, renderer->vkCore.timestampPool, 0);
    executeGraph(&rg, renderer, cBufIndex);
    vkCmdWriteTimestamp(gcbuf, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, renderer->vkCore.timestampPool, 1);

    imgMemoryBarrier.srcAccessMask = currentScImg->accessMask;
    imgMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    imgMemoryBarrier.oldLayout = currentScImg->CurrentLayout;
    imgMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vkCmdPipelineBarrier(gcbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, NULL, 0, NULL, 1, &imgMemoryBarrier);
    currentScImg->CurrentLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    currentScImg->accessMask = VK_ACCESS_MEMORY_READ_BIT;

    vkEndCommandBuffer(gcbuf);
    vkEndCommandBuffer(ccbuf);
    {
        VkCommandBufferSubmitInfo cBufSubmit = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            NULL,
            ccbuf,
            0,
        };
        VkSemaphoreSubmitInfo timelineSemaphoreSubmitInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            NULL,
            renderer->vkCore.computeTimeline,
            submitValue,
            VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            0,
        };
        VkSubmitInfo2 submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            NULL,
            0,
            0,
            NULL,
            1,
            &cBufSubmit,
            1,
            &timelineSemaphoreSubmitInfo,
        };
        vkQueueSubmit2(renderer->vkCore.compQueue, 1, &submitInfo, NULL);
    }

    {
        VkCommandBufferSubmitInfo cBufSubmit = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            NULL,
            gcbuf,
            0,
        };
        VkSemaphoreSubmitInfo timelineSemaphoreSubmitInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            NULL,
            renderer->vkCore.graphicsTimeline,
            submitValue,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            0,
        };
        VkSemaphoreSubmitInfo graphicsSemaphoreSubmitInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            NULL,
            renderer->vkCore.renderFinished[currentSCImageIndex],
            0,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            0,
        };
        VkSemaphoreSubmitInfo graphicsSemaphoreWaitInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            NULL,
            renderer->vkCore.imageAvailable[cBufIndex],
            0,
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            0,
        };
        VkSubmitInfo2 submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            NULL,
            0,
            1,
            &graphicsSemaphoreWaitInfo,
            1,
            &cBufSubmit,
            2,
            (VkSemaphoreSubmitInfo[2]){timelineSemaphoreSubmitInfo, graphicsSemaphoreSubmitInfo},
        };

        vkQueueSubmit2(renderer->vkCore.gQueue, 1, &submitInfo, NULL);

        VkPresentInfoKHR presentInfo = {0};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = NULL;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderer->vkCore.renderFinished[currentSCImageIndex];

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &WREswapChain;
        presentInfo.pImageIndices = &currentSCImageIndex;

        result = 0;
        if ((result = vkQueuePresentKHR(renderer->vkCore.pQueue, &presentInfo)) == VK_ERROR_OUT_OF_DATE_KHR)
        {
            swapchainCorrect = false;
        }
    }
    vkGetQueryPoolResults(renderer->vkCore.lDev, renderer->vkCore.timestampPool, 0, 2, 4 * sizeof(uint64_t), WREstats.timeStampValues, 2 * sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
    VkPhysicalDeviceProperties devProps = {0};
    vkGetPhysicalDeviceProperties(WREPhysicalDevice, &devProps);
    if (WREstats.timeStampValues[3] != 0)
    {
        WREstats.deltaTime = (float)(WREstats.timeStampValues[2] - WREstats.timeStampValues[0]) * devProps.limits.timestampPeriod / 1000000.0f;
        elapsed += ((clock() - initTime) * 1000.f / CLOCKS_PER_SEC) + WREstats.deltaTime;
    }
    frames += 1;
    if (elapsed >= 1000)
    {
        WREstats.avgFPS = frames;
        frames = 0;
        elapsed = 0;
    }
    printf("[INFO] FPS: %f FRAMETIME: %f ms\n", WREstats.avgFPS, WREstats.deltaTime);
    // destroyRenderGraph(&rg);
}

void copyGraph(GraphBuilder *src, GraphBuilder *dst)
{
    dst->passes = realloc(dst->passes, sizeof(RenderPass) * (dst->passCount + src->passCount));
    memcpy(dst->passes + dst->passCount, src->passes, sizeof(RenderPass) * src->passCount);
    dst->passCount += src->passCount;
}

void removePass(GraphBuilder *builder, const char *name)
{
    if (builder->passCount < 0)
    {
        return;
    }

    bool removed = false;
    for (int i = 0; i < builder->passCount; i++)
    {
        if (strcmp(builder->passes[i].name, name))
        {
            free(builder->passes[i].resources);
            free(builder->passes[i].colorAttachments);
            builder->passes[i] = (RenderPass){0}; // could be removed
            removed = true;
        }
        if (removed)
        {
            if (i == builder->passCount - 1)
                break;
            builder->passes[i] = builder->passes[i + 1];
        }
    }
    if (builder->passCount > 0)
    {
        builder->passCount -= 1;
        builder->passes = realloc(builder->passes, sizeof(RenderPass) * builder->passCount);
    }
}