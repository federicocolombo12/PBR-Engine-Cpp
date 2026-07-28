#version 330 core
// "layout (location = 0)" corrisponde esattamente allo 0 che abbiamo scritto
// nel comando glVertexAttribPointer() in C++!
layout (location = 0) in vec2 aPos;

// Variabile in uscita (out) per passarla al Fragment Shader
out vec2 vUv;

void main()
{
    // In OpenGL il centro dello schermo è (0,0), la sinistra è -1, la destra è +1.
    // Trasformiamo queste coordinate matematiche in coordinate UV classiche (da 0 a 1)
    vUv = aPos * 0.5 + 0.5;
    
    // gl_Position è una variabile built-in di OpenGL. 
    // Richiede un vec4 (x, y, z, w). Essendo un piano 2D mettiamo z=0 e w=1.
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
