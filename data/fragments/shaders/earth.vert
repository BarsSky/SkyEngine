#version 450

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec4 Color;

layout (binding = 0) uniform UBO 
{
	mat4 position;
	mat4 view;
	mat4 projection;
	mat4 normal;
	vec4 lightPos;
} ubo;

layout (location = 0) out vec3 aNormal;
layout (location = 1) out vec4 aColor;
layout (location = 2) out vec2 aUV;
layout (location = 3) out vec3 ViewVec;
layout (location = 4) out vec3 LightVec;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	mat4 modelView = ubo.view * ubo.position;
	vec4 pos = modelView * vec4(Pos,1.0f);
	//aNormal = mat3(transpose(inverse(ubo.position)))*Normal;
	aColor = Color;
	aUV = UV;
	gl_Position = ubo.projection *pos;
	
	aNormal = mat3(ubo.position) * Normal;
	vec3 lPos = vec3(ubo.position * vec4(Pos,1.0f));
	LightVec =  ubo.lightPos.xyz - lPos;
	ViewVec = vec3(modelView * pos) - pos.xyz;
}
