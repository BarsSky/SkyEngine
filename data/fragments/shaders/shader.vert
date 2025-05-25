#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec4 inTangent;

layout(set = 1, binding=0) uniform UniformBufferObject{
    uvec4 unique_id;
    mat4 model;//position
    mat4 view;
    mat4 projection;
    vec4 viewPos;
    vec4 lightPos;
}ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;
layout (location = 5) out vec4 outTangent;
layout (location = 6) out uvec2 outID;

void main(){
    outNormal = inNormal;
    outColor = inColor;
    outUV = inUV;
    outTangent = inTangent;
    outID = ubo.unique_id.xy;
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPos.xyz, 1.0);

    outNormal = mat3(ubo.model) * inNormal;
    vec4 pos = ubo.model * vec4(inPos, 1.0);
    outLightVec = ubo.lightPos.xyz - pos.xyz;
    outViewVec = ubo.viewPos.xyz - pos.xyz;
}