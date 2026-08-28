#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;
out vec3 TangentViewPos; // Nuove variabili per il POM
out vec3 TangentFragPos; // Nuove variabili per il POM

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos; // La telecamera in coordinate mondo

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    // 1. Calcolo della Matrice TBN (Dal Modello al Mondo)
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    
    // Gram-Schmidt per sicurezza
    T = normalize(T - dot(T, N) * N);
    TBN = mat3(T, B, N);

    // 2. TBN Trasposta (Dal Mondo allo Spazio Tangente)
    // La matrice inversa di una matrice ortogonale è la sua trasposta.
    mat3 invTBN = transpose(TBN);

    // 3. Trasformiamo le posizioni chiave nello spazio locale del triangolo!
    TangentViewPos = invTBN * viewPos;
    TangentFragPos = invTBN * FragPos;

    gl_Position = projection * view * worldPos;
}
