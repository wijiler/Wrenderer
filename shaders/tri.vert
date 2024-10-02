#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_EXT_scalar_block_layout : require

struct Vertex {
	vec3 position;
	vec3 color;
};

layout (scalar,buffer_reference) buffer Buffer {
	Vertex vertices[];
};

layout(std430, push_constant) uniform PC
{
    uint64_t ptr;
} pc;
layout (location = 0) out vec3 outColor;

void main() 
{

	Buffer buf = Buffer(pc.ptr);
	gl_Position = vec4(buf.vertices[gl_VertexIndex].position,1.0f);
	outColor = buf.vertices[gl_VertexIndex].color;
}
