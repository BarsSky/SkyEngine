#version 450

layout (set = 0, binding = 1) uniform sampler2D samplerColormap;
layout (set = 0, binding = 2) uniform sampler2D normalMap;


layout (location = 0) in vec3 aNormal;
layout (location = 1) in vec4 Color;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 ViewVec;
layout (location = 4) in vec3 LightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec4 color = texture(samplerColormap, UV)*Color;
	vec3 ambient = color.rgb * vec3(0.2f) * Color.rgb;
	vec3 N = normalize(aNormal)*(normalize(texture(normalMap,UV).xyz));
	vec3 L = normalize(LightVec);
	vec3 V = normalize(ViewVec);
	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N, L), 0.15).rrr;
	float specular = pow(max(dot(R, V), 0.0), 1.0);
	outFragColor = vec4(diffuse* color.rgb, color.a);//
}