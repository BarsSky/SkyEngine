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

layout (push_constant) uniform PushConstant{
    vec2 scale;
    vec2 translate;
} pushConst;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outColor = inColor;
    outUV = inUV;

    vec2 scale = vec2(2.f/ubo.viewportDim.x,2.f/ubo.viewportDim.y);
    gl_Position = vec4(inPos.xy,0,1.0) + vec4(ubo.viewPos.x,ubo.viewPos.y,0,0);//*scale + pushConst.translate, 0.0, 1.0)
                    //+ vec4(ubo.viewPos.xy*pushConst.scale - pushConst.translate, 0.0, 1.0);

    //    gl_Position = ubo.projection * ubo.view * ubo.model * ubo.viewPos;
    //    // Base size of the point sprites
    //    float spriteSize = 0.4;
    //    vec4 eyePos = ubo.view * vec4(inPos.xyz, 1.0);
    //    vec4 projectedCorner = ubo.projection * vec4(0.5 * spriteSize, 0.5 * spriteSize, 0, eyePos.w);
    //
    //
    //    gl_PointSize = ubo.viewportDim.x * projectedCorner.x / projectedCorner.w;
}