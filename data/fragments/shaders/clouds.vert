#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;

layout (binding = 0) uniform UBO
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 lightPos;
} ubo;

layout (location = 0) out vec3 aNormal;
layout (location = 1) out vec4 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 ViewVec;
layout (location = 4) out vec3 LightVec;

void main ()
{
    aNormal = mat3(ubo.model)*inNormal;
    outColor = inColor;
    outUV = inUV;
    mat4 modelView = ubo.view * ubo.model;
    vec4 pos = modelView * vec4(inPos, 1.0f);
    vec3 lPos = vec3(ubo.model * vec4(inPos, 1.0f));
    LightVec = ubo.lightPos.xyz - lPos;
    ViewVec = vec3(modelView * pos) - pos.xyz;

    gl_Position = ubo.projection *pos;
}