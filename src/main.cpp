#include <avr/io.h>
#include <util/delay.h>

// ==========================================
// 1. MATRICE MINIMALĂ DE FONTURI (5x7 pixeli pentru text pe OLED)
// Contine strict caracterele pentru: "SCANARE", "PERICOL", "STINGERE", " "
// ==========================================
const uint8_t Font_Minimal[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // [0] Spațiu ' '
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // [1] 'C'
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, // [2] 'A'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // [3] 'N'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // [4] 'R'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // [5] 'E'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // [6] 'P'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // [7] 'I'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // [8] 'O'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // [9] 'L'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // [10] 'G'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // [11] 'T'
    {0x23, 0x45, 0x49, 0x31, 0x12}  // [12] 'S'
};

// Maparea șirurilor de caractere conform indicilor din matricea de mai sus
const uint8_t Text_Scanare[]  = {12, 1, 2, 3, 2, 4, 5};         // "SCANARE"
const uint8_t Text_Pericol[]  = {6, 5, 4, 7, 1, 8, 9};         // "PERICOL"
const uint8_t Text_Stingere[] = {12, 11, 7, 3, 10, 5, 4, 5};    // "STINGERE"

// ==========================================
// 2. DRIVER I2C (TWI) ȘI OLED PRIN REGISTRI
// ==========================================
void initializare_I2C(void) {
    // ACTIVARE REZISTENȚE PULL-UP INTERNE PE PC4(SDA) și PC5(SCL)
    // Fără linia asta, protocolul I2C pe bare-metal este mort!
    PORTC |= (1 << PC4) | (1 << PC5); 

    TWBR = 72; // SCL la 100kHz
    TWSR = 0;  // Prescaler = 1
}

void I2C_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    uint16_t timeout = 10000; // Limită de siguranță
    // Dacă ecranul se blochează, timeout-ul scade la 0 și procesorul iese din buclă forțat!
    while (!(TWCR & (1 << TWINT)) && timeout > 0) {
        timeout--;
    }
}

void I2C_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    uint16_t timeout = 10000;
    while ((TWCR & (1 << TWSTO)) && timeout > 0) {
        timeout--;
    }
}

void I2C_scrie(uint8_t date) {
    TWDR = date;
    TWCR = (1 << TWINT) | (1 << TWEN);
    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)) && timeout > 0) {
        timeout--;
    }
}

void oled_comanda(uint8_t comanda) {
    I2C_start();
    I2C_scrie(0x3C << 1); // Adresă OLED scriere
    I2C_scrie(0x00);      // Co=0, D/C#=0 (Mod Comandă)
    I2C_scrie(comanda);
    I2C_stop();
}

void initializare_OLED(void) {
    _delay_ms(100); // Lăsăm ecranul să se alimenteze
    
    oled_comanda(0xAE); // Display OFF (îl oprim pt configurare)
    
    oled_comanda(0xD5); oled_comanda(0x80); // Set Display Clock Divide
    oled_comanda(0xA8); oled_comanda(0x3F); // Set Multiplex Ratio (64 linii)
    oled_comanda(0xD3); oled_comanda(0x00); // Set Display Offset
    
    oled_comanda(0x40); // Set Start Line la 0
    
    oled_comanda(0x8D); oled_comanda(0x14); // CHARGE PUMP ENABLE (Cel mai important!)
    
    oled_comanda(0x20); oled_comanda(0x00); // Memory Addressing Mode (Horizontal)
    
    oled_comanda(0xA1); // Segment Re-map
    oled_comanda(0xC8); // COM Output Scan Direction (Rotește imaginea corect)
    
    oled_comanda(0xDA); oled_comanda(0x12); // COM Pins Hardware Configuration
    
    oled_comanda(0x81); oled_comanda(0x7F); // Set Contrast
    
    oled_comanda(0xD9); oled_comanda(0xF1); // Set Pre-charge Period
    oled_comanda(0xDB); oled_comanda(0x40); // Set VCOMH Deselect Level
    
    oled_comanda(0xA4); // Entire Display ON (resume)
    oled_comanda(0xA6); // Normal Display (nu inversat)
    
    oled_comanda(0xAF); // DISPLAY ON (Trezirea finală)
}

// Șterge tot ecranul (scrie pixeli negri)
void oled_clear(void) {
    I2C_start();
    I2C_scrie(0x3C << 1);
    I2C_scrie(0x40); // Co=0, D/C#=1 (Mod Date)
    for (uint16_t i = 0; i < 1024; i++) {
        I2C_scrie(0x00);
    }
    I2C_stop();
}

// Setează cursorul pe ecran (Page = linie 0-7, Coloană = 0-127)
void oled_set_cursor(uint8_t pagina, uint8_t coloana) {
    oled_comanda(0xB0 + pagina);
    oled_comanda(0x00 + (coloana & 0x0F));
    oled_comanda(0x10 + ((coloana >> 4) & 0x0F));
}

// Randează un cuvânt definit în matricea noastră de fonturi
void oled_scrie_text(const uint8_t* text, uint8_t lungime) {
    I2C_start();
    I2C_scrie(0x3C << 1);
    I2C_scrie(0x40); // Mod Date
    
    for (uint8_t i = 0; i < lungime; i++) {
        uint8_t caracter_idx = text[i];
        // Trimitem cele 5 coloane de pixeli ale literei + 1 coloană goală (spațiere)
        for (uint8_t col = 0; col < 5; col++) {
            I2C_scrie(Font_Minimal[caracter_idx][col]);
        }
        I2C_scrie(0x00); // Spațiu între litere
    }
    I2C_stop();
}

