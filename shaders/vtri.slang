struct Vertex
{
    float3 position;
    float3 color;
}

[vk_push_constant]
const Vertex *ptrs;
[shader("vertex")]
float4 main(uint32_t vertID: SV_VertexID)
    : SV_Position
{
    return float4(ptrs[vertID].position, 0);
}
