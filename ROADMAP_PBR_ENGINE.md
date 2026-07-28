# Roadmap: Sviluppo di un Motore PBR (Deferred Rendering) in C++

Questa roadmap traccia il percorso didattico e tecnico per trasformare il nostro attuale framework di base in un vero e proprio motore grafico basato sulla fisica (Physically Based Rendering), seguendo le architetture dei motori AAA come Unreal Engine 5.

## Fase 1: Dalla matematica 2D al vero 3D (Rasterizzazione)
Prima di calcolare la luce perfetta, dobbiamo poter muovere la telecamera ed esplorare modelli 3D reali (non solo SDF).
- **Libreria Matematica (GLM):** Integrazione di GLM per gestire Vettori e Matrici (Model, View, Projection) in C++.
- **La Camera 3D:** Creazione di una classe `Camera` controllabile con mouse e tastiera in stile First Person.
- **Caricamento Modelli (Assimp):** Impareremo a leggere file `.obj` o `.fbx` (vertici, normali, coordinate UV) e a inviarli alla GPU tramite VBO complessi, abbandonando il Fullscreen Quad per il primo step di rendering.
- **Depth Testing:** Attivazione dello Z-Buffer per far sì che gli oggetti vicini coprano quelli lontani.

## Fase 2: L'Architettura Deferred (Il G-Buffer)
Nei giochi moderni non calcoliamo la luce mentre disegniamo il modello (Forward Rendering). Salviamo i dati "crudi" per processarli dopo.
- **Framebuffer Objects (FBO):** Creeremo dei target di rendering custom invece di disegnare direttamente sullo schermo.
- **Costruzione del G-Buffer:** Scriveremo uno Shader che prende i modelli 3D e salva in tre texture separate: 
  1. *Albedo* (Il colore base)
  2. *Normale* (La direzione della superficie, cruciale per la luce)
  3. *Posizione/Profondità* (Le coordinate 3D del pixel)
- **Il ritorno del Fullscreen Quad:** Usa il quad che abbiamo già creato per spalmare queste texture a tutto schermo e verificare che i dati siano stati salvati correttamente.

## Fase 3: La Magia della Luce (PBR e Cook-Torrance)
Questa è la fase più intensiva dal punto di vista matematico, ed è il cuore delle domande tecniche ai colloqui.
- **Materiali PBR:** Aggiunta di *Roughness* (Ruvidezza) e *Metallic* al nostro G-Buffer.
- **Equazione di Rendering (BRDF):** Implementazione in GLSL della *Cook-Torrance BRDF*. Capiremo come si calcola la distribuzione delle micro-faccette (Normal Distribution Function) e la formula di Fresnel (come la luce riflette ai bordi).
- **Luci Multiple:** Calcoleremo l'impatto di decine di Point Lights leggendo i dati dal G-Buffer, ottimizzando le performance rispetto al Forward Rendering.

## Fase 4: Post-Processing e Resa Cinematica
Un'immagine PBR senza post-processing appare sempre piatta e "strana". Diamo il tocco finale (Tech Art puro).
- **HDR (High Dynamic Range):** Permetteremo alla luce di superare il valore bianco massimo (1.0), salvando il render in texture a virgola mobile.
- **Tone Mapping:** Implementeremo l'algoritmo *ACES* (usato nel cinema e in Unreal Engine) per riportare i valori HDR nel range visibile del monitor, ottenendo contrasti realistici.
- **Bloom:** Estrazione dei pixel più luminosi, sfocatura gaussiana (Gaussian Blur) e blending sopra l'immagine originale per simulare il bagliore dell'obiettivo della telecamera.

---
**Obiettivo Finale:** Alla fine di questo percorso, avrai un repository GitHub con un tuo motore grafico capace di renderizzare asset PBR alla stessa qualità visiva nativa di Unreal Engine, dimostrando una padronanza totale del C++ e delle architetture GPU.
