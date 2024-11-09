static const int scaleFactor = 10;

struct Light
{
    float3 position;
    float4 color;
    float radius;
}
///
struct Transform
{
    float3 position;
    float2 scale;
    float rotation;
}
static const float4 vpositions[6] =
    {
        float4(1.0f, 1.0f, 0.0f, 1.0f),
        float4(1.0f, -1.0f, 0.0f, 1.0f),
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
        float4(-1.0f, 1.0f, 0.0f, 1.0f),
        float4(1.0f, 1.0f, 0.0f, 1.0f),
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
    float3 rPos = (float3(vpositions[id].xyz) + vert.position) / scaleFactor;
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
}

struct VertOutput
{
    float4 pos : SV_Position;
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
    output.pos = float4(transform(pc.instances[instID], vertID), 1.0f);
    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input, uint2 fragPos: SV_Position)
    : SV_Target
{
    float4 color = albedo[fragPos];
    float3 diffuse = float3(0, 0, 0);
    for (uint i = 0; i < pc.lightCount; i++)
    {
        Light light = pc.lights[i];
        float3 ambient = light.color.xyz * 0.1f;

        float3 lightDir = light.position - input.pos.xyz;
        float lightDist = length(lightDir);
        float atten = 1.0 / (lightDist * lightDist);
        float3 lightColor = light.color.xyz * (light.color.w * atten);
        float3 diffuse = lightColor;

        diffuse += ambient + diffuse;
    }
    color = float4(diffuse, 1.0f) * color;
    return color;
}
