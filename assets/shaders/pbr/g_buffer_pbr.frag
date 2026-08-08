#version 330 core
// Abbiamo impacchettato 6 canali dati in sole 3 texture!
layout (location = 0) out vec4 gPositionAO;
layout (location = 1) out vec4 gNormalRoughness;
layout (location = 2) out vec4 gAlbedoMetallic;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

// Sampler provenienti dal C++
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform bool hasTexture;

void main() {
    // 1. Estrazione Texture Base
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;
    
    // 2. Normal Mapping Reale
    // La normal map è memorizzata nell'intervallo [0, 1]. Dobbiamo portarla in [-1, 1].
    vec3 normalMap = texture(texture_normal1, TexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0; 
    // Moltiplichiamo per la matrice TBN per portare la normale dallo Spazio Tangente allo Spazio Mondo
    vec3 N = normalize(TBN * normalMap);

    // 3. Estrazione Proprietà PBR
    float metallic = texture(texture_metallic1, TexCoords).r;
    float roughness = texture(texture_roughness1, TexCoords).r;
    float ao = texture(texture_ao1, TexCoords).r;

    // 4. PACKING - Riempiamo il G-Buffer
    gPositionAO.rgb = FragPos;
    gPositionAO.a = ao;

    gNormalRoughness.rgb = N;
    gNormalRoughness.a = roughness;

    gAlbedoMetallic.rgb = albedo;
    gAlbedoMetallic.a = metallic;
}
