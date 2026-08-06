#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 lightPos; 
uniform vec3 viewPos;

const vec3 lightColor = vec3(1.0, 0.9, 0.8);

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a; 
    
    if(length(Normal) < 0.1) {
        FragColor = vec4(0.1, 0.12, 0.15, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    
    // 1. Ambient
    vec3 ambient = 0.1 * Albedo;
    
    // 2. Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * Albedo;
    
    // 3. Specular (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = lightColor * spec * Specular;
    
    // Attenuazione matematica ricalibrata (meno aggressiva)
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.045 * distance + 0.0075 * (distance * distance));
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}