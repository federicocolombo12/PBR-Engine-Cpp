#include "Mesh.h"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::setupMesh() {
    // Generiamo Etichetta (VAO), Secchio dei Vertici (VBO) e Secchio degli Indici (EBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    // Versiamo i vertici nel VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Versiamo gli indici (Element Buffer) nell'EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // SPIEGHIAMO AD OPENGL COME LEGGERE LA STRUCT 'Vertex':
    
    // Attributo 0: Posizione (x, y, z)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Attributo 1: Normali (x, y, z). offsetof dice a C++ "Quanti byte ci sono dall'inizio della struct fino a 'Normal'?"
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    // Attributo 2: Coordinate UV (u, v)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Pulizia
    glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) {
    // 1. Colleghiamo tutte le texture necessarie prima del render
    shader.setBool("hasTexture", textures.size() > 0);
    
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        // Attiviamo l'unità di texture corretta (GL_TEXTURE0, GL_TEXTURE1, ecc.)
        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
            
        // Diciamo allo shader quale unità di texture usare (es: "texture_diffuse1" -> 0)
        shader.setInt((name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    // Ripristiniamo l'unità 0 per sicurezza
    glActiveTexture(GL_TEXTURE0);

    // 2. Disegniamo la geometria
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
