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

struct pushConstants
{
    uint64_t *ids;
    Transform *instances;
    Camera *camera;
}

[vk_push_constant]
const pushConstants ptrs;
[vk_binding(0)]
Sampler2D textures[];

struct VertOutput
{
    float3 color : fragColor;
    float2 uv : vertUVCOORDS;
    uint64_t texID : meshTextureID;
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
    output.texID = ptrs.ids[instID];
    output.uv = uvs[vertID];

    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    float4 albedo = textures[NonUniformResourceIndex(int(input.texID))].Sample(input.uv) * float4(input.color, 1.0);

    // float4 color = float4(input.normal, 1.0f);

    return albedo;
}

