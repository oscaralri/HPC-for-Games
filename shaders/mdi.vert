#version 460

struct InstanceData {
    uint entityID;      
    mat4 modelMatrix;  
    uint textureLayer;
};

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(std140, binding = 3) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};
layout (location = 4) in float aTexIndex; 

out vec3 TexCoords; 

// El binding=0 debe coincidir con el que pongas en glBindBufferBase
layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

void main() {
    InstanceData data = instances[gl_DrawID];

    TexCoords = vec3(aTexCoords, data.textureLayer);
    gl_Position = projection * view * data.modelMatrix * vec4(inPos, 1.0);
}