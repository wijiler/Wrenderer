struct Transform
{
    float3 Origin;
    float3 Scale;
    float4 rotation;
}
struct Vertex
{
    float3 pos;
    float3 color;
    float2 uv;
}
struct MeshInstance
{
    Transform Transf;
}

struct Mesh
{
    MeshInstance *instances;
    Vertex *vertices;
    uint64_t textureId;
}

[vk_binding(0)]
Sampler2D textures[];

[vk_push_constant]
Mesh *meshes;

struct VertexShaderOutput
{
    float4 pos : SV_Position;
    float3 color : outColor;
    float2 uv : outUV;
}

[shader("vertex")]
VertexShaderOutput vertMain()
{
    VertexShaderOutput output;
    // something or something
    return output;
}

[shader("fragment")]
float4 fragMain()
    : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
