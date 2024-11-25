import constants;

struct Light
{
    float3 position;
    float4 color;
    float radius;
    bool on;
    uint id;
}
///
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

static const float4 vpositions[6] =
    {
        float4(.1f, .1f, 0.0f, 1.0f),
        float4(.1f, -.1f, 0.0f, 1.0f),
        float4(-.1f, -.1f, 0.0f, 1.0f),
        float4(-.1f, -.1f, 0.0f, 1.0f),
        float4(-.1f, .1f, 0.0f, 1.0f),
        float4(.1f, .1f, 0.0f, 1.0f),
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
///
struct pushConstants
{
    uint lightCount;
    Light *lights;
    Transform *instances;
    Camera *camera;
}

struct VertOutput
{
    float4 pos : SV_Position;
    float3 realPos : REAL_POS;
}

[vk_binding(0, 0)]
Texture2D<float4> gBuffer[];

static const Texture2D<float4> albedo = gBuffer[0];

[vk_push_constant]
pushConstants pc;
[shader("vertex")]
VertOutput vertMain(uint vertID: SV_VertexID, uint instID: SV_InstanceID)
{
    VertOutput output;
    float4 pos = mul(pc.camera.perspective, mul(pc.camera.view, float4(transform(pc.instances[instID], vertID), 1.0f)));
    output.pos = pos;
    output.realPos = float3(pos.x, pos.y, pos.z);
    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    float4 color = albedo[uint2(input.pos.xy)];
    float3 diffuse = float3(0, 0, 0);
    for (uint i = 0; i < pc.lightCount; i++)
    {
        Light light = pc.lights[i];
        if (!light.on)
            continue;
        float3 lightPos = light.position / spriteScaleFactor;
        float3 ambient = light.color.xyz * 0.1f;

        float3 lightDir = lightPos - input.realPos;
        float lightDist = length(lightDir);
        float atten = (1.0 / (lightDist * lightDist)) - 1 / light.radius;
        float3 lightColor = light.color.xyz * (light.color.w * atten);
        diffuse = lightColor;
    }
    return float4(diffuse, 1.0f) * color;
}
