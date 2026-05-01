#version 460 

struct InstanceData {
    mat4 modelMatrix;
    uint entityID;
    uint diffuseLayer[2];
    uint specLayer;   
    uint cmdIDs[2];
    uint lodActive;
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

layout(std430, binding = 0) buffer InstanceBuffer {
    InstanceData instances[];
};

layout(std430, binding = 1) buffer VisibleIndicesBuffer {
    uint visibleIndices[]; 
};

out vec3 vTexCoords;
out vec3 vSpecCoords;
out vec3 viewPos;   
out vec3 FragPos;
out vec3 Normal;

void main() {
    uint idx = visibleIndices[gl_DrawID];
    InstanceData data = instances[idx];
    uint lodLevel = gl_BaseInstance;

    vec4 worldPos = data.modelMatrix * vec4(inPos, 1.0);
    FragPos = worldPos.xyz;

    vTexCoords = vec3(aTexCoords, float(data.diffuseLayer[lodLevel]));
    vSpecCoords = vec3(aTexCoords, float(data.specLayer));

    Normal = mat3(data.modelMatrix) * aNormal;

    viewPos = cameraPos.xyz;

    gl_Position = projection * view * worldPos;
}