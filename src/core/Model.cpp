#include "Model.h"
#include <iostream>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include "stb_image.h"

// Funzione helper per caricare i file immagine su GPU (Texture Loader Universale)
unsigned int TextureFromFile(const char *path, const std::string &directory) {
    // IGINEZZATORE DI PERCORSI (Path Sanitizer)
    std::string rawPath = std::string(path);
    // Sostituiamo eventuali backslash di Windows con gli slash di Unix/Mac
    std::replace(rawPath.begin(), rawPath.end(), '\\', '/');
    // Strappiamo via tutte le cartelle originali e teniamo solo il nome del file (es. "Cerberus_A.tga")
    std::string filename = rawPath.substr(rawPath.find_last_of('/') + 1);

    // Sistema di Fallback a 3 livelli:
    // 1. Proviamo il percorso crudo (per i modelli vecchi o fatti benissimo)
    std::string try1 = directory + '/' + rawPath;
    // 2. Forza la ricerca nella cartella "textures/" che sta di fianco a "source/"
    std::string try2 = directory + "/../textures/" + filename;
    // 3. Molti asset convertiti online prendono il .tga e gli appiccicano il .png. Proviamolo!
    std::string try3 = directory + "/../textures/" + filename + ".png";

    int width, height, nrComponents;
    unsigned char *data = nullptr;
    std::string loadedPath = "";

    // Tentativo 1
    data = stbi_load(try1.c_str(), &width, &height, &nrComponents, 0);
    if(data) loadedPath = try1;
    
    // Tentativo 2
    if (!data) {
        data = stbi_load(try2.c_str(), &width, &height, &nrComponents, 0);
        if(data) loadedPath = try2;
    }

    // Tentativo 3
    if (!data) {
        data = stbi_load(try3.c_str(), &width, &height, &nrComponents, 0);
        if(data) loadedPath = try3;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    if (data) {
        GLenum format = GL_RGBA;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "ERRORE TEXTURE CRITICO: Impossibile trovare la texture -> " << filename << std::endl;
        std::cout << "Provati i seguenti percorsi:\n1. " << try1 << "\n2. " << try2 << "\n3. " << try3 << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

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
    // aiProcess_FlipUVs: Capovolge l'asse Y delle texture
    // aiProcess_GenSmoothNormals: SE il modello 3D non ha le normali salvate nel file, le calcola matematicamente!
    // aiProcess_CalcTangentSpace: Calcola la matrice TBN per il Normal Mapping
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERRORE ASSIMP: Impossibile caricare il modello! " << importer.GetErrorString() << std::endl;
        return;
    }
    this->directory = path.substr(0, path.find_last_of('/'));

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
        
        // Tangenti e Bitangenti (per il Normal Mapping)
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        } else {
            vertex.Tangent = glm::vec3(0.0f);
            vertex.Bitangent = glm::vec3(0.0f);
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
    
    // Novità: Processiamo i materiali (Texture)
    std::vector<Texture> textures;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. Mappa Diffuse (il colore base)
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        // 2. Normal Map (Per i dettagli in rilievo reali)
        // Nota: Alcuni formati salvano le Normal in aiTextureType_HEIGHT, noi cerchiamo NORMALS.
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 3. Metallic (È metallo o plastica?)
        std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "texture_metallic");
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

        // 4. Roughness (Quanto è ruvido/opaco?)
        std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

        // 5. Ambient Occlusion (Micro-ombre)
        std::vector<Texture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

        // 6. Height Map / Displacement Map (Per POM e Tassellazione)
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_height");
        if(heightMaps.empty()) {
            // Alcuni formati (es. OBJ classici) salvano le mappe di spostamento/bump sotto HEIGHT
            heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
        }
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
        
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        bool skip = false;
        // Controlliamo se l'abbiamo già caricata (ottimizzazione)
        for(unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // Aggiungiamo alla cache
        }
    }
    return textures;
}
