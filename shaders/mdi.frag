#version 460

in vec3 TexCoords;

out vec4 FragColor;

uniform sampler2DArray uTextureArray;

void main() {
    FragColor = texture(uTextureArray, TexCoords);
}