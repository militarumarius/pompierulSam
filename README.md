Sistem de Detectie a Focului si Stingere Automata cu Apa
=========================================================

Proiect realizat la PM
Autor: Marius

Descriere
---------
Proiectul foloseste un ATmega328P pentru a detecta flacara, a afisa starea pe un OLED I2C si a controla un sistem de stingere cu pompa. LED-ul RGB si buzzerul semnalizeaza starea, iar cele doua servomotoare orienteaza furtunul.

Componente
----------
- ATmega328P (pe breadboard)
- Ecran OLED I2C
- LED RGB (catod comun)
- Senzor flacara (mod digital)
- Buzzer piezo
- 2x Servo SG90 (baza + furtun)
- Modul releu + pompa de apa

Conexiuni hardware
------------------
| Componenta | Pin / Fir Componenta | Unde se leaga pe Cip (Pin Fizic ATmega328P) | Unde se leaga pe Breadboard / Alimentare |
| --- | --- | --- | --- |
| Cipul ATmega328P | Pin 7 (VCC) & Pin 20 (AVCC) | - | Sina de +5V |
| Cipul ATmega328P | Pin 8 (GND) & Pin 22 (GND) | - | Sina de GND (Minus) |
| Ecran OLED (I2C) | VCC | - | Sina de +5V |
| Ecran OLED (I2C) | GND | - | Sina de GND (Minus) |
| Ecran OLED (I2C) | SDA (Date) | Pin Fizic 27 (PC4) | - |
| Ecran OLED (I2C) | SCL (Ceas) | Pin Fizic 28 (PC5) | - |
| LED RGB (catod comun) | Cathode (cel mai lung) | - | Sina de GND (Minus) |
| LED RGB (catod comun) | Red (Rosu) | Pin Fizic 12 (PD6) | - |
| LED RGB (catod comun) | Green (Verde) | Pin Fizic 11 (PD5) | - |
| LED RGB (catod comun) | Blue (Albastru) | Pin Fizic 5 (PD3) | - |
| Senzor flacara (digital) | VCC | - | Sina de +5V |
| Senzor flacara (digital) | GND | - | Sina de GND (Minus) |
| Senzor flacara (digital) | DO (Digital Output) | Pin Fizic 4 (PD2) | - |
| Buzzer (piezo) | Fir rosu (+) | Pin Fizic 13 (PD7) | - |
| Buzzer (piezo) | Fir negru (-) | - | Sina de GND (Minus) |
| Servo baza (SG90 - jos) | Fir galben/portocaliu | Pin Fizic 15 (PB1 / OCR1A) | - |
| Servo baza (SG90 - jos) | Fir rosu (+) | - | Sina de +5V |
| Servo baza (SG90 - jos) | Fir maro/negru (-) | - | Sina de GND (Minus) |
| Servo furtun (SG90 - sus) | Fir galben/portocaliu | Pin Fizic 16 (PB2 / OCR1B) | - |
| Servo furtun (SG90 - sus) | Fir rosu (+) | - | Sina de +5V |
| Servo furtun (SG90 - sus) | Fir maro/negru (-) | - | Sina de GND (Minus) |
| Modul releu (control) | VCC | - | Sina de +5V |
| Modul releu (control) | GND | - | Sina de GND (Minus) |
| Modul releu (control) | IN (Semnal) | Pin Fizic 14 (PB0) | - |
| Pompa de apa (prin releu) | Borna COM (mijloc) | - | Sina de +5V |
| Pompa de apa (prin releu) | Borna NO (sus) | - | Se leaga la firul rosu (+) al pompei |
| Pompa de apa (prin releu) | Borna NC (jos) | - | Lasa goala |
| Pompa de apa (prin releu) | Fir negru (-) | - | Sina de GND (Minus) |

Detalii de implementare
-----------------------
- ATmega328P gestioneaza citirea senzorului de flacara pe intrarea digitala si comanda releul pentru pompa.
- OLED-ul afiseaza starea curenta (normal, detectie flacara, stingere).
- LED-ul RGB semnalizeaza vizual starea sistemului.
- Buzzerul ofera alarma sonora la detectie.
- Servomotoarele orienteaza furtunul pentru scanare orizontala si inclinare verticala.

Rulare
------
- Build: `platformio run`
- Upload: `platformio run --target upload`
