#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec4 inTangent;

layout (set = 1, binding = 0) uniform UBOScene
{
    uvec4 unique_id;
    mat4 model;
    mat4 view;
    mat4 projection;
    vec4 viewPos;
    vec4 lightPos;
} umat;

//layout (set = 0, binding = 1) uniform UBOScene
//{
//	mat4 model;
//	mat4 view;
//	mat4 projection;
//	vec4 viewPos;
//	vec4 lightPos;
//} uboScene;

// layout(push_constant) uniform PushConsts {
// 	mat4 model;
// } primitive;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;
layout (location = 5) out vec4 outTangent;
layout (location = 6) out uvec2 outID;

void main()
{

    outNormal = inNormal;
    outColor = inColor;
    outUV = inUV;
    outTangent = inTangent;
    outID = umat.unique_id.xy;
    gl_Position = umat.projection * umat.view * umat.model * vec4(inPos.xyz, 1.0);

    outNormal = mat3(umat.model) * inNormal;
    vec4 pos = umat.model * vec4(inPos, 1.0);
    outLightVec = umat.lightPos.xyz - pos.xyz;
    outViewVec = umat.viewPos.xyz - pos.xyz;
}