#version 450

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 model;//position
	mat4 view;
	mat4 projection;
	mat4 normal;
	vec4 lightPos;
} ubo;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;



void main() 
{
	outUV = inUV;
	outNormal = inNormal;

	mat4 modelView = ubo.view * ubo.model;
	vec4 pos = modelView * inPos;
	gl_Position = ubo.projection * pos;	
}
