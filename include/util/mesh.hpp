#include <WREmath.h>
#include <renderer.h>

#ifndef WRE_MESH_H__
#define WRE_MESH_H__
typedef struct
{
    vec3 pos;
    vec4 color;
    vec2 uv;
} WREVertex3D;

typedef struct
{
    Texture AlbedoMap, NormalMap;
} WREMaterial3D;

typedef struct
{
    uint32_t id;
    uint32_t indexCount;
    WREVertex3D *vertices;
    uint32_t *indices;
    Buffer vBuf;
    Buffer iBuf;

    WREMaterial3D material;
} WREmesh;

typedef struct
{
    mat4x4 transform;
} WREMeshInstance;

typedef struct
{
    WREmesh mesh;
    uint32_t instanceCount;
    WREMeshInstance *instances;
} WREMeshGroup;

typedef struct
{
    graphicsPipeline gbufferPipeline;
    computePipeline lightingPipeline;

    uint32_t maxMeshGroupCount;
    uint32_t meshGroupCount;
    uint32_t totalInstanceCount;
    WREMeshGroup *MeshGroups;
} WREScene3D;

WREScene3D loadSceneGLTF(char *filepath, renderer_t *renderer);
void meshPass(WREScene3D *scene, renderer_t *renderer);
void createMeshInstance(WREmesh *mesh, WREScene3D *scene, renderer_t *renderer, vec3 position, vec3 rot, vec3 scale);
void initializeScene3D(WREScene3D *scene, renderer_t *renderer);
#endif