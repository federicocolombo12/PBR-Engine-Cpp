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

### Le Fondamenta del Vero 3D: Il Modello MVP
In Unity, quando scrivi uno Shader Graph, queste matrici sono già calcolate di nascosto. In C++ dobbiamo costruirle e inviarle alla GPU riga per riga.
Ogni vertice di un modello 3D parte da uno "spazio locale" (Coordinate Modello, es. il centro del cubo è 0,0,0) e deve finire sulle coordinate piatte del tuo monitor 2D. Questo viaggio passa per 3 trasformazioni matematiche (moltiplicazioni di matrici 4x4):

1. **Model Matrix (Matrice Modello):** Sposta il modello dal suo spazio locale allo spazio globale del mondo (World Space). Se vuoi traslare, ruotare o scalare l'oggetto, modifichi questa matrice.
2. **View Matrix (Matrice di Vista):** Trasla e ruota *l'intero universo* in senso opposto alla tua Telecamera. Se la telecamera va avanti di 3 metri, la View Matrix sposta l'intero mondo indietro di 3 metri. Ora siamo nel *View Space* (o Camera Space).
3. **Projection Matrix (Matrice di Proiezione):** Prende questo mondo 3D e applica la distorsione prospettica. Le cose lontane diventano più piccole. Schiaccia tutto in un cubo normalizzato (da -1 a 1) chiamato NDC (Normalized Device Coordinates). 

Nel nostro Vertex Shader (`raster.vert`), il calcolo finale sarà sempre:
`gl_Position = Projection * View * Model * vec4(aPos, 1.0);`
*(Nota per chi usa HLSL: in GLSL l'ordine della moltiplicazione è inverso! Leggiamo da destra a sinistra).*

### Il Depth Testing (Z-Buffer)
Quando disegniamo un cubo di 36 vertici, OpenGL disegnerà i triangoli nell'ordine in cui sono nell'array. Questo è un disastro: la faccia "dietro" del cubo potrebbe essere disegnata *dopo* la faccia davanti, coprendola!
La soluzione hardware è lo **Z-Buffer** (Depth Buffer). È una texture invisibile grande quanto lo schermo, che non salva colori, ma la "distanza" (Profondità) del pixel dalla telecamera.
Abilitando `glEnable(GL_DEPTH_TEST)`, OpenGL prima di colorare un pixel fa una domanda: *"Questo nuovo pixel è più vicino alla telecamera rispetto a quello già salvato nello Z-Buffer?"*. Se sì, lo disegna e aggiorna il buffer. Se no, lo scarta (Depth Fail).

### Il Movimento 3D e il Delta Time
Nei motori grafici (sia Unity che C++), il rendering non è mai perfettamente fisso a 60 FPS. Potrebbe andare a 144 FPS o scendere a 30.
Se dicessimo alla camera `Position.z += 1.0f` ad ogni frame, chi ha un PC potente camminerebbe velocissimo, chi ha un PC lento andrebbe a rallentatore.
La soluzione è il **Delta Time (Tempo trascorso dall'ultimo frame)**.
Moltiplicando la nostra velocità per il `deltaTime` (`velocity = MovementSpeed * deltaTime`), leghiamo il movimento al *tempo reale* (metri al secondo) e non al framerate! 
Inoltre, per muoverci di lato (A e D), abbiamo usato il prodotto vettoriale per ottenere il vettore `Right` della telecamera, che calcola automaticamente la direzione perpendicolare a dove stiamo guardando.

### La Rotazione della Camera (Angoli di Eulero)
Per girare la testa usiamo il mouse, leggendo di quanti pixel si è spostato dall'ultimo frame (`xoffset` e `yoffset`). Applichiamo questi offset a due angoli fondamentali, gli **Angoli di Eulero**:
- **Yaw (Imbardata):** Gira la testa a destra e a sinistra (asse Y).
- **Pitch (Beccheggio):** Guarda in alto e in basso (asse X).
- *(C'è anche il Roll - Rollio - per inclinare la testa di lato sull'asse Z, ma in un FPS non si usa quasi mai).*

**Il problema del Gimbal Lock:** 
Se permettiamo al Pitch di superare i 90 gradi verso l'alto (es. guardare dietro la schiena piegandosi all'indietro), l'asse Y (Up) del mondo si allinea perfettamente all'asse Z della telecamera. Matematicamente, questo manda in crisi il nostro prodotto vettoriale (`glm::cross`), causando lo "scatto" improvviso e innaturale della visuale e la perdita di un grado di libertà. 
Per evitarlo, aggiungiamo un semplice limite hardware: `if (Pitch > 89.0f) Pitch = 89.0f;`. Mantenendo l'angolo appena sotto i 90 gradi perfetti, la matematica è salva.

## Fase 1.3: Caricamento Modelli Reali
Abbiamo abbandonato i vertici scritti a mano per integrare **Assimp** (Open Asset Import Library). 
### L'Astrazione ad Oggetti: Mesh e Model
Adesso che sappiamo caricare i file, ci serve un'infrastruttura OOP (Object-Oriented Programming) per non impazzire nel `main.cpp`.
Abbiamo creato due classi chiave (che imitano esattamente come funziona internamente un Prefab in Unity):
1. **Mesh:** Rappresenta la geometria "grezza". Possiede un array di struct `Vertex` (Posizione, Normale, UV) e un array di interi (gli Indici). Si occupa solo di mandare questi dati alla VRAM (VAO, VBO, EBO) e di disegnare se stessa. Al posto della vecchia `glDrawArrays`, usiamo per la prima volta **`glDrawElements`**, che dice ad OpenGL di leggere i dati in modo iper-ottimizzato usando l'Index Buffer (EBO).
2. **Model:** Il contenitore (il Root del Prefab). Legge il file 3D dal disco usando l'Importer di Assimp. Assimp restituisce una "Scena" fatta ad Albero (Nodi che hanno figli). La classe `Model` naviga in questo albero, estrae i dati da ogni `aiMesh` trovata, crea oggetti `Mesh` C++ e li salva in una lista. Per renderizzare l'intero personaggio, basta chiamare `Model.Draw()`.
