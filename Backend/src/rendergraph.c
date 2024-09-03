#include <rendergraph.h>
#include <stdbool.h>

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
const VkRenderPassBeginInfo bInfo = {0};

void recordPass(RenderPass *pass, VkCommandBuffer cBuf)
{
    VkRenderingInfo renInf = {0};
    renInf.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renInf.pNext = NULL;

    renInf.colorAttachmentCount = pass->colorAttCount;
    renInf.pColorAttachments = pass->ColorAttachments;
    renInf.pDepthAttachment = &pass->depthAttachment;
    renInf.pStencilAttachment = &pass->stencilAttachment;

    vkCmdBeginRendering(cBuf, &renInf);

    pass->callBack(cBuf, *pass);

    vkCmdEndRendering(cBuf);
}

bool resEq(Resource rhs, Resource lhs)
{
    return rhs.BufferIndex == lhs.BufferIndex &&
           rhs.flags == lhs.flags &&
           rhs.image == lhs.image &&
           rhs.type == lhs.type &&
           rhs.usage == lhs.usage;
}

Resource *getNodeDependencies(int *depCount, RenderPass pass)
{
    Resource *deps = malloc(sizeof(Resource) * pass.resourceCount);
    int count = 0;

    for (int i = 0; i < pass.resourceCount; i++)
    {
        if ((pass.resources[i].usage & READ) != 0)
        {
            deps[i] = pass.resources[i];
            count += 1;
        }
    }
    deps = realloc(deps, sizeof(Resource) * count); // shrink ideally
    *depCount = count;
    return deps;
}

Resource *getNodeWrites(int *writeCount, RenderPass pass)
{
    Resource *writes = malloc(sizeof(Resource) * pass.resourceCount);
    int count = 0;

    for (int i = 0; i < pass.resourceCount; i++)
    {
        if ((pass.resources[i].usage & WRITE) != 0)
        {
            writes[i] = pass.resources[i];
            count += 1;
        }
    }
    writes = realloc(writes, sizeof(Resource) * count); // shrink ideally
    *writeCount = count;
    return writes;
}

// Optimizes passes to trim unused ones & places memory barriers
void optimizePasses(RenderGraph *graph, VkImageView *swapChainImageViews, int Index)
{
    int passCount = 0;
    int rootpassCount = 0;
    RenderPass *passes = malloc(sizeof(RenderPass) * graph->passCount);
    int *rootIndices = malloc(sizeof(int) * graph->passCount); // any pass that has a WRITE handle to a swaphchain image
    for (int i = 0; i < graph->passCount; i++)
    {
        RenderPass pass = graph->passes[i];

        for (int j = 0; j < pass.resourceCount; j++)
        {
            if (pass.resources[j].image == swapChainImageViews[Index] && (pass.resources[j].usage & WRITE) != 0)
            {
                rootIndices[rootpassCount] = i;
                rootpassCount += 1;
            }
        }
    }
    // we go back up from the last root pass, find every single pass it reads from, and every single pass that writes to one of those passes those are the ones added
    int rootDepCount = 0;
    Resource *rootDeps = getNodeDependencies(&rootDepCount, graph->passes[rootIndices[rootpassCount]]);
    int currentRoot = rootpassCount;
    for (int i = rootIndices[rootpassCount]; i >= 0; i--)
    {
        if (i == rootIndices[currentRoot - 1])
        {
            currentRoot = rootIndices[currentRoot - 1];

            rootDeps = getNodeDependencies(&rootDepCount, graph->passes[i]);
            passes[i] = graph->passes[i];
            passCount += 1;
        }
        int writeCount = 0;
        Resource *writes = getNodeWrites(&writeCount, graph->passes[i]);
        for (int j = 0; j < writeCount; j++)
        {
            for (int r = 0; r < rootDepCount; r++)
            {
                if (resEq(writes[j], rootDeps[r]))
                {
                    passes[i] = graph->passes[i];
                    passCount += 1;
                }
            }
        }
    }
    free(graph->passes);
    passes = realloc(passes, sizeof(RenderPass) * passCount);
    graph->passCount = passCount;
    graph->passes = passes;
}

void recordGraph(RenderGraph *graph, VkCommandBuffer cBuf)
{
    for (int i = 0; i < graph->passCount; i++)
    {
        recordPass(&graph->passes[i], cBuf);
    }
}