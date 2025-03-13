#version 450

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	mat4 normal;
	vec4 lightPos;
	mat4 position;
} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	mat4 modelView = ubo.model * ubo.position;
	vec4 pos = modelView * inPos;
	outNormal = inNormal;
	outColor = inColor;
	outUV = inUV;
	gl_Position = ubo.projection *pos;
	
	pos = ubo.model * inPos;
	outNormal = mat3(ubo.model) * inNormal;
	vec3 lPos = mat3(ubo.model) * ubo.lightPos.xyz;
	outLightVec = lPos - pos.xyz;
	outViewVec = vec3(modelView * pos);//-pos.xyz;		
}
