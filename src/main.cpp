#include <avr/io.h>
#include <util/delay.h>

#include "i2c.h"
#include "hardware.h"

int main(void) {
    // Pin direction configuration (DDR)
    DDRD |= (1 << DDD6) | (1 << DDD5) | (1 << DDD3) | (1 << DDD7); // RGB LED + Buzzer
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);               // Relay + Servos
    DDRD &= ~(1 << DDD2);                                          // Flame Sensor (Input)

    init_i2c();
    init_oled();
    oled_clear();
    init_timer1_servo();
    
    set_pump(0);             // Pump off
    set_led_color(0, 1, 0); // Green LED at startup
    
    int16_t base_angle = 75;
    int8_t base_direction = 1;
    uint8_t previous_state = 99; // Avoid redrawing the screen when the state hasn't changed

    set_servo_base(base_angle);
    set_servo_hose(90); 
    
    _delay_ms(1500);

    while (1) {
        // Sensor read (0 = fire detected)
        uint8_t fire_detected = !(PIND & (1 << PIND2)); 

        if (fire_detected) {            
            // 1. Freeze servos on the fire
            set_servo_base(base_angle); 
            set_servo_hose(base_angle);       
            
            // update teh oled screen
            if (previous_state != 1) {
                oled_clear();
                oled_set_cursor(2, 22); 
                oled_write_text_wide(Text_Danger, Text_Danger_Length);
                oled_set_cursor(4, 16); 
                oled_write_text_wide(Text_Extinguish, Text_Extinguish_Length);
                previous_state = 1;
                _delay_ms(50); 
            }
            //start the water , after I2C has finished
            set_pump(1);               
            // 4. Siren and LED
            set_led_color(1, 0, 0);
            for(uint16_t i = 0; i < 300; i++) { 
                PORTD |= (1 << PD7);    
                _delay_us(333);         
                PORTD &= ~(1 << PD7);   
                _delay_us(333);         
            } 
            set_led_color(0, 0, 0);
            for(uint16_t i = 0; i < 200; i++) { 
                PORTD |= (1 << PD7);    
                _delay_us(500);         
                PORTD &= ~(1 << PD7);   
                _delay_us(500);         
            }
        } else {
            //stop pump and buzzer
            set_pump(0);          
            PORTD &= ~(1 << PD7); 
            set_led_color(0, 1, 0);
            set_servo_hose(90);  
            
            // Update the screen
            if (previous_state != 0) {
                _delay_ms(50);
                oled_clear();
                oled_set_cursor(3, 22); 
                oled_write_text_wide(Text_Scan, Text_Scan_Length);
                previous_state = 0;
            }

            base_angle += base_direction;
            if (base_angle >= 150)      base_direction = -1;
            else if (base_angle <= 30)  base_direction = 1;
            
            set_servo_base(base_angle);
            // Scan speed
            _delay_ms(25);
        }
    }
    return 0;
}