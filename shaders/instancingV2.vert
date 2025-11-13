#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset; 

void main()
{
    gl_Position = vec4(aPos + vec3(aOffset, 0.0), 1.0);
}