# Graphics Lab

A personal research environment built in **C++** and **OpenGL** where I implement modern rendering techniques, geometry processing algorithms, and real-time graphics experiments from scratch.

This project is a deliberate departure from commercial engines (Unity/Unreal). Its goal is to understand the math, the architecture, and the GPU pipeline behind modern computer graphics, building a custom rendering framework tailored for specific experiments.

## 🛠 Tech Stack
- **Language:** C++17
- **Graphics API:** OpenGL 3.3 Core Profile
- **Window & Input:** GLFW
- **Math:** GLM (OpenGL Mathematics)
- **Asset Loading:** Assimp (Open Asset Import Library)
- **Build System:** CMake (Cross-platform support)

## 🗺 Roadmap & Devlog
The project is strictly developed *one feature at a time*, following a rigorous documentation approach.
- See the [ROADMAP_GRAPHICS_LAB.md](ROADMAP_GRAPHICS_LAB.md) for the technical trajectory (Deferred Rendering, PBR, Shadow Mapping, SDF, Compute).
- See the [PROJECT_MEMORY.md](PROJECT_MEMORY.md) for the internal Devlog and architectural choices behind every single feature implemented.

## 🚀 How to Build
This project uses a standard, cross-platform CMake configuration. It will automatically download dependencies like Assimp and GLM using `FetchContent`.

```bash
# 1. Clone the repository
git clone https://github.com/federicocolombo12/PBR-Engine-Cpp.git
cd PBR-Engine-Cpp

# 2. Configure the build
cmake -B build

# 3. Compile
cmake --build build

# 4. Run the lab
./build/Raymarcher
```

*(Note: Assimp compilation during the first build might take a few minutes).*
