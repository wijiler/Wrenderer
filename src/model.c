#include <renderer.h>
// This should probably be moved to engine to avoid generalization limitations of the renderer
Mesh createMesh(renderer_t renderer, uint32_t vertCount, void *vertices, uint32_t indexCount, uint32_t indices[], uint32_t instanceCount)
{
    Mesh mesh = {0};
    mesh.instanceCount = instanceCount;

    uint32_t *newIndices = indices;
    for (uint32_t i = 0; i < indexCount; i++)
    {
        newIndices[i] += renderer.meshHandler.unifiedVertexSize / renderer.meshHandler.vertexSize;
    }

    BufferCreateInfo bCI = {0};
    bCI.access = DEVICE_ONLY;
    bCI.dataSize = vertCount * renderer.meshHandler.vertexSize;
    bCI.usage = BUFFER_USAGE_TRANSFER_SRC | BUFFER_USAGE_TRANSFER_DST;

    Buffer stagingBuf = {0};
    createBuffer(renderer.vkCore, bCI, &mesh.verticies);
    bCI.access = CPU_ONLY;
    bCI.dataSize += indexCount * sizeof(uint32_t);
    createBuffer(renderer.vkCore, bCI, &stagingBuf);
    bCI.access = DEVICE_ONLY;
    bCI.dataSize = indexCount * sizeof(uint32_t);
    createBuffer(renderer.vkCore, bCI, &mesh.indices);

    pushDataToBuffer(vertices, vertCount * renderer.meshHandler.vertexSize, stagingBuf, 0);
    copyBuf(renderer.vkCore, stagingBuf, mesh.verticies, vertCount * renderer.meshHandler.vertexSize, 0, 0);
    pushDataToBuffer(indices, indexCount * sizeof(uint32_t), stagingBuf, vertCount * renderer.meshHandler.vertexSize);
    copyBuf(renderer.vkCore, stagingBuf, mesh.indices, indexCount * sizeof(uint32_t), vertCount * renderer.meshHandler.vertexSize, 0);

    destroyBuffer(stagingBuf, renderer.vkCore);

    return mesh;
}

