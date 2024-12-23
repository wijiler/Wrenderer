import constants;
struct Vertex
{
    float3 pos;
    float4 color;
    float2 uv;
}
struct MeshInstance
{
    float4x4 transformation;
}
struct Material
{
    uint32_t textureId;
    uint32_t normalId;
}

enum WRECAMERATYPE
{
    WRE_ORTHOGRAPHIC_CAM,
    WRE_PERSPECTIVE_CAM,
}

struct cameraTransform
{
    float3 pos;
    float2 rotation;
}

struct Camera
{
    WRECAMERATYPE type;
    float4x4 perspective;
    float4x4 view;
    cameraTransform position;
    float fov;
}
struct Mesh
{
    Vertex *vBuf;
    uint32_t *iBuf;

    Material material;
}

struct pushConstants
{
    MeshInstance *instances;
    Mesh *meshes;
    Camera *activeCam;
}

[vk_binding(0, 0)]
Sampler2D textures[];
[vk_binding(0, 1)]
Sampler2D normals[];

[vk_push_constant]
pushConstants pc;

struct VertOutput
{
    float4 pos : SV_Position;
    float4 color : outColor;
    float2 uv : outUV;
    Material mat : meshMaterialId;
}

[shader("vertex")]
VertOutput vertMain(uint vertId: SV_VertexID, uint instId: SV_InstanceID)
{
    VertOutput output;
    uint32_t index = pc.meshes[0].iBuf[vertId];
    Mesh mesh = pc.meshes[0];

    output.pos = pc.activeCam.perspective * pc.activeCam.view * pc.instances[instId].transformation * float4(mesh.vBuf[index].pos, 1.0f);
    output.color = float4(1, 1, 1, 1);
    output.uv = mesh.vBuf[index].uv;
    output.mat = mesh.material;

    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    return textures[input.mat.textureId].Sample(input.uv) * input.color;
}
