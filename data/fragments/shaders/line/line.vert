#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColor;

//layout(binding=0) uniform UniformBufferObject{
//    mat4 model;//position
//    mat4 view;
//    mat4 projection;
//    vec2 viewportDim;
//    float pointSize;
//}ubo;

layout (location = 0) out vec4 outColor;


//out gl_PerVertex
//{
//    vec4 gl_Position;
//};

void main(){
    outColor = inColor;

    gl_Position = vec4(inPos.xyz, 1.0);
//    gl_Position = ubo.projection * ubo.view * vec4(inPos.xyz, 1.0);

//    vec4 pos = ubo.model * vec4(inPos, 1.0);

}