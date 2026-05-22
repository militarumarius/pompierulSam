#include "i2c.h"

#include <avr/io.h>
#include <util/delay.h>

// FONT MATRIX (5x7 pixels for OLED text)
// Contains only the characters for: "SCANARE", "PERICOL", "STINGERE", " "
static const uint8_t Minimal_Font[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // [0] Space ' '
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

// Character string mapping using the indices from the matrix above
const uint8_t Text_Scan[]       = {12, 1, 2, 3, 2, 4, 5};       // "SCANARE"
const uint8_t Text_Danger[]     = {6, 5, 4, 7, 1, 8, 9};        // "PERICOL"
const uint8_t Text_Extinguish[] = {12, 11, 7, 3, 10, 5, 4, 5};  // "STINGERE"

const uint8_t Text_Scan_Length = sizeof(Text_Scan);
const uint8_t Text_Danger_Length = sizeof(Text_Danger);
const uint8_t Text_Extinguish_Length = sizeof(Text_Extinguish);

// I2C (TWI) DRIVER AND OLED VIA REGISTERS
void init_i2c(void) {
    // ENABLE INTERNAL PULL-UPS ON PC4 (SDA) and PC5 (SCL)
    PORTC |= (1 << PC4) | (1 << PC5);

    TWBR = 72; // SCL at 100kHz
    TWSR = 0;  // Prescaler = 1
}

static void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)) && timeout > 0) {
        timeout--;
    }
}

static void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    uint16_t timeout = 10000;
    while ((TWCR & (1 << TWSTO)) && timeout > 0) {
        timeout--;
    }
}

static void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)) && timeout > 0) {
        timeout--;
    }
}

static void oled_command(uint8_t command) {
    i2c_start();
    i2c_write(0x3C << 1); // OLED write address
    i2c_write(0x00);      // Co=0, D/C#=0 (Command mode)
    i2c_write(command);
    i2c_stop();
}

void init_oled(void) {
    _delay_ms(100); // Let the display power up
    oled_command(0xAE); // Display OFF (stop it for setup)
    oled_command(0xD5); oled_command(0x80); // Set Display Clock Divide
    oled_command(0xA8); oled_command(0x3F); // Set Multiplex Ratio (64 lines)
    oled_command(0xD3); oled_command(0x00); // Set Display Offset
    oled_command(0x40); // Set Start Line to 0
    oled_command(0x8D); oled_command(0x14); // CHARGE PUMP ENABLE (Most important!)
    oled_command(0x20); oled_command(0x02); // Set Memory Addressing Mode to PAGE
    oled_command(0xA1); // Segment Re-map
    oled_command(0xC8); // COM Output Scan Direction (rotates image correctly)
    oled_command(0xDA); oled_command(0x12); // COM Pins Hardware Configuration
    oled_command(0x81); oled_command(0x7F); // Set Contrast
    oled_command(0xD9); oled_command(0xF1); // Set Pre-charge Period
    oled_command(0xDB); oled_command(0x40); // Set VCOMH Deselect Level
    oled_command(0xA4); // Entire Display ON (resume)
    oled_command(0xA6); // Normal Display (not inverted)
    oled_command(0xAF); // DISPLAY ON (final wake-up)
}

// cler function that fully removes artifacts and leftover text)
void oled_clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_command(0xB0 + page); // Set page (line 0-7)
        oled_command(0x00);
        oled_command(0x10);
        i2c_start();
        i2c_write(0x3C << 1);
        i2c_write(0x40); // Data mode
        // Clear the 128 pixels on the current line
        for (uint8_t col = 0; col < 128; col++) {
            i2c_write(0x00);
        }
        i2c_stop();
    }
}

// Set the screen cursor (Page = line 0-7, Column = 0-127)
void oled_set_cursor(uint8_t page, uint8_t column) {
    oled_command(0xB0 + page);
    oled_command(0x00 + (column & 0x0F));
    oled_command(0x10 + ((column >> 4) & 0x0F));
}

// Render a word defined in our font matrix
void oled_write_text_wide(const uint8_t* text, uint8_t length) {
    i2c_start();
    i2c_write(0x3C << 1);
    i2c_write(0x40); // Data mode
    for (uint8_t i = 0; i < length; i++) {
        uint8_t char_index = text[i];
        for (uint8_t col = 0; col < 5; col++) {
            i2c_write(Minimal_Font[char_index][col]);
            i2c_write(Minimal_Font[char_index][col]);
        }
        i2c_write(0x00);
        i2c_write(0x00);
    }
    // send 8 empty columns at the end to fully cut any horizontal line left by the display
    for (uint8_t clear = 0; clear < 8; clear++) {
        i2c_write(0x00);
    }
    i2c_stop();
}
