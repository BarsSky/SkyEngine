#version 450

layout(location=0)in vec4 inColor;
layout(location=1)in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

layout(constant_id=0)const bool ALPHA_MASK=false;
layout(constant_id=1)const float ALPHA_MASK_CUTOFF=0.f;

layout(set = 0, binding = 0) buffer writeonly HitBuffer{
    uint data[];
}pickingBuffer;

void main()
{
    vec4 color = inColor;
    float distance_from_center = abs(inUV.y - 0.5);
    float alpha = smoothstep(0.45,0.5,distance_from_center);
    outFragColor = vec4(color.rgb,1.0 - alpha);
}