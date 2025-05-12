#version 450

layout (set = 1,binding = 0) uniform sampler2D samplerColorMap;
layout (set = 1,binding = 1) uniform sampler2D samplerGradientRamp;

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inGradientPos;

layout (location = 0) out vec4 outFragColor;

void main () 
{
//	vec3 color = texture(samplerGradientRamp, vec2(1.0, 0.0)).rgb;
//	outFragColor.rgb = texture(samplerColorMap, gl_PointCoord).rgb;// * color;

	vec4 color;
//	// Rotate texture coordinates
//	// Rotate UV
//	float rotCenter = 0.5;
//	float rotCos = cos(0);
//	float rotSin = sin(0);
//	vec2 rotUV = vec2(
//	rotCos * (gl_PointCoord.x - rotCenter) + rotSin * (gl_PointCoord.y - rotCenter) + rotCenter,
//	rotCos * (gl_PointCoord.y - rotCenter) - rotSin * (gl_PointCoord.x - rotCenter) + rotCenter);
//
	color = texture(samplerGradientRamp, vec2(inGradientPos.x, 0.0));
//
	outFragColor.rgb = texture(samplerColorMap, gl_PointCoord).rgb*color.rgb*inGradientPos.y;
	outFragColor.a = inGradientPos.y;
}
