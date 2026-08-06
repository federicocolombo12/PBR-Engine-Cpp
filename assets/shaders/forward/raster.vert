#version 330 core
// A differenza del Fullscreen Quad che era 2D (x,y), il Cubo è 3D (x,y,z)
layout (location = 0) in vec3 aPos;

// Le 3 famose matrici MVP
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Passiamo le coordinate locali al fragment shader per fargli generare dei colori fake
out vec3 vLocalPos;

void main()
{
    vLocalPos = aPos;

    // L'ordine è rigorosamente da destra a sinistra:
    // 1. Spazio Locale -> Spazio Mondo (model)
    // 2. Spazio Mondo -> Spazio Vista (view)
    // 3. Spazio Vista -> Spazio Schermo (projection)
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
