#include <rendergraph.h>
#include <stdbool.h>
#include <stdio.h>
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
    if (pipelineCount == 0)
    {
        ap_Pipelines = malloc(sizeof(pipelineInfo));
        pipelineCount += 1;

        ap_Pipelines[0] = plInf;
        return;
    }
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

void bindPipeline(Pipeline pline, VkCommandBuffer cBuf)
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
}

void recordPass(RenderPass *pass, VkCommandBuffer cBuf)
{
    VkRenderingInfo renInf = {0};
    renInf.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renInf.pNext = NULL;

    renInf.colorAttachmentCount = pass->cAttCount;
    renInf.pColorAttachments = pass->colorAttachments;
    renInf.pDepthAttachment = &pass->depthAttachment;
    renInf.pStencilAttachment = &pass->stencilAttachment;

    vkCmdBeginRendering(cBuf, &renInf);

    pass->callBack(*pass, cBuf);

    vkCmdEndRendering(cBuf);
}

bool resEq(Resource rhs, Resource lhs)
{
    return rhs.value.buffer.buffer == lhs.value.buffer.buffer &&
           rhs.value.img.imgview == lhs.value.img.imgview &&
           rhs.type == lhs.type &&
           rhs.usage == lhs.usage;
}

bool resEqWoUsage(Resource rhs, Resource lhs)
{
    return rhs.type == lhs.type &&
           rhs.value.buffer.index == lhs.value.buffer.index &&
           rhs.value.img.image == lhs.value.img.image;
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
    if (pass->resourceCount == 0)
    {
        pass->resources = malloc(sizeof(Resource));
        pass->resources[0] = res;
        pass->resourceCount += 1;
        return;
    }
    pass->resources = realloc(pass->resources, sizeof(Resource) * (pass->resourceCount + 1));
    pass->resources[pass->resourceCount] = res;
    pass->resourceCount += 1;
}

