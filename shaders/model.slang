struct Transform
{
    float3 Origin;
    float3 Scale;
    float4 rotation;
}
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
struct Mesh
{
    MeshInstance *instances;
    Vertex *vertices;
    Material mat;
}

[vk_binding(0, 0)]
Sampler2D textures[];

[vk_binding(0, 1)]
Sampler2D normals[];

[vk_push_constant]
Mesh mesh;

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

    output.pos = mul(mesh.instances[instId].transformation, float4(mesh.vertices[vertId].pos, 1));
    output.color = mesh.vertices[vertId].color;
    output.uv = mesh.vertices[vertId].uv;
    output.mat = mesh.mat;

    return output;
}

struct fragOutput
{
    float4 albedoBuffer : SV_Target0;
    float4 normalBuffer : SV_Target1;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
