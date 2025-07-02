struct VertOut
{
    float4 pos : SV_Position;
    float3 color : OUTCOLOR;
}
struct Vertex
{
    float3 pos;
}

struct VertPC
{
    Vertex *vertBuf;
}
[vk_push_constant]
const VertPC pushconsts;

[vk_binding(0, 1)]
Sampler2D textures[];

[shader("vertex")]
VertOut vertMain(uint32_t vertId: SV_VertexID)
{
    VertOut output = {};

    const float3 colors[3] = float3[3](
        float3(1.0f, 0.0f, 0.0f), // red
        float3(0.0f, 1.0f, 0.0f), // green
        float3(0.0f, 0.0f, 1.0f)  // blue
    );

    output.pos = float4(pushconsts.vertBuf[vertId].pos, 1.0f);
    output.color = colors[vertId % 3];
    return output;
}

[shader("fragment")]
float4 fragMain(VertOut input)
{
    return float4(input.color, 1.0f);
}
