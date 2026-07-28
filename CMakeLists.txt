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

# Raccogliamo tutti i file .cpp nella cartella src
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Creiamo l'eseguibile del nostro Raymarcher
add_executable(Raymarcher ${SRC_FILES})

# === MAC OS SPECIFIC ===
# Cerchiamo il framework OpenGL nativo di Mac
find_library(OPENGL_FRAMEWORK OpenGL)

# Colleghiamo l'eseguibile alle librerie
target_link_libraries(Raymarcher PRIVATE glfw ${OPENGL_FRAMEWORK})
target_include_directories(Raymarcher PRIVATE src)
