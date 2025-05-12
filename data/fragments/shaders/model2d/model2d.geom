#version 450
#extension GL_EXT_geometry_shader : enable

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

layout(push_constant) uniform PushConsts{
    float line_thiknes;
    int segments;
    bool dash;
} pc;

//layout (location = 0) in vec4 inColor[];
//layout (location = 0) in vec2 inUV[];
//layout (location = 1) in vec3 inNormal[];
//layout (location = 2) in vec3 inViewVec[];
//layout (location = 3) in vec3 inLightVec[];
//layout (location = 4) in vec4 inColor[];
//
//layout (location = 0) out vec2 outUV;
//layout (location = 1) out vec3 outNormal;
//layout (location = 2) out vec3 outViewVec;
//layout (location = 3) out vec3 outLightVec;
//layout (location = 4) out vec4 outColor;

vec3 catmull_rom(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t){
    return 0.5*(
    (-p0 + 3.0*p1 - 3.0*p2 + p3)*t*t*t
    +(2.0*p0 - 5.0*p1 + 4.0*p2 - p3)*t*t
    +(-p0+p2)*t
    + 2.0*p1);
}

void main() {


//    vec3 p0 = gl_in[0].gl_Position.xyz;
//
//    outColor = inColor[0];
//    outUV = inUV[0];

}