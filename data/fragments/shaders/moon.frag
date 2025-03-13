#version 450

layout (binding = 1) uniform sampler2D samplerColormap;


layout (location = 0) in vec3 aNormal;
layout (location = 1) in vec4 Color;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 ViewVec;
layout (location = 4) in vec3 LightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	float specularStrength = 0.5f;
	vec4 color = texture(samplerColormap, UV);
	vec3 ambient = color.rgb * vec3(0.15f) * Color.rgb;
	vec3 N = normalize(aNormal);
	vec3 L = normalize(LightVec);
	vec3 V = normalize(ViewVec);
	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N, L), 0.0) * color.rgb;
	//vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * vec3(0.75);
	outFragColor = vec4(ambient + diffuse, 1.0);// + vec3(specular)
}