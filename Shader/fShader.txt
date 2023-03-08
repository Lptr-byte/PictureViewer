#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D myTexture1;

void main(){
    FragColor = texture(myTexture1, TexCoord);
}