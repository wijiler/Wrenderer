#include <renderer.h>

void submitMesh(Mesh mesh, MeshHandler *handler)
{
    if (mesh.instanceCount > 1)
    {
        handler->instancedMeshes = realloc(handler->instancedMeshes, sizeof(mesh) * (handler->instancedmeshCount + 1));
        handler->instancedMeshes[handler->instancedmeshCount] = mesh;
        handler->instancedmeshCount += 1;
        return;
    }
}