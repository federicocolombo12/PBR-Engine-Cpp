#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Passiamo le coordinate del mondo al fragment shader (ci serve per calcolare la distanza delle luci!)
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    
    // Normal Matrix: per evitare che la rotazione/scalatura del modello deformi le normali
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;

    TexCoords = aTexCoords;
    
    gl_Position = projection * view * worldPos;
}
