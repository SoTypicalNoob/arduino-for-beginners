#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Internal led was used on Arduino:
    // Arduino: Digital pin 13
    // Atmega328P: PB5
    DDRB = 0b00100000; // DDB5 is set to OUTPUT
    while (1) {
        PORTB = 0b00100000; // PB5 is set to HIGH
        _delay_ms(2000);
        PORTB = 0b00000000; // PB5 is set to LOW
        _delay_ms(1000);
    }
}
