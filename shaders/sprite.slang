static const int scaleFactor = 10;

struct Transform
{
    float3 position;
    float2 scale;
    float rotation;
}

struct Light
{
    float3 position;
    float4 color;
}

struct pushConstants
{
    uint64_t *ids;
    Transform *instances;
    Light *lights;
}

[vk_push_constant]
const pushConstants ptrs;

[vk_binding(0)]
Sampler2D textures[];

struct VertOutput
{
    float3 color : fragColor;
    float2 uv : vertUVCOORDS;
    float3 normal : normals;
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

static const float3 normals[4] = {
    float3(0, 1, 1),
    float3(1, 0, 1),
    float3(0, -1, 1),
    float3(-1, 0, 1),

};

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

float3 transformNormals(Transform vert, uint32_t id)
{
    float3 rPos = (float3(normals[id / 2].xyz) + vert.position) / scaleFactor;
    rPos = rot(rPos, vert.position, vert.rotation);
    rPos.x *= vert.scale.x;
    rPos.y *= vert.scale.y;

    return rPos;
}

[shader("vertex")]
VertOutput vertMain(uint32_t vertID: SV_VertexID, uint32_t instID: SV_InstanceID)
{
    VertOutput output;

    output.sv_pos = float4(transform(ptrs.instances[instID], vertID), 1.0f);
    output.normal = transformNormals(ptrs.instances[instID], vertID);
    output.color = float3(1.0f, 1.0f, 1.0f);
    output.texID = ptrs.ids[instID];
    output.uv = uvs[vertID];

    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    Light light = ptrs.lights[0];

    float4 albedo = textures[NonUniformResourceIndex(int(input.texID))].Sample(input.uv) * float4(input.color, 1.0);

    float3 ambient = light.color.xyz * 0.1f;

    float3 lightDir = light.position - float3(input.sv_pos.xyz);
    float atten = float3(1.0 / pow(lightDir, 2)).z;
    float3 lightColor = light.color.xyz * light.color.w * atten;
    float3 normal = normalize(input.normal);
    float3 diffuse = lightColor * max(dot(normal, normalize(lightDir)), 0.0f);

    float4 color = float4((ambient + diffuse), 1.0f) * albedo;

    // float4 color = float4(input.normal, 1.0f);

    return color;
}

