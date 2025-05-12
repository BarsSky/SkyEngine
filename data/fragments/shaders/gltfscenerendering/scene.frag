#version 450

layout(set=2, binding=0) uniform sampler2D samplerColorMap;
layout(set=2, binding=1) uniform sampler2D samplerNormalMap;

layout(location=0)in vec3 inNormal;
layout(location=1)in vec3 inColor;
layout(location=2)in vec2 inUV;
layout(location=3)in vec3 inViewVec;
layout(location=4)in vec3 inLightVec;
layout(location=5)in vec4 inTangent;
layout(location=6)in flat uvec2 inID;

layout(location=0)out vec4 outFragColor;

layout(constant_id=0)const bool ALPHA_MASK=false;
layout(constant_id=1)const float ALPHA_MASK_CUTOFF=0.f;

layout(push_constant) uniform Push {
    vec2 mouse_pos;
    uint inUniqueID;// Number of picked object for change view
// Add parametr off depth check ass push constant uint value
    uint depth_array_value; //Uncomment then added to struct and filed
} pc;

layout(set = 0, binding = 0) buffer writeonly HitBuffer{
    uint data[];
}pickingBuffer;

void main()
{
    vec4 color=texture(samplerColorMap, inUV)*vec4(inColor, 1.);

    if (ALPHA_MASK){
        if (color.a<=ALPHA_MASK_CUTOFF){
            discard;
        }
    }

    vec3 N=normalize(inNormal);
    vec3 T=normalize(inTangent.xyz);
    vec3 B=cross(inNormal, inTangent.xyz)*inTangent.w;
    mat3 TBN=mat3(T, B, N);
    N = TBN * normalize(texture(samplerNormalMap, inUV).xyz*2.-vec3(1.));
    const float ambient=.1;
    vec3 L=normalize(inLightVec);
    vec3 V=normalize(inViewVec);
    vec3 R=reflect(-L, N);
    vec3 diffuse=max(dot(N, L), ambient).rrr;
    float specular=pow(max(dot(R, V), 0.), 32.);
    outFragColor=vec4(diffuse * color.rgb + specular, color.a);

    vec2 fragCoord = gl_FragCoord.xy;
    vec2 mouseCoord = pc.mouse_pos;

    uint depthIndex = uint(gl_FragCoord.z * pc.depth_array_value);// Масштабирование глубины
    if (distance(mouseCoord,fragCoord) < 1) { // Проверка близости к курсору
            pickingBuffer.data[depthIndex] = inID.y;
    }
    if(pc.inUniqueID == inID.y){
        vec3 light_color = vec3(1.0,1.0,1.0);
        outFragColor=vec4(diffuse * light_color + specular, color.a);
    }
}