#shader vertex
#version 330 core

precision mediump float;

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in float a_TexIndex;

out vec2 v_UV;
flat out int v_TexIndex;

uniform mat4 u_MVP;
uniform float u_WidthTiles;
uniform float u_TileWidth;

void main()
{
	v_TexIndex = int(a_TexIndex);
	v_UV = a_UV;

	vec2 position = vec2(0.0);
	position.x = a_Position.x + mod(gl_InstanceID, u_WidthTiles) * u_TileWidth;
	position.y = a_Position.y + -floor(gl_InstanceID / u_WidthTiles) * u_TileWidth;

    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
};