#pragma once
#include <string>
#include <glm/glm.hpp>

// La nostra classe Shader gestirà la compilazione del codice grafico sulla GPU
class Shader {
public:
    // L'ID numerico che OpenGL assegna a questo programma shader
    unsigned int ID;

    // Il costruttore ora prenderà i PERCORSI dei file, aprirà i file di testo e li leggerà
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // Attiva questo shader per usarlo nei prossimi Draw Calls
    void use();

    // Funzioni di utility per passare Uniform (variabili dalla CPU alla GPU)
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    // Funzione interna per controllare se la GPU ci restituisce errori di sintassi
    void checkCompileErrors(unsigned int shader, std::string type);
};
