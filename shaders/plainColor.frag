#version 460 core

out vec4 FragColor;

vec3 color = vec3(1.f, 1.f, 1.f);

void main()
{
    FragColor = vec4(color, 1.0f);
}