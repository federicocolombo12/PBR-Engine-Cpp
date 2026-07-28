#include "Model.h"

Model::Model(std::string path) {
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    // Disegniamo ogni sotto-pezzo (Mesh) che compone questo Modello
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    // Leggiamo il file. 
    // aiProcess_Triangulate: Forza Assimp a dividere tutti i poligoni strani in Triangoli (GPU amichevole)
    // aiProcess_FlipUVs: Capovolge l'asse Y delle texture (OpenGL legge dal basso, molti sw 3D dall'alto)
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERRORE ASSIMP: Impossibile caricare il modello! " << importer.GetErrorString() << std::endl;
        return;
    }

    // Partiamo dalla radice dell'albero e scendiamo giù
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // 1. Trasformiamo le Mesh attaccate a questo Nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // 2. Facciamo lo stesso per tutti i Nodi Figli (Ricorsione)
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // --- 1. LETTURA DEI VERTICI ---
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        // Posizione
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        
        // Normale (Se il file 3D ha i dati di smoothing)
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        } else {
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        
        // Coordinate UV (Texture)
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        
        vertices.push_back(vertex);
    }

    // --- 2. LETTURA DEGLI INDICI (EBO) ---
    // Assimp raggruppa gli indici in Facce (Faces). Siccome abbiamo usato aiProcess_Triangulate, ogni Faccia è un Triangolo (3 indici).
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Ritorniamo una nostra Mesh nativa
    return Mesh(vertices, indices);
}
