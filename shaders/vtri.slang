struct Vertex
{
    float3 position;
    float3 color;
}

[vk_push_constant]
const Vertex *ptrs;
[shader("vertex")]
func main()->float3
{
}
