#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 trans;
uniform float theta;
uniform float xoffset;
uniform float yoffset;

float x = aPos.x * cos(theta) - aPos.y * sin(theta) + xoffset;
float y = aPos.y * cos(theta) + aPos.x * sin(theta) + yoffset;

void main()
{
	gl_Position = trans * vec4(x, y, aPos.z, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}