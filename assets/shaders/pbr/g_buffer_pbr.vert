#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN; // Matrice per il Normal Mapping

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    // Calcolo della Matrice TBN (Tangent, Bitangent, Normal)
    // Convertiamo i vettori dal Model Space al World Space (usiamo una cast mat3 per annullare le traslazioni)
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    
    // Per sicurezza nel caso le tangenti non siano perfettamente ortogonali (Riorganizzazione Gram-Schmidt)
    T = normalize(T - dot(T, N) * N);
    
    TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPos;
}
