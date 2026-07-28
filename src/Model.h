#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <vector>
#include <iostream>

// Il Model è un "Gruppo" che contiene una o più Mesh e le gestisce.
// È l'equivalente del Prefab Root in Unity.
class Model {
public:
    std::vector<Mesh> meshes;

    // Passiamo il percorso del file .obj o .fbx
    Model(std::string path);
    void Draw(Shader &shader);

private:
    void loadModel(std::string path);
    // Funzioni ricorsive per navigare nell'albero di Assimp
    void processNode(aiNode *node, const aiScene *scene);
    // Converte una mesh di Assimp in una nostra Mesh
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};
