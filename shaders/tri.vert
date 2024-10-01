#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_EXT_scalar_block_layout : require

layout (scalar,buffer_reference) buffer Buffer {
	vec3 positions[];
};

layout(std430, push_constant) uniform PC
{
    uint64_t ptr;
} pc;
layout (location = 0) out vec3 outColor;

void main() 
{

	const vec3 colors[3] = vec3[3](
		vec3(1.0f, 0.0f, 0.0f), 
		vec3(0.0f, 1.0f, 0.0f), 
		vec3(0.0f, 0.0f, 1.0f)
	);
	Buffer buf = Buffer(pc.ptr);
	gl_Position = vec4(buf.positions[gl_VertexIndex],1.0f);
	outColor = colors[gl_VertexIndex];
}
