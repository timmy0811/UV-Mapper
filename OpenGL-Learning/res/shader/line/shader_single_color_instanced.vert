#shader vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

uniform mat4 u_MVP;
uniform float u_TexelWidth;

void main()
{
    v_Color = a_Color;
    vec2 pos = a_Position + vec2(0.0, u_TexelWidth * float(gl_InstanceID));
    gl_Position = u_MVP * vec4(pos, -0.1, 1.0);
};