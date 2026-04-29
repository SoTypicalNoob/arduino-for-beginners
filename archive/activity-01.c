#include <avr/io.h>
#ifndef F_CPU // Must be defined before util/delay.h
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#define USART_BAUDRATE 115200
#define UBRR_VALUE ((F_CPU / (8UL * USART_BAUDRATE)) - 1)

void USARTInit(void) {
    // Set baud rate registers
    UBRR0H = (uint8_t) (UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    
    // Enable double speed mode for better baud rate accuracy
    UCSR0A = (1 << U2X0);

    // Set data type to 8 data bits, no parity, 1 stop bit
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    // Enable transmission and reception
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void USARTSendByte(unsigned char u8Data) {
    // Wait while previous byte sent
    while(!(UCSR0A & (1 << UDRE0))) {};

    // Transmit data
    UDR0 = u8Data;
}

void sendString(char myString[]) {
    uint8_t a = 0;
    while (myString[a]) {
        USARTSendByte(myString[a]);
        a++;
    }
}

int main(void) {
    // Internal led was used on Arduino:
    // Arduino: Digital pin 13
    // Atmega328P: PB5
    DDRB = 0b00100000; // DDB5 is set to OUTPUT

    // Initialize USART0
    USARTInit();

    char LEDon[] = "LED is ON\r\n";
    char LEDoff[] = "LED is off\r\n";

    while (1) {
        PORTB = 0b00100000; // PB5 is set to HIGH
        sendString(LEDon);
        _delay_ms(2000);
        PORTB = 0b00000000; // PB5 is set to LOW
        sendString(LEDoff);
        _delay_ms(1000);
    }
}
