#version 450

layout (binding = 1) uniform sampler2D sColorMap;

#define lightRadius 360.0

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;


layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec4 color = texture(sColorMap, inUV);
	vec3 ambient = color.rgb * vec3(1.0f);// * inColor;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 objectColor = vec3(1.0, 1.0, 0.8);

	vec3 diffuse = color.rgb;

	outFragColor = vec4((ambient + diffuse)*lightColor , 1.0f);//+ vec3(specular)+ diffuse
}