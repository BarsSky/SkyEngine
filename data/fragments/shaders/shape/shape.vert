#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec4 inColor;
layout (location = 4) in vec4 inTangent;

layout (set = 1, binding = 0) uniform UBO
{
    uvec4 unique_id;
    mat4 projection;
    mat4 model;
    mat4 view;
    vec4 viewPos;
    vec2 viewportDim;
} ubo;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outViewVec;
layout (location = 3) out vec3 outLightVec;
layout (location = 4) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outColor = inColor;
    outUV = inUV;

    gl_Position = vec4(inPos, 1.0);
}