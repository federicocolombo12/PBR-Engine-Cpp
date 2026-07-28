# Custom C++ PBR Engine & Raymarcher

Questo è un motore grafico personalizzato scritto in **C++ puro** e **OpenGL 3.3 Core**. 
Nasce come esperimento didattico per esplorare le architetture di rendering a basso livello, allontanandosi dalle astrazioni dei motori commerciali (come Unity o Unreal Engine) per avere il controllo completo sull'hardware (CPU/GPU).

## Architettura Multi-Pipeline
Il motore supporta lo switch dinamico (a runtime) tra due diverse pipeline di rendering:
1. **Raymarching Pipeline (SDF):** Un rendering interamente procedurale calcolato nel Fragment Shader su un Fullscreen Quad. Utilizzato per generare forme matematiche (Signed Distance Fields) senza alcuna geometria 3D.
2. **Rasterizer Pipeline (PBR):** *[Work in Progress]* Una pipeline classica di rasterizzazione per modelli 3D (`.obj`), che implementerà il Deferred Rendering e l'illuminazione PBR (Physically Based Rendering).

## Come Compilare (macOS)
Il progetto utilizza CMake come build system e scarica automaticamente le librerie necessarie (come GLFW e GLM).

```bash
# Genera i file di configurazione
cmake -B build

# Compila l'eseguibile
cmake --build build

# Lancia il motore
./build/Raymarcher
```

*Progetto sviluppato come portfolio e approfondimento tecnico nel campo del Graphics Programming e del Technical Art.*