void addCatt(RenderPass *pass, VkRenderingAttachmentInfo att)
{
    if (pass->cAttCount == 0)
    {
        pass->colorAttachments = malloc(sizeof(Resource));
        pass->colorAttachments[0] = att;
        pass->cAttCount += 1;
        return;
    }
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
void addColorAttachment(Image img, RenderPass *pass, VkClearValue *clear)
{
    VkRenderingAttachmentInfo rAttInfo = {0};
    rAttInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    rAttInfo.pNext = NULL;

    rAttInfo.imageView = img.imgview;
    rAttInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rAttInfo.loadOp = clear != NULL ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    rAttInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    if (clear)
        rAttInfo.clearValue = *clear;
    addCatt(pass, rAttInfo);

    Resource res;

    res.type = RES_TYPE_Image;

    res.access = ACCESS_COLORATTACHMENT;
    res.usage = USAGE_COLORATTACHMENT;

    res.value.img = img;
    res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    addResource(pass, res);
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

    Resource res;

    res.type = RES_TYPE_Image;

    res.access = ACCESS_DEPTHATTACHMENT;
    res.usage = USAGE_DEPTHSTENCILATTACHMENT;

    res.value.img = img;
    res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    addResource(pass, res);
}
void addImageResource(RenderPass *pass, Image image, ResourceUsageFlags_t usage)
{
    Resource res = {0};
    res.type = RES_TYPE_Image;
    res.value.img = image;
    res.usage = usage;

    switch (res.usage)
    {
    case USAGE_COLORATTACHMENT:
        res.access = ACCESS_COLORATTACHMENT;
        res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        return;
        break;
    case USAGE_DEPTHSTENCILATTACHMENT:
        res.access = ACCESS_DEPTHATTACHMENT;
        res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        return;
        break;
    case USAGE_TRANSFER_SRC:
        res.access = ACCESS_TRANSFER_READ;
        res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        break;
    case USAGE_TRANSFER_DST:
        res.access = ACCESS_TRANSFER_WRITE;
        res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        break;
    case USAGE_SAMPLED:
        res.access = ACCESS_READ;
        res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
    case USAGE_UNDEFINED:
        res.access = 0;
        res.value.img.CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        break;
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
    if (builder->passCount == 0)
    {
        builder->passes = malloc(sizeof(RenderPass) * (builder->passCount + 1));
        builder->passes[builder->passCount] = *pass;
        builder->passCount += 1;
        return;
    }
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

    int EdgeResourceCount = 0;
    Resource *edgeResources = malloc(sizeof(Resource));

    int imgBrrCount = 0;
    VkImageMemoryBarrier *imgMemBarriers = malloc(sizeof(VkImageMemoryBarrier));

    int bufBrrCount = 0;
    VkBufferMemoryBarrier *bufMemBarriers = malloc(sizeof(VkBufferMemoryBarrier));

    for (int i = graph->passCount - 1; i >= 0; i--)
    {
        RenderPass curPass = graph->passes[i];
        for (int r = 0; r <= curPass.resourceCount - 1; r++)
        {
            Resource cr = curPass.resources[r];
            if ((cr.access & ACCESS_TRANSFER_WRITE) != 0 && cr.value.img.imgview == swapChainImg.imgview)
            {
                rootResources = realloc(rootResources, sizeof(Resource) * (rootResourceCount + curPass.resourceCount));
                memcpy(rootResources + rootResourceCount, curPass.resources, sizeof(Resource) * (curPass.resourceCount));
                rootResourceCount += curPass.resourceCount;

                newPasses[graph->passCount - newPassCount - 1] = curPass;
                newPassCount += 1;
                break;
            }
            else if ((cr.usage & USAGE_COLORATTACHMENT) != 0 || (cr.usage & USAGE_TRANSFER_DST) != 0 || (cr.usage & USAGE_DEPTHSTENCILATTACHMENT) != 0)
            {
                for (int e = 0; e < rootResourceCount; e++)
                {
                    if (resEqWoUsage(cr, rootResources[e]))
                    {
                        rootResources = realloc(rootResources, sizeof(Resource) * (rootResourceCount + curPass.resourceCount));
                        memcpy(rootResources + rootResourceCount, curPass.resources, sizeof(Resource) * (curPass.resourceCount));
                        rootResourceCount += curPass.resourceCount;

                        newPasses[graph->passCount - newPassCount - 1] = curPass;
                        newPassCount += 1;

                        r = curPass.resourceCount + 1;
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < graph->passCount; i++)
    {
        for (int r = 0; r < graph->passes[i].resourceCount; r++)
        {
            Resource cr = graph->passes[i].resources[r];
            for (int e = 0; e < EdgeResourceCount; e++)
            {
                if (resEqWoUsage(cr, edgeResources[e]))
                {
                    // flush out edge resources and re-malloc it
                    r = graph->passes[i].resourceCount + 1; // get out of outer loop
                    break;
                }
            }
            // create barriers & add to edgeResources
        }
    }

    free(graph->passes);
    graph->passes = &newPasses[graph->passCount] - newPassCount;
    graph->passCount = newPassCount;

    free(rootResources);
}

RenderGraph buildGraph(GraphBuilder *builder, Image scImage)
{
    RenderGraph rg = {0};
    rg.passes = malloc(sizeof(RenderPass) * builder->passCount);
    memcpy(rg.passes, builder->passes, sizeof(RenderPass) * builder->passCount);
    rg.passCount = builder->passCount;
    free(builder->passes);
    optimizePasses(&rg, scImage);
    return rg;
}

void executeGraph(RenderGraph *graph, VkCommandBuffer cBuf)
{
    for (int i = 0; i < graph->passCount; i++)
    {
        recordPass(&graph->passes[i], cBuf);
    }
}

void destroyRenderGraph(RenderGraph *graph)
{
    for (int i = 0; i < graph->passCount - 1; i++)
    {
        free(graph->passes[i].colorAttachments);
        free(graph->passes[i].resources);
    }
    // free(graph->passes);
}