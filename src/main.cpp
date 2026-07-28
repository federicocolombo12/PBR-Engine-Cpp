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

// Importiamo la matematica 3D e la nostra Camera
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Model.h"

// Variabili globali per la Telecamera e il Mouse (devono essere globali per la callback)
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0f; // Centro dello schermo (X)
float lastY = 600.0f / 2.0f; // Centro dello schermo (Y)
bool firstMouse = true;

// Callback: viene chiamata automaticamente quando l'utente ridimensiona la finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Diciamo a OpenGL di aggiornare le coordinate di disegno (Viewport)
    glViewport(0, 0, width, height);
}

// Callback: viene chiamata automaticamente quando il mouse si muove
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // Al primo avvio, evitiamo sbalzi improvvisi della telecamera
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Quanto si è mosso il mouse rispetto all'ultimo frame?
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertito perché le Y in OpenGL vanno dal basso verso l'alto

    lastX = xpos;
    lastY = ypos;

    // Inviamo i dati alla Camera
    camera.ProcessMouseMovement(xoffset, yoffset);
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
    
    // Registriamo le funzioni di callback per il resize della finestra e per il mouse
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Catturiamo il cursore del mouse (lo nascondiamo e lo blocchiamo dentro la finestra)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    std::cout << "Finestra creata con successo! Preparo la GPU..." << std::endl;

    // === STEP 4: PREPARIAMO IL FULLSCREEN QUAD ===
    // Per fare Raymarching non passiamo modelli 3D. Creiamo un "quadrato" piatto 
    // composto da 2 triangoli che copre tutto lo schermo.
    // Le coordinate normalizzate in OpenGL vanno da -1.0 (sinistra/basso) a +1.0 (destra/alto)
    
     // I 6 vertici (2 triangoli) del nostro vecchio "Fullscreen Quad" 2D
    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f,
        -1.0f, -1.0f
    };

    // --- SETUP QUAD (SDF) ---
    unsigned int quadVBO, quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // === CARICHIAMO I NOSTRI SHADER ===
    Shader sdfShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    Shader rasterShader("assets/shaders/raster.vert", "assets/shaders/raster.frag");

    // === CARICHIAMO IL MODELLO 3D CON ASSIMP ===
    std::cout << "Caricamento modello in corso..." << std::endl;
    Model myModel("assets/models/teapot.obj");
    std::cout << "Modello caricato!" << std::endl;

    // Abilitiamo il Depth Testing (Z-Buffer)! FONDAMENTALE PER IL 3D.
    glEnable(GL_DEPTH_TEST);

    // Variabili per il Toggle
    bool useRasterizer = true;
    bool spacePressed = false;

    // Variabili per il Delta Time
    float deltaTime = 0.0f; // Quanto tempo è passato dall'ultimo frame
    float lastFrame = 0.0f; // Il tempo assoluto al frame precedente

    // 4. Il "Game Loop" (Render Loop)
    // Continua a girare finché qualcuno non dice alla finestra di chiudersi
    while (!glfwWindowShouldClose(window)) {
        
        // --- CALCOLO DEL DELTA TIME ---
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- INPUT BASE ---
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
            
        // --- INPUT: MOVIMENTO CAMERA (WASD) ---
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        // --- INPUT: IL TOGGLE ---
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (!spacePressed) {
                useRasterizer = !useRasterizer; // Inverte lo stato
                spacePressed = true;
            }
        } else {
            spacePressed = false;
        }

        // --- RENDER ---
        // Scegliamo il colore di "pulizia"
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        // Puliamo sia il Colore che lo Z-Buffer (Depth)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (useRasterizer) {
            // == PIPELINE 3D RASTERIZER ==
            rasterShader.use();

            // Calcoliamo la Projection Matrix (FOV 45 gradi, aspect ratio, near/far clip plane)
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
            
            // La View Matrix ci viene data dalla Camera
            glm::mat4 view = camera.GetViewMatrix();
            
            // La Model Matrix: ruotiamo e rimpiccioliamo leggermente il modello
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // Abbassiamo un po'
            model = glm::scale(model, glm::vec3(0.2f)); // Rimpiccioliamo
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotazione continua

            // Passiamo le matrici allo shader
            rasterShader.setMat4("projection", projection);
            rasterShader.setMat4("view", view);
            rasterShader.setMat4("model", model);

            // Disegniamo il nostro vero modello 3D!
            myModel.Draw(rasterShader);

        } else {
            // == PIPELINE 2D RAYMARCHER (SDF) ==
            sdfShader.use();
            
            // Passiamo i dati dinamici
            sdfShader.setFloat("iTime", glfwGetTime());
            sdfShader.setVec2("iResolution", 800.0f, 600.0f);

            // Disegniamo il quad 2D a tutto schermo
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

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
