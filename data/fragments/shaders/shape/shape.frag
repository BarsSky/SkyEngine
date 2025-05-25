#version 450

layout (set = 1, binding = 1) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inLightVec;
layout (location = 4) in vec4 inColor;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec4 color = texture(samplerColor, inUV);
    outFragColor = inColor*color;

}