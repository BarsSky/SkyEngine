#version 450 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec2 outUV;

layout (set = 1, binding = 1) uniform UBO
{
	uvec4 unique_id;
	mat4 projection;
	mat4 model;
	mat4 view;
	vec4 viewPos;
	vec2 viewportDim;
} ubo;

layout (push_constant) uniform PushConstant{
	vec2 scale;
	vec2 translate;
} pushConst;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main(void)
{
	gl_Position =  vec4(inPos.xy,0,1.0);
	outUV = inUV;
}
