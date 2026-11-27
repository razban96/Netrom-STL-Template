# Netrom STL Template (C++20)

Repository template pentru rezolvarea problemelor de antrenament și a testului practic STL (C++20) cu testare automată pe fișiere `input*.txt` / `output*.txt`, sanitizere de memorie în modul Debug și suport nativ pentru **mai multe probleme în același proiect**.

---

## Structura proiectului

```text
Netrom-STL-Template/
├── .github/workflows/ci.yml   # Verificare automată în GitHub Actions (Debug + Release)
├── .clang-format              # Stil de formatare cod (Allman, C++20, 4 spații)
├── .gitignore                 # Ignoră folderele de build și fișierele my_output.txt
├── CMakeLists.txt             # Detectează automat toate problemele și testele lor
└── problems/
    ├── template1/              # Prima problemă (gata de folosit / clonat)
    │   ├── IO/
    │   │   ├── input1.txt
    │   │   ├── output1.txt
    │   │   └── ...
    │   └── main.cpp           # Fișier unic, complet autonom, gata de upload
    └── template2/             # A doua problemă
        ├── IO/
        │   ├── input1.txt
        │   └── output1.txt
        └── main.cpp
```

---

## Cum începi rapid (Examen / Practică)

1. Apasă **Use this template** pe GitHub (sau clonează repository-ul local) și deschide folderul în **CLion**.
2. Deschide `problems/template1/IO/` și pune fișierele primite (`input.txt`/`output.txt` sau `input1.txt`–`input4.txt` și `output1.txt`–`output4.txt`). Opțional, poți salva și enunțul `.pdf` direct în `problems/template1/`.
3. Dă **Reload CMake Project** în CLion (dacă ai adăugat fișiere noi în `IO/`).
4. Scrie rezolvarea în funcția `solve()` din `problems/template1/main.cpp` folosind normal `cin` și `cout`.
5. Rulează verificarea automată pe toate testele selectând target-ul **`test_template1`** din colțul dreapta-sus al CLion și apăsând **Run**.
6. La final, încarcă pe platforma de evaluare **doar fișierul `.cpp`** (`problems/template1/main.cpp`).

---

## Cum funcționează `main.cpp`

Fișierul `.cpp` din fiecare problemă este **100% autonom** (nu depinde de headere locale), astfel încât să poată fi încărcat direct pe platforma de verificare fără nicio modificare.

* **Funcția `solve()`**: Aici scrii toată logica problemei, citind din `cin` și afișând în `cout`.
* **Macro-ul `#ifdef LOCAL`**: CMake definește automat `LOCAL` și `IO_DIR` pentru build-urile locale. Clasa `IoRedirector` redirecționează automat `cin` și `cout` către fișierele din `IO/` folosind `rdbuf()`, iar la ieșirea din program restaurează stream-urile. Pe serverul de evaluare, `LOCAL` nu este definit, deci codul va folosi standard `stdin` / `stdout`.
* **Două moduri de rulare**:
  * **Rulare manuală / Debug (din CLion):** folosește automat `IO/input<TEST_ID>.txt` și scrie rezultatul în `IO/my_output.txt`.
  * **Rulare automată (CTest):** CMake pasează căile de input și output ca argumente în linia de comandă (`argv[1]`, `argv[2]`), rulând pe rând fiecare test fără să fie nevoie să modifici codul.

### Cum schimbi testul pentru rulare manuală / Debugging

În `main.cpp`, modifică doar linia:

```cpp
#define TEST_ID "1"
```

* Setează `"1"`, `"2"`, `"3"` etc. pentru a rula `IO/input1.txt`, `IO/input2.txt`, `IO/input3.txt`.
* Setează `""` (șir gol) dacă problema folosește fișierul `IO/input.txt` fără cifră.

Selectează target-ul problemei (ex: `template`) din CLion și apasă **Run** sau **Debug**. Output-ul va fi scris în `problems/<nume_problema>/IO/my_output.txt`.

---

## Cum funcționează `CMakeLists.txt` și testarea automată

Nu trebuie să modifici `CMakeLists.txt` atunci când adaugi probleme sau teste noi — totul este descoperit automat:

1. **Descoperire automată de probleme**: CMake scanează toate subfolderele din `problems/`. Pentru fiecare folder care conține un singur fișier `.cpp`, creează:
   * un executabil cu numele folderului (ex: `template`, `template2`);
   * un target de testare dedicat `test_<nume_folder>` (ex: `test_template`, `test_template2`);
   * un target global `run_tests` care rulează toate testele din toate problemele.
2. **Descoperire automată de teste**: Pentru fiecare fișier `IO/input*.txt`, CMake caută automat perechea `IO/output*.txt` (ex: `input.txt` ↔ `output.txt`, `input1.txt` ↔ `output1.txt`, `input4_bonus.txt` ↔ `output4_bonus.txt`). Dacă lipsește fișierul de output pereche, CMake afișează un avertisment (`WARNING`).
3. **Comparare inteligentă (Diff)**:
   * Ignoră diferențele de final de linie (`CRLF` vs `LF`) și spațiile/liniile goale de la final.
   * Oprește automat programul dacă depășește **10 secunde** (protecție împotriva buclelor infinite).
   * La diferențe, indică **exact prima linie care diferă** (`Generat` vs `Asteptat`), plus un preview trunchiat al fișierelor.
4. **Protecție maximă în modul Debug**:
   * **Linux / macOS (GCC / Clang):** `-Wall -Wextra -Wshadow`, `_GLIBCXX_DEBUG` (prinde iteratori invalizi și indexări out-of-bounds în STL), plus AddressSanitizer & UndefinedBehaviorSanitizer (`-fsanitize=address,undefined`).
   * **Windows (MSVC):** `/W4`, `_ITERATOR_DEBUG_LEVEL=2` și `/fsanitize=address`.

---

## Cum adaugi o problemă nouă

1. Copiază folderul `problems/template/` și lipește-l în `problems/` cu un nume descriptiv, fără spații (ex: `problems/03_gestiune_comenzi/`).
2. Înlocuiește fișierele din `problems/03_gestiune_comenzi/IO/` cu fișierele `input*.txt` și `output*.txt` ale noii probleme și pune enunțul `.pdf` în folderul problemei (opțional).
3. În CLion, apasă **Tools → CMake → Reload CMake Project**.
4. Selectează din bara de sus:
   * `03_gestiune_comenzi` pentru **Run / Debug** pe un singur test;
   * `test_03_gestiune_comenzi` pentru a rula **toate testele** problemei.

> **Regulă:** Fiecare folder din `problems/` trebuie să conțină **un singur fișier `.cpp`** (poate fi numit `main.cpp` sau cum cere platforma de upload). Dacă există mai multe `.cpp` în același folder, CMake va ignora folderul și va afișa un warning.

---

## Rulare din terminal (opțional)

```bash
# Configurare și compilare (Debug sau Release)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# Rulează toate testele pentru toate problemele
ctest --test-dir build --output-on-failure

# Rulează testele doar pentru o anumită problemă (ex: template)
ctest --test-dir build -R "^template\." --output-on-failure
```

---

## Formatare cod

Proiectul include `.clang-format` configurat pentru C++20 (acolade Allman, 4 spații, limită 100 caractere, sortare automată `#include`). În CLion, apasă `Ctrl + Alt + L` (Windows/Linux) sau `Cmd + Option + L` (macOS) pentru a formata automat fișierul curent.