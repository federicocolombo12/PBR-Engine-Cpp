cmake_minimum_required(VERSION 3.10)
project(GraphicsLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Scarichiamo GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Scarichiamo GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
endif()

# Scarichiamo Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.1 
)
FetchContent_MakeAvailable(assimp)

find_package(OpenGL REQUIRED)

# === LIBRERIA CORE ===
# Raggruppiamo tutte le classi base (Camera, Shader, Mesh, Model) in una libreria invisibile
file(GLOB_RECURSE CORE_SRC "src/core/*.cpp")
add_library(CoreLib STATIC ${CORE_SRC})
target_link_libraries(CoreLib PUBLIC glfw assimp OpenGL::GL)
target_include_directories(CoreLib PUBLIC src src/core ${glm_SOURCE_DIR})

# === ESPERIMENTO 1: RAYMARCHER SDF ===
add_executable(01_Raymarcher "src/experiments/01_Raymarcher/main.cpp")
target_link_libraries(01_Raymarcher PRIVATE CoreLib)

# === ESPERIMENTO 2: DEFERRED RENDERING ===
add_executable(02_Deferred src/experiments/02_Deferred/main.cpp)
target_link_libraries(02_Deferred PRIVATE CoreLib)

# === ESPERIMENTO 3: PBR (PHYSICALLY BASED RENDERING) ===
add_executable(03_PBR src/experiments/03_PBR/main.cpp)
target_link_libraries(03_PBR PRIVATE CoreLib)

# === ESPERIMENTO 4: SILHOUETTE POM ===
add_executable(04_POM src/experiments/04_POM/main.cpp)
target_link_libraries(04_POM PRIVATE CoreLib)
