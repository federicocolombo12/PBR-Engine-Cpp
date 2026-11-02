# Graphics Lab: Custom C++17 & OpenGL Rendering Engine 🧪

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=flat&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![OpenGL](https://img.shields.io/badge/Graphics_API-OpenGL_3.3_%2F_4.6_Core-5586A4?style=flat&logo=opengl)](https://www.opengl.org/)
[![GLSL](https://img.shields.io/badge/Shading-GLSL_330_Core-4584b6?style=flat)](https://www.khronos.org/opengl/wiki/Core_Language_(GLSL))
[![CMake](https://img.shields.io/badge/Build_System-CMake_3.20+-064F8C?style=flat&logo=cmake)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-macOS_%7C_Linux_%7C_Windows-lightgrey?style=flat)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A modular, low-level real-time rendering engine and shader research laboratory built from scratch in **C++17** and **OpenGL Core Profile**. 

### 🎯 The Philosophy: Playing with Graphics to Bridge the Engine Gap
As a Technical Artist and Graphics Programmer who works daily with modern production engines like **Unreal Engine 5** and **Unity**—authoring procedural PCG graphs, MetaHuman character pipelines, and complex shaders—I didn't want to just consume high-level abstractions without truly mastering the low-level machinery underneath. 

I built this laboratory as a hands-on playground to **play with graphics from the bare metal up**: to dismantle the "black box", sharpen my C++ and GPU programming skills, and deeply understand the exact principles behind the features I deploy in modern game engines—how a G-Buffer serializes into VRAM, how microfacets scatter light according to the Cook-Torrance BRDF, and how driver-level state machines behave when you don't have an engine safety net.

---

## 🏗️ Architecture & Engine Layout

The engine is engineered around a clean separation between reusable core runtime systems (`src/core/`) and isolated experimental targets (`src/experiments/`):

```
PBR-Engine-Cpp/
├── CMakeLists.txt                # Automated FetchContent dependency management
├── PROJECT_MEMORY.md             # Formal devlog recording problems, math & bug fixes
├── ROADMAP_GRAPHICS_LAB.md       # Technical trajectory & milestone tracking
├── assets/
│   ├── models/                   # Complex multi-material FBX/OBJ test meshes (Cerberus, Rocks)
│   └── shaders/
│       ├── deferred/             # G-Buffer generation & deferred lighting shaders
│       ├── pbr/                  # Cook-Torrance BRDF microfacet lighting evaluation
│       ├── pom/                  # Tangent Space Silhouette Parallax Occlusion Mapping
│       └── raymarcher/           # Fullscreen analytical SDF sphere-tracer
├── references/
│   └── Tatarchuk_POM_SIGGRAPH2006.pdf # Foundation paper for Silhouette POM
└── src/
    ├── core/
    │   ├── Camera.cpp / .h       # 6DOF camera with hardware pitch clamp (Gimbal lock fix)
    │   ├── Mesh.cpp / .h         # VAO/VBO/EBO encapsulation & draw calls
    │   ├── Model.cpp / .h        # Assimp integration with path & normal sanitizers
    │   ├── Shader.cpp / .h       # GLSL compilation, linking & uniform reflection
    │   └── stb_image.cpp / .h    # Texture ingestion & sRGB space handling
    └── experiments/
        ├── 01_Raymarcher/        # Fullscreen quad analytical SDF fractal sandbox
        ├── 02_Deferred/          # Multi-Render Target (MRT) G-Buffer geometry pipeline
        ├── 03_PBR/               # Packed G-Buffer with Cook-Torrance microfacet BRDF
        └── 04_POM/               # Silhouette Parallax Occlusion Mapping on low-poly quads
```

---

## ⚡ Key Technical Features

### 1. Two-Pass Deferred Shading & G-Buffer VRAM Packing
Traditional Forward Rendering evaluates lighting inside geometry draw calls, scaling to $O(N \times M)$ complexity for $N$ overlapping fragments and $M$ dynamic lights. The engine splits rendering into:
1. **Geometry Pass:** Rasterizes geometry into a Multi-Render Target (**MRT**) Framebuffer without computing lighting ($O(N)$).
2. **Lighting Pass:** Renders a single screen-space quad, sampling the G-Buffer textures to evaluate lighting exclusively on visible fragments ($O(M)$), eliminating overdraw penalties.

#### Channel Packing Scheme (VRAM Bandwidth Optimization)
Rather than allocating 6 separate textures, the engine packs 6 physical surface properties into just **3 high-precision `GL_RGBA16F` textures**, cutting memory bandwidth consumption by ~33%:

| Framebuffer Target | Format | RGB Channels | Alpha Channel (`.a`) | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| **`gPositionAO`** (`RT0`) | `GL_RGBA16F` | World Position (`FragPos.xyz`) | **Ambient Occlusion (`ao`)** | Reconstructs 3D position & cavity occlusion |
| **`gNormalRoughness`** (`RT1`) | `GL_RGBA16F` | Surface Normal (`N.xyz`) | **Roughness (`roughness`)** | Microfacet orientation & specular blur |
| **`gAlbedoMetallic`** (`RT2`) | `GL_RGBA16F` | Linear Albedo (`albedo.rgb`) | **Metallic (`metallic`)** | Color reflectance & conductor absorption |

```glsl
// Unpacking in pbr_light.frag:
vec4 posAO       = texture(gPositionAO, TexCoords);
vec3 FragPos     = posAO.rgb;
float ao         = posAO.a;

vec4 normRough   = texture(gNormalRoughness, TexCoords);
vec3 N           = normalize(normRough.rgb);
float roughness  = normRough.a;

vec4 albedoMetal = texture(gAlbedoMetallic, TexCoords);
vec3 albedo      = pow(albedoMetal.rgb, vec3(2.2)); // Linearize sRGB textures
float metallic   = albedoMetal.a;
```

---

### 2. Physically Based Rendering (Cook-Torrance BRDF)
Implements the microfacet model of specular reflection based on the physics of light-matter interaction:

$$f_r = k_d \frac{c}{\pi} + \frac{D \cdot F \cdot G}{4(\vec{n} \cdot \vec{\omega}_o)(\vec{n} \cdot \vec{\omega}_i)}$$

- **$D$ — Normal Distribution Function:** Trowbridge-Reitz GGX with Disney's $\alpha = \text{roughness}^2$ remapping.
- **$G$ — Geometric Attenuation:** Smith's Schlick-GGX formulation accounting for micro-occlusion and light shadowing.
- **$F$ — Fresnel Reflectivity:** Fresnel-Schlick approximation with dielectric baseline $F_0 = 0.04$ tinted by Albedo for metallic conductors ($F_0 = \text{mix}(0.04, \text{albedo}, \text{metallic})$).
- **Energy Conservation:** $k_D = (1.0 - k_S) \times (1.0 - \text{metallic})$, guaranteeing that metals absorb refracted light into their conduction band.

---

### 3. Silhouette Parallax Occlusion Mapping (SPOM)
Direct implementation of **Natalya Tatarchuk's SIGGRAPH 2006 paper** on real-time displacement approximations:
- **Tangent Space Inversion:** Transformed camera view vector into local Tangent Space via the inverse TBN matrix ($\text{TBN}^T$).
- **Dynamic LOD Raymarching:** Dynamically scales ray steps between $8$ (normal viewing) and $32$ (grazing angles) based on $\text{dot}(\vec{N}, \vec{V})$ to preserve framerate.
- **Sub-Pixel Linear Refinement:** Computes linear depth interpolation between pre-collision and post-collision samples to eliminate stepping artifacts.
- **Silhouette Boundary Clipping:** Fragments that displace outside the UV interval $[0.0, 1.0]$ are eliminated via `discard`, giving flat polygon boundaries authentic geometric silhouettes.

```glsl
// Silhouette Clipping in g_buffer_pom.frag:
vec2 finalTexCoords = ParallaxMapping(TexCoords, viewDir);
if (finalTexCoords.x > 1.0 || finalTexCoords.y > 1.0 || 
    finalTexCoords.x < 0.0 || finalTexCoords.y < 0.0) {
    discard;
}
```

---

### 4. Analytical Raymarching & SDFs
A dedicated screen-space raymarching target (`01_Raymarcher`) evaluating mathematical Signed Distance Fields (SDFs):
- **Sphere Tracing:** Dynamically step rays by the exact value of the scene's global distance field.
- **Constructive Solid Geometry (CSG):** Real-time union, intersection, and subtraction of procedural 3D primitives.
- **Numerical Gradient Normals:** Surface normals extracted using the central tetrahedron technique ($f(\vec{p} \pm \epsilon)$) without stored vertex buffers.

---

## 🛠️ Post-Mortem: Real-World Engineering & Debugging

Every milestone followed a strict engineering discipline documented in [`PROJECT_MEMORY.md`](PROJECT_MEMORY.md):

1. **Camera Gimbal Lock Singularity:**
   - *Problem:* When Pitch reached $\pm 90.0^\circ$, `glm::cross(Front, Up)` degenerated to $(0,0,0)$, causing View Matrix collapse.
   - *Fix:* Enforced a strict hardware clamp in `Camera.cpp` at $\pm 89.0^\circ$.
2. **Missing Mesh Normals & NaN Artifacts:**
   - *Problem:* Multi-material assets lacking normal data produced zero vectors in vertex attributes; calling `normalize(vec3(0.0))` resulted in `NaN` black fragments.
   - *Fix:* Integrated `aiProcess_GenSmoothNormals` inside the Assimp import flags to reconstruct smooth normals from topology.
3. **OpenGL State Leakage (Rainbow Textures):**
   - *Problem:* Sub-meshes lacking textures sampled residual World Position attachments from `GL_TEXTURE0`, displaying 3D world coordinates as psychedelic colors.
   - *Fix:* Implemented clean texture unit unbinding post-draw and added an in-shader fallback uniform (`hasTexture`).
4. **FBX Absolute Path Sanitizer:**
   - *Problem:* 3D models embedded hardcoded Windows paths (`D:\Art\Textures\...`), causing runtime I/O exceptions on macOS/Linux.
   - *Fix:* Authored a path sanitizer in `Model.cpp` that extracts filenames and redirects lookups to local relative paths.
5. **GPU Profiling with RenderDoc:**
   - Verified draw call dispatch, monitored G-Buffer attachment state, and confirmed zero VRAM leaks across frames.

---

## 🚀 How to Build & Run

### Prerequisites
- **CMake 3.20+**
- **C++17 Compiler** (Clang / GCC / MSVC)
- **OpenGL 3.3+ Compatible GPU & Drivers**

### Build Instructions
Dependencies (**Assimp**, **GLFW**, **GLM**, **stb_image**) are downloaded and configured automatically via CMake `FetchContent`:

```bash
# 1. Clone the repository
git clone https://github.com/federicocolombo12/PBR-Engine-Cpp.git
cd PBR-Engine-Cpp

# 2. Configure build directory
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Compile all targets
cmake --build build -j8
```

### Running the Experiments
Each milestone compiles into a standalone executable in the `build/` directory:

```bash
# Run the Analytical SDF Raymarcher
./build/01_Raymarcher

# Run the Two-Pass Deferred Shading Engine
./build/02_Deferred

# Run the Cook-Torrance PBR Engine
./build/03_PBR

# Run Silhouette Parallax Occlusion Mapping
./build/04_POM
```

---

## 🗺️ Roadmap

- [x] Analytical SDF Raymarcher & 2D Quad Pipeline
- [x] 6DOF Camera System with Gimbal Lock Protection
- [x] Assimp Asset Loader with Sanitization & Normal Generation
- [x] Multi-Render Target (MRT) G-Buffer Deferred Shading
- [x] Packed G-Buffer Architecture (3x RGBA16F Textures)
- [x] Cook-Torrance Microfacet BRDF (GGX / Smith / Schlick)
- [x] Silhouette Parallax Occlusion Mapping (Dynamic LOD & Clipping)
- [ ] Image-Based Lighting (IBL) with Split-Sum HDR Convolution
- [ ] Cascaded Shadow Maps (CSM) for Directional Lights
- [ ] Screen Space Ambient Occlusion (SSAO) with Depth-Aware Bilateral Filter
- [ ] Compute Shader FFT Ocean Simulation & GPGPU Particles
- [ ] Hardware-Independent Renderer Abstraction (Vulkan / D3D12)

---

## 📄 References & Acknowledgments
- **Natalya Tatarchuk (SIGGRAPH 2006):** *Dynamic Parallax Occlusion Mapping with Silhouette Approximations* (Included in [`references/`](references/)).
- **Brian Karis (Epic Games, 2013):** *Real Shading in Unreal Engine 4* (Cook-Torrance GGX microfacet formulations).
- **LearnOpenGL & Joey de Vries:** Excellent reference material for OpenGL API architecture.

---

## 📜 License
This project is open-source under the [MIT License](LICENSE).
