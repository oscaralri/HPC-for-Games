#version 460 

struct InstanceData
{
    mat4 modelMatrix;
    uint entityID;
    uint textureLayer;
    uint specLayer;
    uint padding0;    
    uint cmdIDs[2];
};

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(std140, binding = 3) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};

layout (location = 4) in float aTexIndex; 

out vec3 vTexCoords;

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

layout(std430, binding = 1) buffer VisibleIndicesBuffer {
    uint visibleIndices[]; 
};

void main() {
    uint idx = visibleIndices[gl_DrawID];
    InstanceData data = instances[idx];

    vTexCoords = vec3(aTexCoords, float(data.textureLayer));
    gl_Position = projection * view * data.modelMatrix * vec4(inPos, 1.0);
}