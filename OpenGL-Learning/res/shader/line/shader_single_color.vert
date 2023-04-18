#shader vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

uniform mat4 u_MVP;

void main()
{
    v_Color = a_Color;
    gl_Position = u_MVP * vec4(a_Position, 0.0, 1.0);
};