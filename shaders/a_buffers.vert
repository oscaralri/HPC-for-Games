#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(std140, binding = 3) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};
layout (location = 4) in float aTexIndex; 

out vec3 TexCoords; 

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    TexCoords = vec3(aTexCoords, aTexIndex);
}