#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

// G-Buffer Packs
uniform sampler2D gPositionAO;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoMetallic;

uniform vec3 lightPos;
uniform vec3 viewPos;

const float PI = 3.14159265359;

// 1. NDF (Normal Distribution Function) - Trowbridge-Reitz GGX
// Questa funzione calcola statisticamente quante "microfaccette" sono orientate ESATTAMENTE 
// a metà tra la luce (L) e la telecamera (V). Questo vettore a metà si chiama H (Halfway vector).
// Più la superficie è ruvida, più questo valore si "spalma" creando un riflesso sfocato.
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness; // Trucco Epic Games: elevare al quadrato rende il passaggio tra ruvido e lucido più naturale alla vista
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    // Formula originale di Trowbridge-Reitz
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / max(denom, 0.0000001); // Previene la divisione per zero
}

// 2. Funzione di Geometria - Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // Per le luci dirette (Point/Spot)
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness); // Ostacolo verso telecamera
    float ggx1 = GeometrySchlickGGX(NdotL, roughness); // Ostacolo verso la luce
    return ggx1 * ggx2;
}

// 3. Equazione di Fresnel - Schlick
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    // UNPACKING - Estraiamo le 6 variabili dalle 3 texture
    vec4 posAO = texture(gPositionAO, TexCoords);
    vec3 FragPos = posAO.rgb;
    float ao = posAO.a;

    vec4 normRough = texture(gNormalRoughness, TexCoords);
    vec3 N = normalize(normRough.rgb);
    float roughness = normRough.a;

    vec4 albedoMetal = texture(gAlbedoMetallic, TexCoords);
    vec3 albedo = pow(albedoMetal.rgb, vec3(2.2)); // Le texture diffuse sono in sRGB, le riportiamo in lineare per la fisica
    float metallic = albedoMetal.a;

    vec3 V = normalize(viewPos - FragPos);

    // F0 base per la riflettività al punto di incidenza (0.04 è lo standard per i non-metalli)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic); // I metalli "tingono" il riflesso con il proprio Albedo!

    // ==========================================
    // BRDF Cook-Torrance (Per una singola luce puntiforme)
    // ==========================================
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L); // Halfway vector
    
    // Decadimento della luce fisicamente accurato (Legge dell'inverso del quadrato)
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance); 
    vec3 radiance = vec3(500.0) * attenuation;

    // Calcoliamo i 3 componenti della BRDF (Il numeratore dell'equazione di rendering)
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // Numeratore: D * G * F
    vec3 numerator = NDF * G * F;
    
    // Denominatore: 4 * (N dot V) * (N dot L). (Fattore di normalizzazione)
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    
    // Il risultato è il raggio Speculare finale!
    vec3 specular = numerator / denominator;

    // kS rappresenta la percentuale di luce che RIMBALZA come specchio (quindi è pari a Fresnel)
    vec3 kS = F;
    
    // kD rappresenta la percentuale di luce che ENTRA nell'oggetto (energia rimanente per conservazione dell'energia)
    vec3 kD = vec3(1.0) - kS;
    
    // Regola d'oro del PBR: I metalli puri non hanno rifrazione! Assorbono tutta la luce interna.
    // Quindi moltiplichiamo kD per (1 - metallic). Se metallic è 1 (Metallo), kD diventa 0!
    kD *= 1.0 - metallic;


    float NdotL = max(dot(N, L), 0.0);
    // Sommiamo la componente diffusa e speculare
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    // Aggiungiamo un po' di luce ambientale moltiplicata per l'Ambient Occlusion
    // L'ho alzata a 0.2 per togliere l'effetto "Vuoto Cosmico", in futuro si userà una Skybox (IBL) per l'ambient
    vec3 ambient = vec3(0.2) * albedo * ao;
    vec3 color = ambient + Lo;

    // Tone Mapping (Reinhard) e Gamma Correction
    // Riportiamo i colori calcolati in lineare allo spazio sRGB per lo schermo
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
