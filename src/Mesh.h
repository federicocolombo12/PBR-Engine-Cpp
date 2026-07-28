#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

// Struttura dati per un singolo vertice. Al posto di usare float crudi, usiamo i glm::vec!
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords; // UV
};

// Una Mesh è un singolo "pezzo" di geometria solida, con il suo VBO, VAO e l'EBO (Index Buffer)
class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    
    // Funzione per inviare i dati alla GPU e disegnare questa mesh
    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;
    
    // Inizializza i buffer di OpenGL
    void setupMesh();
};
