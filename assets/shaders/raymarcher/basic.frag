#version 330 core
out vec4 FragColor;

in vec2 vUv;

// Uniform passate dalla CPU (C++)
uniform vec2 iResolution;
uniform float iTime;

// Funzione SDF per una sfera: 
// p = posizione nello spazio 3D, s = raggio
float sdSphere(vec3 p, float s) {
    return length(p) - s;
}

// Mappa della scena (qui potremmo aggiungere altri oggetti)
float map(vec3 p) {
    // Sfera al centro, che oscilla su e giù nel tempo
    vec3 spherePos = vec3(0.0, sin(iTime) * 0.5, 0.0);
    return sdSphere(p - spherePos, 1.0);
}

// Calcolo della normale calcolando il gradiente dell'SDF
vec3 calcNormal(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx)
    ));
}

void main()
{
    // 1. Setup delle coordinate UV da -1 a 1 corrette per l'aspect ratio
    vec2 uv = vUv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    // 2. Setup della Camera
    vec3 ro = vec3(0.0, 0.0, -3.0); // Ray Origin (Posizione Camera)
    vec3 rd = normalize(vec3(uv, 1.0)); // Ray Direction

    // 3. Il Loop di Raymarching
    float t = 0.0;     // Distanza totale percorsa
    float d = 0.0;     // Distanza dall'oggetto più vicino
    int maxSteps = 100;
    
    for(int i = 0; i < maxSteps; i++) {
        vec3 p = ro + rd * t; // Posizione corrente lungo il raggio
        d = map(p);           // Distanza dall'SDF
        
        if(d < 0.001 || t > 100.0) break; // Colpito o andato troppo lontano
        
        t += d; // Avanza il raggio
    }

    // 4. Colorazione e Shading
    vec3 col = vec3(0.1, 0.12, 0.15); // Colore di sfondo (uguale a glClearColor!)

    if(t < 100.0) { // Se abbiamo colpito la sfera
        vec3 p = ro + rd * t;
        vec3 n = calcNormal(p);
        
        // Direzione luce (ruota nel tempo)
        vec3 lightDir = normalize(vec3(sin(iTime), 1.0, cos(iTime)));
        
        // Modello di illuminazione Lambertiano base
        float diff = max(dot(n, lightDir), 0.0);
        
        col = vec3(0.8, 0.2, 0.3) * diff; // Sfera rossa illuminata
        
        // Aggiungiamo un po' di luce ambientale
        col += vec3(0.1); 
    }

    // Output finale
    FragColor = vec4(col, 1.0);
}
