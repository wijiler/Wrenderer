#include <renderer.h>

void submitMesh(Mesh mesh, MeshHandler *handler, renderer_t renderer)
{
    if (mesh.instanceCount > 1)
    {
        handler->instancedMeshes = realloc(handler->instancedMeshes, sizeof(mesh) * (handler->instancedmeshCount + 1));
        handler->instancedMeshes[handler->instancedmeshCount] = mesh;
        handler->instancedmeshCount += 1;
        return;
    }
    copyBuf(renderer.vkCore, mesh.verticies, handler->unifiedVerts, mesh.verticies.size, 0, handler->unifiedSize);
}