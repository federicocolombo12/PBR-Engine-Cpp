#include "Camera.h"

Camera::Camera(glm::vec3 position) {
    Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f); // L'asse Y globale va sempre verso l'alto
    Yaw = -90.0f; // Di base guardiamo lungo l'asse Z negativo (dentro lo schermo)
    Pitch = 0.0f;
    MovementSpeed = 2.5f; // Velocità in metri al secondo
    MouseSensitivity = 0.1f; // Moltiplicatore per il movimento del mouse
    
    // Ricalcola i vettori direzionali in base agli angoli iniziali
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    // glm::lookAt è la funzione fondamentale: crea una matrice di "vista" 
    // dati 3 parametri: Da dove guardo, Cosa guardo, Qual è la direzione "Su"
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    // La formula fisica per lo spazio percorso: Spazio = Velocità * Tempo
    float velocity = MovementSpeed * deltaTime;
    
    // Se andiamo avanti, sommiamo alla posizione attuale il vettore Front scalato per la velocità
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // Assicuriamoci che quando guardiamo in su/giù non ribaltiamo la telecamera
    // (evitiamo il famoso Gimbal Lock limitando il pitch a 89 gradi)
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Dopo aver cambiato gli angoli, ricalcoliamo i vettori Front, Right, Up
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Calcoliamo la nuova direzione Front usando un po' di trigonometria sferica
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    // Normalizziamo i vettori (li rendiamo di lunghezza 1)
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp)); // Prodotto vettoriale
    Up    = glm::normalize(glm::cross(Right, Front));
}
