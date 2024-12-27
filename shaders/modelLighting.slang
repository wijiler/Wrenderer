import constants;

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
    float4x4 invPerspective;
    float4x4 invView;
}

struct Light
{
    float radius;
    float4 color;
    float3 position;
}

struct PushConstants
{
    Camera *cam;
    uint lightCount;
    Light *lights;
    uint2 screenExtent;
}

[vk_push_constant]
PushConstants constants;

[vk_binding(0, 0)]
Texture2D<float4> gBuffer[];

static const Texture2D<float4> albedo = gBuffer[0];
static const Texture2D<float4> normal = gBuffer[1];
static const Texture2D<float4> depth = gBuffer[2];

[vk_binding(1, 0)]
RWTexture2D<float4> output;

float4 findWorldPos(uint2 uv, float depthNDC)
{
    float2 ndcXY = float2(uv / constants.screenExtent) * 2.0 - 1.0;
    float4 ndcPos = float4(ndcXY, depthNDC, 1.0f);

    float4 viewPos = ndcPos * constants.cam.invView;
    viewPos /= viewPos.w;

    float4 worldPos = viewPos * constants.cam.invPerspective;

    return worldPos;
}

[shader("compute")]
[numthreads(32, 32)]
float4 computeMain(uint2 threadId: SV_DispatchThreadID)
{
    float4 depthImg = depth[threadId];
    float depthNDC = (depthImg.x + depthImg.y + depthImg.z + depthImg.w);

    float4 fragPos = findWorldPos(threadId, depthNDC);
}
