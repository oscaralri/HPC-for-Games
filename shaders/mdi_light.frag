#version 460

const uint UNUSED_SPEC = 0xFFFFFFFFu;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const vec3 LIGHT_POS = vec3(1000.0, 1000.0, 1000.0);
const vec3 TARGET    = vec3(0.0, 0.0, 0.0);

DirectionalLight dirLight = DirectionalLight(
    normalize(LIGHT_POS - TARGET), 
    vec3(0.5),           
    vec3(1.0),            
    vec3(0.5)             
);

const vec3 FOG_COLOR = vec3(0.5, 0.6, 0.7); 
const float FOG_DENSITY = 0.000012;            
const float FOG_GRADIENT = 2.0;          

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
    vec3 texDiffuse = texture(uDiffuseArray, vTexCoords).rgb;
    vec3 texSpecular = texture(uSpecularArray, vSpecCoords).rgb;

    // ambient
    vec3 ambient = dirLight.ambient * texDiffuse;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texDiffuse;

    // specular
    vec3 specular = vec3(0.0);
    if(uint(vSpecCoords.z) != UNUSED_SPEC)
    {
        vec3 texSpecular = texture(uSpecularArray, vSpecCoords).rgb;
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
        
        specular = dirLight.specular * spec * texSpecular;
    }
    

    vec3 color = ambient + diffuse + specular;

    float distance = length(viewPos - FragPos);
    float visibility = exp(-pow((distance * FOG_DENSITY), FOG_GRADIENT));
    visibility = clamp(visibility, 0.0, 1.0);

    FragColor = vec4(mix(FOG_COLOR, color, visibility), 1.0);
    
    
    //FragColor = vec4(result, 1.0);
} 