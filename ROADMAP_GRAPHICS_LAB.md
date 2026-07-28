# Graphics Lab: Roadmap

A personal research environment where I implement modern rendering techniques, geometry processing algorithms and real-time graphics experiments in C++ / OpenGL.

## Regola d'Oro
**Una feature alla volta.** Niente salti. Si passa alla feature successiva solo quando la precedente è completata, funzionante e rigorosamente documentata nel `PROJECT_MEMORY.md`.

## 1. Rendering Base (Completato)
- [x] Contesto OpenGL, Finestra (GLFW)
- [x] Input Camera (WASD + Mouse Pitch/Yaw)
- [x] Astrazione Mesh e Model
- [x] Integrazione Assimp e caricamento `.obj`

## 2. Architettura Rendering Avanzata (In Corso)
- [ ] **Deferred Rendering:** Implementazione del G-Buffer (Position, Normal, Albedo).
- [ ] **Lighting:** Modello di illuminazione deferito (Point Lights, Directional).
- [ ] **PBR (Physically Based Rendering):** Materiali fisicamente accurati (Cook-Torrance).
- [ ] **Shadow Mapping:** Generazione di mappe d'ombra direzionali.

## 3. Post-Processing & Screen Space
- [ ] **HDR & Tone Mapping:** Gestione dei colori ad alta gamma dinamica.
- [ ] **Bloom:** Estrazione aree luminose e Gaussian blur.
- [ ] **SSAO (Screen Space Ambient Occlusion):** Ombre di contatto.

## 4. Geometry & Math
- [ ] **SDF (Signed Distance Fields):** Integrazione formale dello shader Raymarching 2D esistente.
- [ ] *(Opzionale)* Marching Cubes o Dual Contouring.

## 5. GPU & Simulation (Compute Shaders)
- [ ] **Compute Shaders:** Setup base per calcoli GPU paralleli.
- [ ] **Particles / Fluid:** Simulazione GPGPU.
