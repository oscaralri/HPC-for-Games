#version 460

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

Light light = Light(
    vec3(-300.0, 100.0, 0.0), 
    vec3(0.5),           
    vec3(1),            
    vec3(0.5)             
);

in vec3 vTexCoords;
in vec3 vSpecCoords;
in vec3 FragPos;  
in vec3 Normal;  
in vec3 viewPos;

out vec4 FragColor;

layout(binding = 0) uniform sampler2DArray uDiffuseArray; 
layout(binding = 1) uniform sampler2DArray uSpecularArray;

void main()
{
    vec3 texSpecular = texture(uSpecularArray, vSpecCoords).rgb;

    // ambient
    vec3 ambient = light.ambient * texture(uDiffuseArray, vTexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(uDiffuseArray, vTexCoords).rgb;  

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
    vec3 specular = light.specular * spec * texSpecular;  


    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 