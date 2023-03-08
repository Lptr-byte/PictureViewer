#version 460 core
layout(location=0) in vec3 aPos;
layout(location=2) in vec2 aCoord;

out vec2 TexCoord;

uniform mat4 trans;
uniform float xoffset;
uniform float yoffset;
uniform float theta;

float x = aPos.x * cos(theta) - aPos.y * sin(theta);
float y = aPos.x * sin(theta) + aPos.y * cos(theta);

void main(){
    gl_Position = trans * vec4(x + xoffset, y + yoffset, aPos.z, 1.0);
    TexCoord = aCoord;
}