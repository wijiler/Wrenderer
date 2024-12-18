import constants;

struct Light
{
    float3 position;
    float4 color;
    float radius;
    bool on;
    uint id;
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
    uint32_t lightCount;
    Light *lights;
    Mesh *meshes;
    MeshInstance *instances;
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
static const Texture2D<float4> normal = gBuffer[1];

[vk_push_constant]
pushConstants pc;
[shader("vertex")]
VertOutput vertMain(uint vertID: SV_VertexID, uint instID: SV_InstanceID)
{
    MeshInstance instance = pc.instances[instID];
    VertOutput output;
    float4 pos = pc.camera.perspective * pc.camera.view * instance.transformation * float4(pc.meshes[instance.], 1.0);
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
        float3 lightColor = light.color.xyz * (light.color.w * atten + (max(dot(float3(normal[uint2(input.pos.xy)].xyz), lightDir), 0)));
        diffuse = lightColor;
    }
    return float4(diffuse, 1.0f) * color;
}
