#version 450

layout(location = 0) in FragData{
    vec3 color;
}fragData;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(fragData.color,1);
}
