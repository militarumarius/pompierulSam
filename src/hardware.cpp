#include "hardware.h"

#include <avr/io.h>

// HARDWARE CONFIGURATION (SERVO AND LED)
void init_timer1_servo(void) {
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
    ICR1 = 4999; // 50Hz for servo
}

void set_servo_base(uint16_t grade) {
    if (grade > 180) grade = 180;
    OCR1A = 250 + ((uint32_t)grade * 250) / 180;
}

void set_servo_hose(uint16_t grade) {
    if (grade > 180) grade = 180;
    OCR1B = 250 + ((uint32_t)grade * 250) / 180;
}

void set_led_color(uint8_t red, uint8_t green, uint8_t blue) {
    if (red)      PORTD &= ~(1 << PD6); else PORTD |= (1 << PD6);
    if (green)    PORTD &= ~(1 << PD5); else PORTD |= (1 << PD5);
    if (blue)     PORTD &= ~(1 << PD3); else PORTD |= (1 << PD3);
}

void set_pump(uint8_t on) {
    // Relay active on LOW level (inverted pump control)
    if (on) {
        PORTB &= ~(1 << PB0);
    } else {
        PORTB |= (1 << PB0);
    }
}
