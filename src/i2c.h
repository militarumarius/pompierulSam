#pragma once

#include <stdint.h>

extern const uint8_t Text_Scan[];
extern const uint8_t Text_Danger[];
extern const uint8_t Text_Extinguish[];

extern const uint8_t Text_Scan_Length;
extern const uint8_t Text_Danger_Length;
extern const uint8_t Text_Extinguish_Length;

void init_i2c(void);
void init_oled(void);
void oled_clear(void);
void oled_set_cursor(uint8_t page, uint8_t column);
void oled_write_text(const uint8_t* text, uint8_t length);
void oled_write_text_wide(const uint8_t* text, uint8_t length);
