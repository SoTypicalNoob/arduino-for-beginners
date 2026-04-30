#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // for loop 0 -> 255 (fade in)
    // for loop 255 -> 0 (fade out)
    DDRB = 0b00100000; // DDB5 is set to OUTPUT
    while (1) {
        PORTB = 0b00100000; // PB5 is set to HIGH
        _delay_ms(2000);
        PORTB = 0b00000000; // PB5 is set to LOW
        _delay_ms(1000);
    }
}
