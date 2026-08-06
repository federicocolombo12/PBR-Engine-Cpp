#version 330 core
// Definiamo i 3 output (le 3 texture del G-Buffer)
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// Mappa Diffuse caricata da C++ tramite stb_image
uniform sampler2D texture_diffuse1;
uniform bool hasTexture; // Novità: Interruttore di sicurezza!

void main()
{    
    // 1. Salviamo la coordinata World Space nella prima texture
    gPosition = vec4(FragPos, 1.0);
    
    // 2. Salviamo la Normale nella seconda texture (normalizzata!)
    gNormal = vec4(normalize(Normal), 1.0);
    
    // 3. Leggiamo il colore direttamente dalla texture mappata e lo salviamo nel G-Buffer!
    if (hasTexture) {
        gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    } else {
        // Se il modello non ha texture, usiamo un legno rossastro di emergenza
        gAlbedoSpec.rgb = vec3(0.6, 0.2, 0.2);
    }
    // Lasciamo l'Alpha a 0.5 per lo speculare (potremmo usare una texture_specular1 se l'avessimo)
    gAlbedoSpec.a = 0.5; 
}
