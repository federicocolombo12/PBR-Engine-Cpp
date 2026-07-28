# C++ Raymarcher: Da Tech Artist a Graphics Programmer

Questo documento riassume i concetti fondamentali esplorati durante il nostro progetto, pensato per fare il salto dallo scripting visuale (Blueprints) o C# (Unity) allo sviluppo grafico a basso livello (C++ / OpenGL).

---

## Step 1: Il Build System (CMake) e le Dipendenze
In C++, a differenza di C#, non esiste una soluzione universale integrata (come la solution di Visual Studio per Unity) per gestire i file. L'industria utilizza **CMake**.
- **Cosa fa:** Legge un file `CMakeLists.txt` e genera il sistema di compilazione adatto alla tua macchina (su Mac usa `make` e Clang).
- **Le Librerie esterne:** Abbiamo utilizzato `FetchContent` in CMake per scaricare **GLFW** da GitHub. GLFW è una libreria minimale scritta in C che permette di parlare con macOS per aprire finestre e leggere mouse/tastiera.
- **La magia del Mac:** Su Windows avremmo avuto bisogno di un'altra libreria (es. GLAD) per caricare le funzioni OpenGL (i puntatori a funzione). Su Mac, Apple integra direttamente il Core Profile nel sistema, quindi ci è bastato includere `<OpenGL/gl3.h>` e collegare il framework nativo `find_library(OPENGL_FRAMEWORK OpenGL)`.

## Step 2: Inizializzazione della Finestra e Render Loop (main.cpp)
Per disegnare a schermo, non basta scrivere codice grafico. La CPU deve preparare il terreno:
1. **Window Hints:** Diciamo a GLFW che tipo di contesto vogliamo (OpenGL 3.3 Core).
2. **Context Creation:** Creiamo la finestra e "attacchiamo" lo stato di OpenGL ad essa (`glfwMakeContextCurrent`). Da questo momento, i comandi OpenGL finiranno su questa finestra.
3. **Double Buffering:** Le schede video non disegnano pixel in tempo reale sullo schermo visibile (causerebbe *tearing*). Disegnano su una tela nascosta in VRAM. Finito il disegno, chiamiamo `glfwSwapBuffers()` per scambiare istantaneamente le due tele.
4. **Il Render Loop:** Un ciclo `while` infinito che, 60+ volte al secondo, pulisce lo schermo (`glClear`), gestisce input e disegna la geometria.

## Step 3: La Classe Shader (Object-Oriented C++)
In Unreal o Unity, il motore compila il codice shader (HLSL) di nascosto. In C++ puro, dobbiamo farlo noi istruzione per istruzione.
L'architettura C++ ci insegna a nascondere il codice brutto (codice procedurale in stile C) dietro eleganti **Classi**.
- **Header (`Shader.h`):** Il "Menu" della classe. Definisce solo *cosa* esiste (costruttore, ID pubblico, funzioni `use()`).
- **Implementation (`Shader.cpp`):** La vera implementazione. Qui avviene la comunicazione con i driver della GPU:
  1. `glCreateShader`: Creiamo spazi in memoria GPU.
  2. `glShaderSource`: Trasferiamo il codice sorgente in formato stringa.
  3. `glCompileShader`: Ordiniamo alla GPU di compilarlo.
  4. `glLinkProgram`: Uniamo Vertex e Fragment shader in un unico materiale eseguibile.

## Step 4: Geometria (VBO e VAO)
Per fare Raymarching non inviamo modelli 3D complessi alla GPU. Inviamo solo **due triangoli** che coprono l'intero schermo (un *Fullscreen Quad*). La magia 3D avverrà tutta matematicamente nel Fragment Shader!
Ma come si inviano questi vertici?
1. **VBO (Vertex Buffer Object):** È un blocco di memoria grezza (un "secchio") che viene allocato sulla GPU. Con `glBufferData` riversiamo l'array di coordinate dalla RAM del Mac direttamente alla potentissima VRAM della GPU. È un'operazione lenta, quindi si fa *una sola volta* fuori dal Game Loop.
2. **VAO (Vertex Array Object):** La VRAM contiene dati grezzi. La GPU non sa cosa siano. Il VAO è un'etichetta che salva lo stato (il *layout*) di quei dati, spiegando alla pipeline grafica: *"Ehi Input Assembler, leggi i dati dal VBO a gruppi di 2 Float alla volta (x,y)!"*.

## Step 5: Uniforms e GLSL Raymarching
Il nostro "schermo" geometrico era pronto, mancava solo disegnarlo.
1. **Le Uniform:** Dal C++ dobbiamo inviare dati dinamici (che cambiano ogni frame) alla GPU. Abbiamo usato `glGetUniformLocation` e `glUniform1f` per inviare alla scheda video `iTime` (il tempo in secondi) e `iResolution` (larghezza/altezza).
2. **HLSL vs GLSL:** Il codice GLSL (`basic.frag`) è pressoché identico ad HLSL:
   - Invece di `float3` si usa `vec3`.
   - Abbiamo scritto una funzione **SDF** (`sdSphere`) per creare una sfera matematica (non poligonale).
   - Abbiamo implementato un classico ciclo `for` di **Raymarching**: iteriamo avanzando lungo il raggio (Ray Direction) usando la distanza dall'oggetto più vicino (fornita dalla mappa della scena).
   - Quando il raggio colpisce un oggetto matematico (distanza < 0.001), calcoliamo la **Normale** (il gradiente matematico dell'SDF) per calcolare la luce di Lambert.

---
Questo mini-motore C++ ora è una tela bianca. Qualsiasi SDF, materiale procedurale o logica particellare potrai inventare in futuro, avrai sempre il controllo completo su come i dati partono dal C++ e vengono calcolati dalla GPU!
