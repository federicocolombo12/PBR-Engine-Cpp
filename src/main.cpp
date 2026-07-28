#include <iostream>

// === SETUP SPECIFICO PER MAC OS ===
// Evita che il compilatore si lamenti che OpenGL è deprecato su Mac
#define GL_SILENCE_DEPRECATION
// Importa le funzioni native di OpenGL 3.3 Core (sostituisce GLAD)
#include <OpenGL/gl3.h>

// Importa GLFW per gestire finestra e input
#include <GLFW/glfw3.h>

// Importiamo la nostra nuova fantastica Classe Shader
#include "Shader.h"

// Callback: viene chiamata automaticamente quando l'utente ridimensiona la finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Diciamo a OpenGL di aggiornare le coordinate di disegno (Viewport)
    glViewport(0, 0, width, height);
}

int main() {
    // 1. Inizializziamo GLFW
    if (!glfwInit()) {
        std::cerr << "Errore: impossibile inizializzare GLFW!" << std::endl;
        return -1;
    }

    // 2. Configuriamo il contesto OpenGL (versione 3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Essenziale per Mac

    // 3. Creiamo la finestra (Larghezza, Altezza, Titolo)
    GLFWwindow* window = glfwCreateWindow(800, 600, "C++ Raymarcher - Tech Art", nullptr, nullptr);
    if (!window) {
        std::cerr << "Errore: impossibile creare la finestra!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Rendiamo la finestra appena creata il "contesto corrente" su cui disegnerà OpenGL
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::cout << "Finestra creata con successo! Preparo la GPU..." << std::endl;

    // === STEP 4: PREPARIAMO IL FULLSCREEN QUAD ===
    // Per fare Raymarching non passiamo modelli 3D. Creiamo un "quadrato" piatto 
    // composto da 2 triangoli che copre tutto lo schermo.
    // Le coordinate normalizzate in OpenGL vanno da -1.0 (sinistra/basso) a +1.0 (destra/alto)
    float quadVertices[] = {
        // x      y      (Coordinate dei vertici)
        -1.0f,  1.0f,    // Triangolo 1: alto-sinistra
        -1.0f, -1.0f,    // Triangolo 1: basso-sinistra
         1.0f, -1.0f,    // Triangolo 1: basso-destra

        -1.0f,  1.0f,    // Triangolo 2: alto-sinistra
         1.0f, -1.0f,    // Triangolo 2: basso-destra
         1.0f,  1.0f     // Triangolo 2: alto-destra
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // Crea un "Etichetta" per i dati
    glGenBuffers(1, &VBO);      // Crea il "Secchio" di memoria (VBO) sulla VRAM della scheda video
    
    // 1. Diciamo a OpenGL di "Lavorare" su questo VAO
    glBindVertexArray(VAO);

    // 2. Apriamo il secchio (VBO) e ci versiamo dentro il nostro array 'quadVertices'
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // 3. Compiliamo l'etichetta (VAO): "Ehi GPU, i dati nel secchio sono gruppi di 2 float (x,y)"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // === CARICHIAMO I NOSTRI SHADER ===
    // Grazie alla nostra classe, tutto il boilerplate di OpenGL sparisce in una riga!
    Shader mioShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    // 4. Il "Game Loop" (Render Loop)
    // Continua a girare finché qualcuno non dice alla finestra di chiudersi
    while (!glfwWindowShouldClose(window)) {
        
        // --- GESTIONE INPUT ---
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // --- RENDER ---
        // Scegliamo il colore di "pulizia"
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Attiviamo il nostro shader (il programma compilato)
        mioShader.use();
        
        // Passiamo i dati dinamici (Uniform) dalla CPU alla GPU!
        // glfwGetTime() restituisce i secondi trascorsi dall'avvio
        mioShader.setFloat("iTime", glfwGetTime());
        // Passiamo la risoluzione della finestra per calcolare l'aspect ratio
        mioShader.setVec2("iResolution", 800.0f, 600.0f);

        // Disegniamo la geometria:
        // "Usa l'etichetta VAO e disegna 6 vertici usando la modalità Triangoli"
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- SWAP BUFFERS & POLL EVENTS ---
        // OpenGL usa il "Double Buffering". Disegna su un'immagine nascosta, poi la scambia 
        // con quella visibile. Questo evita artefatti (screen tearing).
        glfwSwapBuffers(window);
        
        // Controlla il mouse e la tastiera
        glfwPollEvents();
    }

    // 5. Uscita pulita: distruggiamo tutto e liberiamo la memoria
    glfwTerminate();
    return 0;
}