void submitMesh(Mesh mesh, renderer_t *renderer)
{
    if (mesh.instanceCount > 1)
    {
        renderer->meshHandler.instancedMeshes = realloc(renderer->meshHandler.instancedMeshes, sizeof(mesh) * (renderer->meshHandler.instancedmeshCount + 1));
        renderer->meshHandler.instancedMeshes[renderer->meshHandler.instancedmeshCount] = mesh;
        renderer->meshHandler.instancedmeshCount += 1;
        return;
    }
    if (renderer->meshHandler.unifiedVertexCapacity - mesh.verticies.size <= 0)
    {
        int growAmm = -(renderer->meshHandler.unifiedVertexCapacity - mesh.verticies.size) + 1000000;
        renderer->meshHandler.unifiedVertexCapacity = growAmm;
        Buffer buf = {0};
        BufferCreateInfo bCI = {0};
        bCI.dataSize = renderer->meshHandler.unifiedVertexSize + growAmm;
        bCI.access = DEVICE_ONLY;
        bCI.usage = BUFFER_USAGE_VERTEX | BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_TRANSFER_SRC;
        createBuffer(renderer->vkCore, bCI, &buf);
        copyBuf(renderer->vkCore, renderer->meshHandler.unifiedVerts, buf, renderer->meshHandler.unifiedVerts.size, 0, 0);
        destroyBuffer(renderer->meshHandler.unifiedVerts, renderer->vkCore);
        renderer->meshHandler.unifiedVerts = buf;
    }
    else if (renderer->meshHandler.unifiedIndexCapacity - mesh.indices.size <= 0)
    {
        int growAmm = -(renderer->meshHandler.unifiedIndexCapacity - mesh.indices.size) + 1000000;
        renderer->meshHandler.unifiedIndexCapacity = growAmm;
        Buffer buf = {0};
        BufferCreateInfo bCI = {0};
        bCI.dataSize = renderer->meshHandler.unifiedIndexSize + growAmm;
        bCI.access = DEVICE_ONLY;
        bCI.usage = BUFFER_USAGE_INDEX | BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_TRANSFER_SRC;
        createBuffer(renderer->vkCore, bCI, &buf);
        copyBuf(renderer->vkCore, renderer->meshHandler.unifiedIndices, buf, renderer->meshHandler.unifiedIndices.size, 0, 0);
        destroyBuffer(renderer->meshHandler.unifiedIndices, renderer->vkCore);
        renderer->meshHandler.unifiedIndices = buf;
    }
    else
    {
        renderer->meshHandler.unifiedIndexCapacity -= mesh.indices.size;
        renderer->meshHandler.unifiedVertexCapacity -= mesh.verticies.size;
    }
    copyBuf(renderer->vkCore, mesh.verticies, renderer->meshHandler.unifiedVerts, mesh.verticies.size, 0, renderer->meshHandler.unifiedVertexSize);
    renderer->meshHandler.unifiedVertexSize += mesh.verticies.size;
    renderer->meshHandler.unifiedVertexCapacity -= mesh.verticies.size;
    copyBuf(renderer->vkCore, mesh.indices, renderer->meshHandler.unifiedIndices, mesh.indices.size, 0, renderer->meshHandler.unifiedIndexSize);
    renderer->meshHandler.unifiedIndexSize += mesh.indices.size;
    renderer->meshHandler.unifiedIndexCapacity -= mesh.indices.size;
    destroyBuffer(mesh.verticies, renderer->vkCore);
    destroyBuffer(mesh.indices, renderer->vkCore);
}
typedef struct
{
    VkDeviceAddress address;
} pushConstants;
const uint64_t offSet = 0;
void sceneDrawCallBack(RenderPass pass, VkCommandBuffer cBuf)
{
    int indexCount = *((int *)pass.resources[3].value.arbitrary);
    bindGraphicsPipeline(pass.gPl, cBuf);
    pushConstants uConstants = {pass.resources[1].value.buffer.gpuAddress};
    vkCmdPushConstants(cBuf, pass.gPl.plLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, pass.gPl.pcRange.size, &uConstants);

    vkCmdBindIndexBuffer(cBuf, pass.resources[2].value.buffer.buffer, offSet, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(cBuf, indexCount / sizeof(uint32_t), 1, 0, 0, 0);

    for (int i = 4; i < pass.resourceCount; i += 3)
    {
        pushConstants constants = {pass.resources[i].value.buffer.gpuAddress};
        vkCmdPushConstants(cBuf, pass.gPl.plLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, pass.gPl.pcRange.size, &constants);
        vkCmdBindIndexBuffer(cBuf, pass.resources[i + 1].value.buffer.buffer, offSet, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cBuf, pass.resources[i + 2].value.buffer.size / sizeof(uint32_t), *((uint32_t *)pass.resources[i].value.arbitrary), 0, 0, 0);
    }
}

RenderPass sceneDraw(renderer_t *renderer)
{
    RenderPass pass = newPass("SceneDraw", PASS_TYPE_GRAPHICS);

    addImageResource(&pass, renderer->vkCore.currentScImg, USAGE_COLORATTACHMENT);
    addBufferResource(&pass, renderer->meshHandler.unifiedVerts, USAGE_UNDEFINED);
    addBufferResource(&pass, renderer->meshHandler.unifiedIndices, USAGE_UNDEFINED);
    addArbitraryResource(&pass, &renderer->meshHandler.unifiedIndexSize);

    for (uint32_t i = 0; i < renderer->meshHandler.instancedmeshCount; i++)
    {
        addBufferResource(&pass, renderer->meshHandler.instancedMeshes[i].verticies, USAGE_UNDEFINED);
        addBufferResource(&pass, renderer->meshHandler.instancedMeshes[i].indices, USAGE_UNDEFINED);
        addArbitraryResource(&pass, &renderer->meshHandler.instancedMeshes[i].instanceCount);
    }

    setExecutionCallBack(&pass, sceneDrawCallBack);
    return pass;
}
