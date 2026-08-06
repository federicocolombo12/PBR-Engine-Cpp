#pragma once

// Includiamo GLM, la libreria standard per la matematica 3D in OpenGL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Un semplice enum per non dover passare stringhe o numeri magici
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Questa classe incapsulerà la "View Matrix" (dove si trova e dove guarda la camera)
// e ci permetterà in futuro di muoverci nel mondo 3D con WASD e il mouse.
class Camera {
public:
    // Attributi spaziali
    glm::vec3 Position;
    glm::vec3 Front; // Direzione verso cui stiamo guardando
    glm::vec3 Up;    // Il vettore "Sopra"
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Angoli di Eulero per la rotazione (stile mouse FPS)
    float Yaw;
    float Pitch;

    // Velocità di movimento della telecamera
    float MovementSpeed;
    // Sensibilità del mouse
    float MouseSensitivity;

    // Costruttore: di default posizioniamo la camera leggermente indietro (Z = 3)
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f));

    // Restituisce la matrice magica che trasforma il mondo in base al nostro punto di vista
    glm::mat4 GetViewMatrix();

    // Processa l'input della tastiera e aggiorna la Posizione
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // Processa il movimento del mouse per ruotare la visuale
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

private:
    // Ricalcola i vettori Front, Right e Up in base a Yaw e Pitch
    void updateCameraVectors();
};
