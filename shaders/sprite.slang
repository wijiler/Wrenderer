import constants;

struct Transform
{
    float3 position;
    float2 scale;
    float rotation;
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

struct MaterialId
{
    uint32_t texture;
    uint32_t normal;
}

struct pushConstants
{
    MaterialId *ids;
    Transform *instances;
    Camera *camera;
}

[vk_push_constant]
const pushConstants ptrs;
[vk_binding(0, 0)]
Sampler2D textures[];
[vk_binding(0, 1)]
Sampler2D normals[];

struct VertOutput
{
    float3 color : fragColor;
    float2 uv : vertUVCOORDS;
    MaterialId matID : spriteTextureID;
    float4 sv_pos : SV_Position;
}

static const float2 uvs[6] =
    {
        float2(1.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(0.0f, 1.0f),
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 0.0f),
    };

float3 rot(float3 p, float3 o, float a)
{
    return float3((p.x - o.x) * cos(a) - (p.y - o.y) * sin(a) + o.x, (p.x - o.x) * sin(a) + (p.y - o.y) * cos(a) + o.y, p.z);
}

float3 scale(float3 p, float3 o, float sf)
{
    return float3(o.x + (p.x / sf), o.y + (p.y / sf), p.z);
}

float3 transform(Transform vert, uint32_t id)
{
    float3 rPos = (float3(spriteVPositions[id].xyz) + (vert.position / spriteScaleFactor));
    rPos = rot(rPos, vert.position, vert.rotation);
    rPos.x *= vert.scale.x;
    rPos.y *= vert.scale.y;

    return rPos;
}

[shader("vertex")]
VertOutput vertMain(uint32_t vertID: SV_VertexID, uint32_t instID: SV_InstanceID)
{
    VertOutput output;

    output.sv_pos = mul(ptrs.camera.perspective, mul(ptrs.camera.view, float4(transform(ptrs.instances[instID], vertID), 1.0f)));
    output.color = float3(1.0f, 1.0f, 1.0f);
    output.matID = ptrs.ids[instID];
    output.uv = uvs[vertID];

    return output;
}

struct FragOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
}

[shader("fragment")]
FragOutput fragMain(VertOutput input)
{
    FragOutput output;
    uint2 fragpos = uint2((uint)input.sv_pos.x, (uint)input.sv_pos.y);

    output.albedo = textures[NonUniformResourceIndex(int(input.matID.texture))].Sample(input.uv) * float4(input.color, 1.0);
    output.normal = normals[NonUniformResourceIndex(int(input.matID.normal))].Sample(input.uv);

    // float4 color = float4(input.normal, 1.0f);

    return output;
}

