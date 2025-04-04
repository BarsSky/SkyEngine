#version 450
#extension GL_EXT_geometry_shader : enable

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices=64) out;

layout(push_constant) uniform PushConsts{
    float line_thiknes;
    int segments;
    bool dash;
} pc;

layout(binding=0) uniform UniformBufferObject{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
}ubo;

layout (location = 0) in vec4 inColor[];
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 uv;

vec3 catmull_rom(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t){
    return 0.5*(
    (-p0 + 3.0*p1 - 3.0*p2 + p3)*t*t*t
    +(2.0*p0 - 5.0*p1 + 4.0*p2 - p3)*t*t
    +(-p0+p2)*t
    + 2.0*p1);
}

void main() {


    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    if (gl_PrimitiveIDIn == 0) p0 = p1 - (p2 -p1);
    if (gl_PrimitiveIDIn == gl_in.length() - 1) p3 = p2 + (p2 - p1);

    //
    int lodSegments = pc.segments;
    if(!pc.dash){
        float dist = distance(0.5 * (p1+p2), ubo.viewPos.xyz);
        lodSegments = clamp(int(pc.segments * 100.0/(dist + 1.0)), 4, 64);
    }

    for(int i = 0; i <= lodSegments; ++i){
        float t = i/float(lodSegments);

        vec3 curr = catmull_rom(p0,p1,p2,p3,t);
        vec3 next = catmull_rom(p0,p1,p2,p3,t + 0.01);//1.0/lodSegments);

        vec3 tangent = normalize(next - curr);
        vec3 normal = normalize(cross(tangent, vec3(0,1,0)));
        vec3 binormal = normalize(cross(tangent, normal));

        vec3 offset = normal*pc.line_thiknes;

        for(int j =0; j < 2; ++j){
            vec3 pos = curr + (j == 0 ? offset : -offset);
            gl_Position = ubo.projection * ubo.view *vec4(pos, 1.0);
            outColor = mix(inColor[1],inColor[2],t);
            uv = vec2(t, j);
            EmitVertex();
        }
    }
    EndPrimitive();
}