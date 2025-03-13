#version 450

// layout (input_attachment_index = 0, binding = 1) uniform subpassInput samplerPositionDepth;
layout (binding = 1) uniform sampler2D samplerTexture;

layout (location = 0) in vec3 aNormal;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 ViewVec;
layout (location = 4) in vec3 LightVec;

layout (location = 0) out vec4 outColor;

// layout (constant_id = 0) const float NEAR_PLANE = 0.1f;
// layout (constant_id = 1) const float FAR_PLANE = 256.0f;

// float linearDepth(float depth)
// {
// 	float z = depth * 2.0f - 1.0f; 
// 	return (2.0f * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));	
// }

void main () 
{
	// Sample depth from deferred depth buffer and discard if obscured
	float depth = 1;//subpassLoad(samplerPositionDepth).a;

	// Save the sampled texture color before discarding.
	// This is to avoid implicit derivatives in non-uniform control flow.
	vec4 sampledColor = vec4(texture(samplerTexture, inUV).rgb,0.5f);
	vec3 ambient = sampledColor.rgb * vec3(0.2f) * inColor.rgb;
	vec3 N = normalize(aNormal);
	vec3 L = normalize(LightVec);
	vec3 V = normalize(ViewVec);
	vec3 R = reflect(-L, N);
	vec3 diffuse = max(dot(N, L), 0.15).rrr;
	float specular = pow(max(dot(R, V), 0.0), 1.0);
	outColor = vec4(diffuse* sampledColor.rgb, sampledColor.a);//
}
