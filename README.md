# 🧱 Breakout

Joc Breakout pentru terminal, scris în C cu biblioteca ncurses. Controlezi paleta cu săgețile, lansezi mingea cu SPACE și încerci să spargi toate cele 45 de cărămizi 🎮

## ✨ Cum funcționează

- 🧱 **45 de cărămizi** pe 5 rânduri și 9 coloane, fiecare rând cu altă culoare. Cu cât rândul e mai sus, cu atât valorează mai multe puncte (de la 10 la 50) 🏆
- 🏓 **Paleta** are 11 caractere, iar unghiul mingii depinde de locul în care o lovește: la margini mingea pleacă mai oblic, în centru rămâne aproape pe verticală 🎯
- ⚽ **Mingea** are coordonate reale (`double`), deci se mișcă fluid și cu viteze diferite pe orizontală și pe verticală
- 💥 **Coliziuni** cu pereții, cu paleta și cu cărămizile; cărămida lovită dispare, iar mingea își schimbă direcția
- 🏁 **Final de joc**: câștigi când nu mai rămâne nicio cărămidă, pierzi dacă mingea cade sub paletă. Poți reîncepe cu `R` 🔁
- 🎨 **Chenar decorativ** desenat cu caractere speciale ncurses, cu colțurile marcate prin diamante și scorul afișat în bara de sus

## 🛠️ Tehnologii folosite

| Tehnologie | La ce o folosesc |
|------------|------------------|
| **C** | limbajul în care e scris tot jocul |
| **ncurses** | fereastra din terminal, culorile, desenarea caracterelor, citirea tastelor |
| `<unistd.h>` | `usleep` pentru pauza de 15 ms dintre cadre |
| `<stdlib.h>` + `<time.h>` | `rand` și `srand(time(NULL))`, pentru direcția inițială aleasă aleatoriu a mingii |
| **gcc** | compilarea programului |

## 🧠 Idei folosite în cod

- 🔄 **Game loop**: citire taste, actualizare stare, desenare, pauză, repetat la infinit
- ⌨️ **Input fără blocare**: `nodelay` și `keypad` fac ca jocul să meargă mai departe chiar dacă nu apeși nicio tastă, și permit citirea săgeților
- 🧩 **`struct Brick`** pentru poziția, culoarea, punctajul și starea fiecărei cărămizi
- 🖌️ **Redesenare parțială**: se șterg doar poziția veche a mingii și a paletei, apoi se redesenează totul în fereastră, fără să clipească tot ecranul
- 🔢 **Mașină de stări** simplă (`stare_finala`): joc în desfășurare, câștigat sau pierdut
- 📐 **Centrare automată** a ferestrei de joc în terminal, în funcție de dimensiunea lui

## 📦 Cerințe

- `gcc`
- biblioteca ncurses: `sudo apt install libncurses-dev` (Debian/Ubuntu)
- un terminal de cel puțin 57 × 21 caractere, cu suport pentru culori

## ▶️ Compilare și rulare

```bash
gcc joc.c -o joc -lncurses
./joc
```

## 🎮 Controale

| Tastă | Acțiune |
|-------|---------|
| ← / → | mută paleta |
| SPACE | lansează mingea |
| R | restart, după ce jocul s-a terminat |
| Q | ieșire |
