#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

// Struttura dati per un singolo vertice. Al posto di usare float crudi, usiamo i glm::vec!
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // Nuovi attributi per il Normal Mapping (Spazio Tangente)
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

// Rappresenta una texture caricata in memoria
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

// Una Mesh è un singolo "pezzo" di geometria solida, con il suo VBO, VAO e l'EBO (Index Buffer)
class Mesh {
public:
    // Dati della Mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Costruttore
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    
    // Funzione per inviare i dati alla GPU e disegnare questa mesh
    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;
    
    // Inizializza i buffer di OpenGL
    void setupMesh();
};
