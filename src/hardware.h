#pragma once

#include <stdint.h>

void init_timer1_servo(void);
void set_servo_base(uint16_t grade);
void set_servo_hose(uint16_t grade);
void set_led_color(uint8_t red, uint8_t green, uint8_t blue);
void set_pump(uint8_t on);
