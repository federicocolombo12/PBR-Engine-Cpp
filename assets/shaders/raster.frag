#version 330 core
out vec4 FragColor;

in vec3 vLocalPos;

void main()
{
    // Generiamo un colore basandoci sulla posizione 3D dei vertici.
    // Convertiamo da -0.5/0.5 a 0.0/1.0
    vec3 color = vLocalPos + vec3(0.5);
    FragColor = vec4(color, 1.0);
}
