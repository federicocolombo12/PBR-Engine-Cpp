#include "Mesh.h"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;
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
    // Invece di usare glDrawArrays, usiamo glDrawElements per dire ad OpenGL di usare l'EBO!
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
