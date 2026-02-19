#version 460 core

out vec4 FragColor;
in vec3 TexCoords;

uniform sampler2DArray uTextureArray;

void main() {
    FragColor = texture(uTextureArray, TexCoords);
}