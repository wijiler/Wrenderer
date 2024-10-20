#include <renderer.h>
#include <stdbool.h>
Mesh createMesh(renderer_t renderer, uint32_t vertCount, void *vertices, uint32_t indexCount, uint32_t indices[], uint32_t instanceCount, size_t vertexSize)
{
    Mesh mesh = {0};
    mesh.instanceCount = instanceCount;

    BufferCreateInfo bCI = {0};
    bCI.access = DEVICE_ONLY;
    bCI.dataSize = vertCount * vertexSize;
    bCI.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;

    Buffer stagingBuf = {0};
    bCI.usage |= BUFFER_USAGE_VERTEX;
    createBuffer(renderer.vkCore, bCI, &mesh.verticies);
    bCI.access = CPU_ONLY;
    bCI.dataSize += indexCount * sizeof(uint32_t);
    bCI.usage &= ~BUFFER_USAGE_VERTEX;
    createBuffer(renderer.vkCore, bCI, &stagingBuf);
    bCI.access = DEVICE_ONLY;
    bCI.dataSize = indexCount * sizeof(uint32_t);
    bCI.usage |= BUFFER_USAGE_INDEX;
    createBuffer(renderer.vkCore, bCI, &mesh.indices);

    pushDataToBuffer(vertices, vertCount * vertexSize, stagingBuf, 0);
    copyBuf(renderer.vkCore, stagingBuf, mesh.verticies, vertCount * vertexSize, 0, 0);
    pushDataToBuffer(indices, indexCount * sizeof(uint32_t), stagingBuf, vertCount * vertexSize);
    copyBuf(renderer.vkCore, stagingBuf, mesh.indices, indexCount * sizeof(uint32_t), vertCount * vertexSize, 0);

    destroyBuffer(stagingBuf, renderer.vkCore);

    return mesh;
}

void submitMesh(Mesh mesh, MeshHandler *handler)
{
    handler->instancedMeshes = realloc(handler->instancedMeshes, sizeof(mesh) * (handler->instancedmeshCount + 1));
    handler->instancedMeshes[handler->instancedmeshCount] = mesh;
    handler->instancedmeshCount += 1;
    return;
}

void removeMesh(Mesh mesh, MeshHandler *handler, renderer_t renderer)
{
    bool destroyed = false;
    for (uint32_t i = 0; i < handler->instancedmeshCount; i++)
    {
        if (handler->instancedMeshes[i].verticies.gpuAddress == mesh.verticies.gpuAddress)
        {
            destroyBuffer(mesh.verticies, renderer.vkCore);
            destroyBuffer(mesh.indices, renderer.vkCore);
            destroyed = true;
        }
        if (destroyed)
        {
            if (i == handler->instancedmeshCount - 1)
                break;
            handler->instancedMeshes[i] = handler->instancedMeshes[i + 1];
        }
    }
    handler->instancedmeshCount -= 1;
    handler->instancedMeshes = realloc(handler->instancedMeshes, sizeof(Mesh) * handler->instancedmeshCount);
}

typedef struct
{
    VkDeviceAddress address;
} pushConstants;
const uint64_t offSet = 0;
void sceneDrawCallBack(RenderPass pass, VkCommandBuffer cBuf)
{
    bindGraphicsPipeline(pass.gPl, cBuf);

    for (int i = 1; i < pass.resourceCount; i += 3)
    {
        pushConstants constants = {pass.resources[i].value.buffer.gpuAddress};
        vkCmdPushConstants(cBuf, pass.gPl.plLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, pass.gPl.pcRange.size, &constants);
        vkCmdBindIndexBuffer(cBuf, pass.resources[i + 1].value.buffer.buffer, offSet, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cBuf, pass.resources[i + 1].value.buffer.size / sizeof(uint32_t), *((uint32_t *)pass.resources[i + 2].value.arbitrary), 0, 0, 0);
    }
}

RenderPass sceneDraw(renderer_t *renderer, MeshHandler *handler, char *name)
{
    RenderPass pass = newPass(name, PASS_TYPE_GRAPHICS);
    addImageResource(&pass, renderer->vkCore.currentScImg, USAGE_COLORATTACHMENT);
    for (uint32_t i = 0; i < handler->instancedmeshCount; i++)
    {
        addBufferResource(&pass, handler->instancedMeshes[i].verticies, USAGE_UNDEFINED);
        addBufferResource(&pass, handler->instancedMeshes[i].indices, USAGE_UNDEFINED);
        addArbitraryResource(&pass, &handler->instancedMeshes[i].instanceCount);
    }

    setExecutionCallBack(&pass, sceneDrawCallBack);
    return pass;
}
