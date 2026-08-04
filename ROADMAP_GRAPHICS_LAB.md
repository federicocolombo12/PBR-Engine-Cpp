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
- [ ] **Deferred Rendering (G-Buffer):** Preparazione dell'infrastruttura per luci illimitate.
- [ ] **PBR & Lighting:** Modello di illuminazione fisicamente accurato (Cook-Torrance).
- [ ] **NPR / Toon Shading (Stile Anime):** Modifica del modello di illuminazione per ottenere il classico effetto Cel Shading e Outline nero in stile animazione giapponese.
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
- [ ] **Water Simulation (Rotta Maggiore):** Simulazione oceanica tramite Gerstner Waves / FFT per renderizzare mari realistici.
- [ ] **Particles:** Effetti particellari GPGPU (fuoco, fumo, Haki).
