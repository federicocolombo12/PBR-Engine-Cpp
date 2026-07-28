cmake_minimum_required(VERSION 3.15)
project(Raymarcher CXX)

# Genera compile_commands.json per aiutare gli strumenti di sviluppo (come VS Code) a trovare gli header
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Richiediamo C++17 (lo standard minimo per lo sviluppo giochi moderno)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Questo modulo ci permette di scaricare librerie da internet (Github) direttamente dal build system
include(FetchContent)

# Diciamo a CMake dove trovare GLFW (la libreria per gestire Finestra e Input)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
# Spegniamo la build di cose inutili di GLFW per velocizzare
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Scarichiamo GLM (OpenGL Mathematics), lo standard dell'industria per le matrici 3D
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
# Poiché GLM è "Header-Only" e il suo CMake è vecchio, scarichiamo i file senza compilarli
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
endif()

# Scarichiamo Assimp (Open Asset Import Library) per caricare modelli 3D (.obj, .fbx)
# Disattiviamo la compilazione dei suoi test e tool per risparmiare tempo di build
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.1 # Ultima versione stabile 
)
FetchContent_MakeAvailable(assimp)

# Raccogliamo tutti i file .cpp nella cartella src
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Creiamo l'eseguibile del nostro Raymarcher
add_executable(Raymarcher ${SRC_FILES})

# === MAC OS SPECIFIC ===
# Cerchiamo OpenGL in modo universale (funziona su Mac, Windows, Linux)
find_package(OpenGL REQUIRED)

# Colleghiamo l'eseguibile alle librerie: glfw (finestra), assimp (modelli), OpenGL (grafica)
target_link_libraries(Raymarcher PRIVATE glfw assimp OpenGL::GL)
target_include_directories(Raymarcher PRIVATE src ${glm_SOURCE_DIR})
