struct Vertex
{
    float3 position;
    float3 color;
}

[vk_push_constant]
const Vertex *ptrs;

struct VertOutput
{
    float3 color : fragColor;
    float4 sv_pos : SV_Position;
}

[shader("vertex")]
VertOutput vertMain(uint32_t vertID: SV_VertexID)
{
    VertOutput output;

    output.color = ptrs[vertID].color;
    output.sv_pos = float4(ptrs[vertID].position, 1);

    return output;
}

[shader("fragment")]
float4 fragMain(float3 a_color: fragColor)
    : SV_Target
{
    return float4(a_color, 1.0);
}
