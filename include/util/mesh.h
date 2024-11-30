#include <renderer.h>
#include <util/math.h>
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
    WREVertex3D *vertices;
    uint32_t *indices;
    Buffer vBuf;
    Buffer iBuf;

    WREMaterial3D material;
    int instanceCount;
    mat4x4 *transformations;
} WREmesh;

typedef struct
{
    WREmesh *parent;
    transform3D transform;
} WREmeshInstance;

typedef struct
{
    graphicsPipeline gbufferPass;
    graphicsPipeline lightingPass;

    uint32_t instanceCount;
    WREmeshInstance *instances;
} WREScene3D;

WREmesh loadMeshFromGLTF(char *filepath, renderer_t *renderer);
#endif