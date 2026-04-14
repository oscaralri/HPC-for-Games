#version 460 

struct InstanceData {
    mat4 modelMatrix;
    uint entityID;
    uint textureLayer;
    uint specLayer;
    uint padding0;    
    uint cmdIDs[2];
    uint padding1;    
    uint padding2;    
};

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(std140, binding = 3) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
};

layout(std140, binding = 5) uniform FrustumBuffer {
    vec4 cameraPos; // usamos vec4 por 16 bytes, como tal solo se usa xyz
    vec4 planes[6]; //plane: glm::vec3 n + float d
};

out vec3 vTexCoords;
out vec3 vSpecCoords;
out vec3 viewPos;
out vec3 FragPos;
out vec3 Normal;

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

layout(std430, binding = 1) buffer VisibleIndicesBuffer {
    uint visibleIndices[]; 
};

void main() {
    uint idx = visibleIndices[gl_DrawID];
    InstanceData data = instances[idx];

    FragPos = vec3(data.modelMatrix * vec4(inPos, 1.0));;
    vTexCoords = vec3(aTexCoords, float(data.textureLayer));
    vSpecCoords = vec3(aTexCoords, float(data.specLayer));
    Normal = mat3(data.modelMatrix) * aNormal;
    viewPos = cameraPos.xyz;
    gl_Position = projection * view * data.modelMatrix * vec4(inPos, 1.0);
}