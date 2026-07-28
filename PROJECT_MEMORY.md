# Graphics Lab: Devlog & Technical Memory

Questo documento è il core del mio *Graphics Lab*. Serve come manuale tecnico e devlog.
**Regola d'oro:** Nessuna feature si considera conclusa se non è stata formalmente documentata qui sotto usando il template prestabilito.

---
## Template di Documentazione

Ogni nuova feature seguirà questo schema:
- **Problema:** L'obiettivo tecnico da raggiungere o il problema da risolvere.
- **Teoria:** Matematica o concetti di Computer Graphics alla base della feature.
- **Implementazione:** Le scelte architetturali in C++ e l'uso delle API (OpenGL).
- **Difficoltà:** Sfide, colli di bottiglia o workaround (es. Gimbal Lock, Z-Fighting).

---

## [Feature Completate]

### Feature 1: Inizializzazione OpenGL & Raymarcher 2D
- **Problema:** Disegnare un pixel a schermo senza l'uso di un motore commerciale (Unity/Unreal), inviando input nativi del sistema operativo.
- **Teoria:** La comunicazione CPU-GPU avviene tramite buffer. Per il Raymarching, è sufficiente inviare un singolo "Quad" (2 triangoli 2D) che copra l'intero schermo (da -1 a 1 nelle Normalized Device Coordinates) e lasciare che il Fragment Shader spari raggi matematici (SDF) per ogni pixel.
- **Implementazione:** Uso di `GLFW` per la gestione della finestra e input. Creazione del **VBO** (Vertex Buffer Object, l'ammasso di vertici grezzi in VRAM) e del **VAO** (Vertex Array Object, la struttura che mappa la memoria per la GPU). Incapsulamento della compilazione degli shader nella classe `Shader.h`.
- **Difficoltà:** OpenGL è una state-machine complessa; omettere una bind (`glBindVertexArray`) causa silent-failures (schermo nero).

### Feature 2: Camera 3D e Matrici (MVP)
- **Problema:** Passare da un Quad 2D a un ambiente 3D liberamente esplorabile tramite WASD e Mouse, slegato dal framerate.
- **Teoria:** Un mondo 3D si mappa sullo schermo 2D passando i vertici attraverso 3 matrici `4x4`:
  1. **Model Matrix:** Sposta l'oggetto dal suo spazio locale al mondo.
  2. **View Matrix:** Sposta l'intero mondo in senso inverso alla telecamera.
  3. **Projection Matrix:** Applica la prospettiva (Z-divide) per schiacciare il mondo nel cubo NDC (Normalized Device Coordinates).
  Il calcolo nel Vertex Shader è rigido (da destra a sinistra): `gl_Position = Projection * View * Model * vec4(aPos, 1.0);`.
- **Implementazione:** Integrazione della libreria matematica `GLM`. Creazione classe `Camera`. Uso del **Delta Time** per scalare la velocità (`velocity = speed * deltaTime`). Uso degli **Angoli di Eulero** (Pitch e Yaw) calcolati tramite l'offset del mouse (Callback di GLFW). Abilitazione dello **Z-Buffer** (`GL_DEPTH_TEST`) per prevenire la sovrapposizione errata dei triangoli.
- **Difficoltà:** Il calcolo degli angoli soffre intrinsecamente di **Gimbal Lock**. Se il Pitch tocca esattamente i 90°, l'asse Z della telecamera si allinea perfettamente all'asse Y globale, mandando in crash il calcolo del prodotto vettoriale (`glm::cross(Front, Up)`). *Soluzione:* Clamp hardware del Pitch a 89.0 gradi.

### Feature 3: Model Loading & Object-Oriented Geometry
- **Problema:** Caricare un modello 3D reale (es. `.obj`) invece di scrivere l'array di vertici a mano nel codice.
- **Teoria:** I file 3D contengono nodi gerarchici. Per ogni geometria dobbiamo estrarre i Vertici (Posizione, Normale, UV) e gli Indici (le istruzioni su come unire i vertici per formare i triangoli senza duplicarli in memoria).
- **Implementazione:** Aggiunta di **Assimp** al progetto via CMake (FetchContent). Creazione di un'architettura OOP simile ai "Prefab" dei motori:
  - Classe `Mesh`: Rappresenta il sotto-pezzo. Inizializza i buffer (incluso l'**EBO** - Element Buffer Object per gestire gli indici) ed espone `Draw()` chiamando `glDrawElements` invece di `glDrawArrays`.
  - Classe `Model`: Parsa il file con Assimp, estrae gli array da `aiScene`, costruisce oggetti `Mesh` C++ e li raggruppa in un vettore.
- **Difficoltà:** Il linker C++ impazzisce (Undefined Symbols) se un nuovo file `.cpp` viene aggiunto al disco ma CMake non viene riconfigurato per tracciarlo (problema del file globbing). Assimp, inoltre, è mastodontico da buildare; sono stati disabilitati i moduli tool e test nel CMakeLists per non freezare la CPU in compilazione.
