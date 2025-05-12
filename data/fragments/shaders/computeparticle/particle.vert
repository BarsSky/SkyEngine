#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inGradientPos;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outGradientPos;


layout (set =1,binding = 2) uniform UBO
{
    uvec4 uinique_id;
    mat4 projection;
    mat4 modelview;
    mat4 model;
    vec2 viewportDim;
    vec3 emitpos;
} ubo;


out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

void main ()
{
    outColor = vec4(0.035);
    vec3 PPos = inPos.xyz+ubo.emitpos.xyz;
    outGradientPos = inGradientPos.xy;
vec4 popo = vec4(PPos.xyz, 1.0);

    gl_Position = ubo.projection * ubo.modelview * ubo.model *popo;
    // Base size of the point sprites
    float spriteSize = 0.4;
    vec4 eyePos = ubo.modelview * vec4(PPos.xyz, 1.0);
    vec4 projectedCorner = ubo.projection * vec4(0.5 * spriteSize, 0.5 * spriteSize, eyePos.z, eyePos.w);

    
    gl_PointSize = inGradientPos.z*0.5*ubo.viewportDim.x * projectedCorner.x / projectedCorner.w;
}