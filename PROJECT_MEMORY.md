# Project Memory: Log Tecnico e Architetturale

Questo documento funge da "Game Design Document" tecnico. Registriamo qui le nostre conquiste architetturali, i "perché" delle nostre scelte, e i concetti chiave che non vogliamo dimenticare.

---

## Fase 0: Le Fondamenta (Il Raymarcher)
Abbiamo iniziato creando il layer di comunicazione basilare tra CPU e GPU.
- **La Finestra (GLFW):** Abbiamo delegato a `GLFW` la noiosa creazione di finestre native macOS e la gestione dell'input.
- **La Classe Shader:** Invece di scrivere comandi `glCompileShader` sparsi per il codice, abbiamo incapsulato la logica in una classe C++ (`Shader.h`). Questo ci permette di caricare i file di testo (`.vert`, `.frag`), compilarli in memoria VRAM e linkarli in una sola riga di codice pulita.
- **VAO & VBO (Il Fullscreen Quad):** 
  - Il *VBO (Vertex Buffer Object)* è il "secchio" di byte grezzi (i nostri 6 vertici del quad) che abbiamo spostato dalla RAM del Mac alla potentissima VRAM della GPU.
  - Il *VAO (Vertex Array Object)* è l'"etichetta" che spiega alla GPU come leggere quel secchio (ovvero: "leggi i float 2 a 2 per formare le coordinate (x,y)").
- **Il Raymarcher GLSL:** Invece di inviare sfere o cubi 3D tramite VBO, abbiamo inviato solo due triangoli 2D (il quad) che coprono lo schermo. Nel Fragment Shader, abbiamo sparato un raggio matematico per ogni pixel, calcolando la distanza da un SDF (Signed Distance Field). Questo è l'apice del rendering matematico/procedurale.

## Fase 1: Transizione al PBR
*Decisione Architetturale:* Abbiamo deciso di mantenere vivo l'SDF Raymarcher tramite un "Toggle" a runtime (Opzione A). Premendo un tasto, scambieremo al volo lo Shader in uso, dimostrando flessibilità estrema nel nostro Render Loop. Stiamo integrando **GLM** per gestire la matematica delle matrici (Projection, View, Model) necessarie per il vero 3D.
