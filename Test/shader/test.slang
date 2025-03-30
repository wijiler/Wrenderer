struct VertOut
{
    float4 pos : SV_Position;
    float3 color : OUTCOLOR;
}

[shader("vertex")]
VertOut vertMain(uint32_t vertId: SV_VertexID)
{
    VertOut output = {};
    const float3 positions[3] = float3[3](
        float3(1.f, 1.f, 0.0f),
        float3(-1.f, 1.f, 0.0f),
        float3(0.f, -1.f, 0.0f));

    const float3 colors[3] = float3[3](
        float3(1.0f, 0.0f, 0.0f), // red
        float3(0.0f, 1.0f, 0.0f), // green
        float3(0.0f, 0.0f, 1.0f)  // blue
    );

    output.pos = float4(positions[vertId % 3], 1.0f);
    output.color = colors[vertId % 3];
    return output;
}

[shader("fragment")]
float4 fragMain(VertOut input)
{
    return float4(input.color, 1.0f);
}
