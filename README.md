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
| Componenta | Pin / Fir Componenta | Unde se leaga pe Cip (Pin Fizic ATmega328P) | Unde se leaga pe Breadboard / Alimentare | Note importante |
| --- | --- | --- | --- | --- |
| Cipul ATmega328P | Pin 7 (VCC) & Pin 20 (AVCC) | - | Sina de +5V | Esential pentru alimentarea cipului |
| Cipul ATmega328P | Pin 8 (GND) & Pin 22 (GND) | - | Sina de GND (Minus) | Masa comuna obligatorie |
| Ecran OLED (I2C) | VCC | - | Sina de +5V | Alimentare ecran |
| Ecran OLED (I2C) | GND | - | Sina de GND (Minus) | Masa ecran |
| Ecran OLED (I2C) | SDA (Date) | Pin Fizic 27 (PC4) | - | Comunicare I2C |
| Ecran OLED (I2C) | SCL (Ceas) | Pin Fizic 28 (PC5) | - | Comunicare I2C |
| LED RGB (catod comun) | Cathode (cel mai lung) | - | Sina de GND (Minus) | Fara rezistenta aici |
| LED RGB (catod comun) | Red (Rosu) | Pin Fizic 12 (PD6) | - | Pune rezistenta de 220 ohmi in serie |
| LED RGB (catod comun) | Green (Verde) | Pin Fizic 11 (PD5) | - | Pune rezistenta de 220 ohmi in serie |
| LED RGB (catod comun) | Blue (Albastru) | Pin Fizic 5 (PD3) | - | Pune rezistenta de 220 ohmi in serie |
| Senzor flacara (digital) | VCC | - | Sina de +5V | Alimentare modul |
| Senzor flacara (digital) | GND | - | Sina de GND (Minus) | Masa modul |
| Senzor flacara (digital) | DO (Digital Output) | Pin Fizic 4 (PD2) | - | Trimite 0 cand vede foc |
| Buzzer (piezo) | Fir rosu (+) | Pin Fizic 13 (PD7) | - | Semnal alarma sonora |
| Buzzer (piezo) | Fir negru (-) | - | Sina de GND (Minus) | Impamantare buzzer |
| Servo baza (SG90 - jos) | Fir galben/portocaliu | Pin Fizic 15 (PB1 / OCR1A) | - | Mijlocul de scanare orizontala |
| Servo baza (SG90 - jos) | Fir rosu (+) | - | Sina de +5V | Consuma mult, pune-l bine in sina |
| Servo baza (SG90 - jos) | Fir maro/negru (-) | - | Sina de GND (Minus) | Masa motor |
| Servo furtun (SG90 - sus) | Fir galben/portocaliu | Pin Fizic 16 (PB2 / OCR1B) | - | Inclinatia verticala (furtun) |
| Servo furtun (SG90 - sus) | Fir rosu (+) | - | Sina de +5V | Alimentare motor |
| Servo furtun (SG90 - sus) | Fir maro/negru (-) | - | Sina de GND (Minus) | Masa motor |
| Modul releu (control) | VCC | - | Sina de +5V | Alimenteaza electronica releului |
| Modul releu (control) | GND | - | Sina de GND (Minus) | Masa releu |
| Modul releu (control) | IN (Semnal) | Pin Fizic 14 (PB0) | - | Comanda de pornire pompa |
| Pompa de apa (prin releu) | Borna COM (mijloc) | - | Sina de +5V | Aduce curentul in intrerupator |
| Pompa de apa (prin releu) | Borna NO (sus) | - | Se leaga la firul rosu (+) al pompei | Curentul pleaca spre pompa doar la comanda HIGH |
| Pompa de apa (prin releu) | Borna NC (jos) | - | Lasa goala | Nu conectezi nimic aici |
| Pompa de apa (prin releu) | Fir negru (-) | - | Sina de GND (Minus) | Inchide circuitul pompei |

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
