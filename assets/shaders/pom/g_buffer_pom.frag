#version 330 core
layout (location = 0) out vec4 gPositionAO;
layout (location = 1) out vec4 gNormalRoughness;
layout (location = 2) out vec4 gAlbedoMetallic;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;
// Novità: La Height Map!
uniform sampler2D texture_height1; 

uniform bool hasTexture;

// Ricalcola le coordinate UV applicando l'algoritmo di Parallax Occlusion Mapping
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    // Fattore di scala dell'altezza (quanto sono "profondi" i mattoni)
    float heightScale = 0.1; 

    // --- LOD DINAMICO (Level of Detail) ---
    // Meno step se guardiamo dritto (minLayers), più step se guardiamo di lato (maxLayers)
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    // dot(vec3(0.0, 0.0, 1.0), viewDir) ci dice quanto siamo "allineati" alla Normale
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  

    // Calcoliamo la "dimensione" di ogni singolo step in profondità
    float layerDepth = 1.0 / numLayers;
    // Altezza corrente del raggio (parte da 0.0 e scende a 1.0)
    float currentLayerDepth = 0.0;

    // Di quanto spostiamo le UV a ogni step? 
    // viewDir.xy / viewDir.z proietta il raggio 3D sul piano 2D delle UV
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    // Campioniamo l'altezza vera dalla mappa
    float currentDepthMapValue = texture(texture_height1, currentTexCoords).r;

    // --- RAYMARCHING DISCRETO ---
    while(currentLayerDepth < currentDepthMapValue) {
        // Scivoliamo lungo le UV
        currentTexCoords -= deltaTexCoords;
        // Scendiamo in profondità
        currentLayerDepth += layerDepth;  
        // Leggiamo la nuova altezza reale
        currentDepthMapValue = texture(texture_height1, currentTexCoords).r;  
    }

    // --- INTERPOLAZIONE SUB-PIXEL (POM REALE) ---
    // Preleviamo lo stato PRIMA dello schianto
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;
    
    // Calcoliamo il peso per l'interpolazione lineare
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main() {
    // 1. Calcoliamo la direzione della vista nello Spazio Tangente
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    
    // 2. Parallax Occlusion Mapping
    vec2 finalTexCoords = ParallaxMapping(TexCoords, viewDir);

    // 3. SILHOUETTE CLIPPING (Crimson Desert Style)
    // Se il raggio è scivolato "fuori" dall'immagine originale per cercare un fondo, disintegriamo il pixel!
    if(finalTexCoords.x > 1.0 || finalTexCoords.y > 1.0 || finalTexCoords.x < 0.0 || finalTexCoords.y < 0.0) {
        discard;
    }

    // --- Da qui in poi, il G-Buffer si riempie normalmente, ma usando le UV MAGICHE! ---
    
    // Estrazione Texture Base usando finalTexCoords
    vec3 albedo = texture(texture_diffuse1, finalTexCoords).rgb;
    
    // Normal Mapping
    vec3 normalMap = texture(texture_normal1, finalTexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0; 
    vec3 N = normalize(TBN * normalMap);

    // Proprietà PBR
    float metallic = texture(texture_metallic1, finalTexCoords).r;
    float roughness = texture(texture_roughness1, finalTexCoords).r;
    float ao = texture(texture_ao1, finalTexCoords).r;

    // PACKING nel G-Buffer
    gPositionAO.rgb = FragPos;
    gPositionAO.a = ao;

    gNormalRoughness.rgb = N;
    gNormalRoughness.a = roughness;

    gAlbedoMetallic.rgb = albedo;
    gAlbedoMetallic.a = metallic;
}
