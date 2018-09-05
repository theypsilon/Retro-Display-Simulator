#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec4 ObjectColor;

uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
    if (ObjectColor.a == 0.0) {
        discard;
    }
    
    // ambient
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    FragColor = vec4(ambient + diffuse * 0.5, 1.0) * ObjectColor;
} 