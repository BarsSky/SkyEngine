#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec3 inTangent;

layout (binding = 0) uniform UBO
{
    mat4 projection;
    mat4 model;
    mat4 view;
    vec4 viewPos;
} ubo;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outViewVec;
layout (location = 3) out vec3 outLightVec;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outUV = inUV;
    mat4 modelPos = ubo.view * ubo.model;
    vec4 worldPos = modelPos * vec4(inPos, 1.0);
    gl_Position = ubo.projection * worldPos;
}