// ==========================================
// 3. CONFIGURARE HARDWARE (SERVO ȘI LED)
// ==========================================
void initializare_Timer1_Servo(void) {
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
    ICR1 = 4999; // 50Hz pentru servo
}

void set_servo_baza(uint16_t grade) {
    if (grade > 180) grade = 180;
    OCR1A = 250 + ((uint32_t)grade * 250) / 180;
}

void set_servo_furtun(uint16_t grade) {
    if (grade > 180) grade = 180;
    OCR1B = 250 + ((uint32_t)grade * 250) / 180;
}

void set_culoare_led(uint8_t rosu, uint8_t verde, uint8_t albastru) {
    // LED common-anode (logica inversa): 0 = aprins, 1 = stins
    if (rosu)     PORTD &= ~(1 << PD6); else PORTD |= (1 << PD6);
    if (verde)    PORTD &= ~(1 << PD5); else PORTD |= (1 << PD5);
    if (albastru) PORTD &= ~(1 << PD3); else PORTD |= (1 << PD3);
}

static inline void set_pompa(uint8_t pornita) {
    // Releu activ pe nivel LOW (inversare sens comanda pompa)
    if (pornita) {
        PORTB &= ~(1 << PB0);
    } else {
        PORTB |= (1 << PB0);
    }
}

// ==========================================
// 4. PROGRAMUL PRINCIPAL
// ==========================================
// ==========================================
// 4. PROGRAMUL PRINCIPAL
// ==========================================
int main(void) {
    // Configurare direcție pini (DDR)
    DDRD |= (1 << DDD6) | (1 << DDD5) | (1 << DDD3) | (1 << DDD7); // LED RGB + Buzzer
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);               // Releu + Servo-uri
    DDRD &= ~(1 << DDD2);                                          // Senzor Flacără (Input)

    // Inițializări
    initializare_I2C();
    initializare_OLED();
    oled_clear();
    initializare_Timer1_Servo();
    
    set_pompa(0);             // Pompa oprită
    set_culoare_led(0, 1, 0); // LED Verde direct la pornire
    
    int16_t unghi_baza = 75;
    int8_t directie_baza = 1;
    uint8_t stare_anterioara = 99; // Pentru a nu redesena ecranul inutil când nu se schimbă starea

    // REPARAT: Furtunul stă la 90 de grade (drept înainte)
    set_servo_baza(unghi_baza);
    set_servo_furtun(90); 
    
    _delay_ms(1500);

    while (1) {
        // Citire senzor (0 = Foc detectat)
        uint8_t foc_detectat = !(PIND & (1 << PIND2)); 

        if (foc_detectat) {
            // ---- STAREA DE URGENTĂ ----
            
            // 1. Îngheață senzorul pe foc (unghiul curent)
            set_servo_baza(unghi_baza); 
            
            // 2. REPARAT: Trimitem furtunul fix la același unghi la care a găsit focul!
            set_servo_furtun(unghi_baza);       
            
            // 3. Pornește apa
            set_pompa(1);               
            
            // Actualizare text pe ecran (doar dacă starea s-a schimbat, ca să nu clipească)
            if (stare_anterioara != 1) {
                oled_clear();
                oled_set_cursor(2, 20); // Linia 2, Coloana 20
                oled_scrie_text(Text_Pericol, sizeof(Text_Pericol));
                oled_set_cursor(4, 20); // Linia 4
                oled_scrie_text(Text_Stingere, sizeof(Text_Stingere));
                stare_anterioara = 1;
            }

            // ---- REPARAT: SIRENA MELODICĂ PENTRU BUZZER PASIV ----
            
            // 1. Tonul înalt (Ni) + LED Roșu aprins (aprox 1500 Hz)
            set_culoare_led(1, 0, 0);
            for(uint16_t i = 0; i < 300; i++) { 
                PORTD |= (1 << PD7);    // Trage membrana sus
                _delay_us(333);         // Așteaptă
                PORTD &= ~(1 << PD7);   // Lasă membrana jos
                _delay_us(333);         // Așteaptă
            }
            
            // 2. Tonul jos (No) + LED Stins (aprox 1000 Hz)
            set_culoare_led(0, 0, 0);
            for(uint16_t i = 0; i < 200; i++) { 
                PORTD |= (1 << PD7);    // Trage membrana sus
                _delay_us(500);         // Așteaptă mai mult (sunet mai grav)
                PORTD &= ~(1 << PD7);   // Lasă membrana jos
                _delay_us(500);         // Așteaptă
            }
            
        } else {
            // ---- STAREA DE VEGHE / SCANARE ----
            set_pompa(0);          // Pompa oprită
            PORTD &= ~(1 << PD7); // Buzzer OFF
            set_culoare_led(0, 1, 0);
            
            // REPARAT: Cât timp scanează după foc, furtunul stă centrat la 90 de grade
            set_servo_furtun(90);  
            
            if (stare_anterioara != 0) {
                oled_clear();
                oled_set_cursor(3, 30); // Centrat pe linia 3
                oled_scrie_text(Text_Scanare, sizeof(Text_Scanare));
                stare_anterioara = 0;
            }

            // Scanare stânga-dreapta de la 30 la 150 grade (Senzorul)
            unghi_baza += directie_baza;
            if (unghi_baza >= 150)      directie_baza = -1;
            else if (unghi_baza <= 30)  directie_baza = 1;
            
            set_servo_baza(unghi_baza);
            _delay_ms(25); // Viteză scanare
        }
    }
    return 0;
}