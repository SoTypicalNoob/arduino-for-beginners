#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>

/* ---- millis via Timer0 ---- */
static volatile unsigned long ms_count;

ISR(TIMER0_COMPA_vect) { ms_count++; }

static unsigned long millis(void) {
    unsigned long m;
    cli(); m = ms_count; sei();
    return m;
}

static void timer0_init(void) {
    TCCR0A = (1 << WGM01);            /* CTC mode */
    TCCR0B = (1 << CS01) | (1 << CS00); /* prescaler 64 */
    OCR0A  = 249;                      /* 16MHz/64/250 = 1kHz */
    TIMSK0 = (1 << OCIE0A);
}

/* ---- USART (115200 @ 16MHz) ---- */
static void uart_init(void) {
    UBRR0 = 8;                        /* 115200 baud (U2X=1) */
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8N1 */
}

static void uart_putc(char c) { while (!(UCSR0A & (1 << UDRE0))); UDR0 = c; }

static void uart_puts(const char *s) { while (*s) uart_putc(*s++); }

static void uart_putu(unsigned int v) {
    char b[6]; utoa(v, b, 10); uart_puts(b);
}

/* ---- ADC ---- */
static void adc_init(void) {
    ADMUX  = (1 << REFS0);            /* AVcc ref */
    ADCSRA = (1 << ADEN) | 0x07;     /* enable, prescaler 128 */
}

static uint16_t adc_read(uint8_t ch) {
    ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

/* ---- PWM on PB2 (pin 10, OC1B) ---- */
static void pwm_init(void) {
    DDRB |= (1 << PB2);
    TCCR1A = (1 << COM1B1) | (1 << WGM10);  /* 8-bit fast PWM */
    TCCR1B = (1 << WGM12) | (1 << CS11);     /* prescaler 8 */
}

static void pwm_set(uint8_t val) { OCR1B = val; }

/* ---- Serial input (non-blocking) ---- */
static int read_serial_nb(void) {
    static char buf[8];
    static uint8_t idx;
    while (UCSR0A & (1 << RXC0)) {
        char c = UDR0;
        if (c == '\n' || c == '\r') {
            if (idx > 0 && idx < sizeof(buf)) {
                buf[idx] = '\0';
                char *end;
                long val = strtol(buf, &end, 10);
                idx = 0;
                if (end != buf && *end == '\0') return (int)val;
                return -1;
            }
            idx = 0;
        } else if (c >= '0' && c <= '9' && idx < sizeof(buf) - 1) {
            buf[idx++] = c;
        }
    }
    return -1;
}

/* ---- ASSERT: fast-blink PB5 (built-in LED) on failure ---- */
#define ASSERT(cond) do { if (!(cond)) { \
    DDRB |= (1<<PB5); \
    for(;;){ PINB = (1<<PB5); for(volatile uint16_t i=0;i<60000;i++); } \
} } while(0)

/* ---- main ---- */
int main(void) {
    timer0_init();
    uart_init();
    adc_init();
    pwm_init();
    DDRB |= (1 << PB5);  /* built-in LED output */
    sei();

    uint8_t serial_input = eeprom_read_byte((const uint8_t *)0);
    uart_puts("Stored serialInput: "); uart_putu(serial_input); uart_puts("\r\n");
    uart_puts("Serial is ready for use.\r\n");

    unsigned long prev_fade = millis(), prev_print = millis();
    int cur = 0;
    uint8_t fading_in = 1;

    for (;;) {
        unsigned long now = millis();
        int pot = adc_read(2);
        int brightness = pot / 4;
        ASSERT(brightness >= 0 && brightness <= 255);

        int v = read_serial_nb();
        if (v >= 0 && v <= 255) {
            uart_puts("Read value: "); uart_putu(v); uart_puts("\r\n");
            serial_input = v;
            eeprom_update_byte((uint8_t *)0, serial_input);
        }

        if (brightness > serial_input) brightness = serial_input;
        if (cur > brightness) cur = brightness;

        if (now - prev_fade >= 50) {
            prev_fade += 50;
            cur += fading_in ? 1 : -1;
            if (cur >= brightness) fading_in = 0;
            if (cur <= 0) fading_in = 1;
            pwm_set(cur);
        }

        if (now - prev_print >= 3000) {
            prev_print += 3000;
            uart_puts("serialInput: ");       uart_putu(serial_input); uart_puts("\r\n");
            uart_puts("LEDBrightness: ");     uart_putu(brightness);   uart_puts("\r\n");
            uart_puts("Stored serialInput: "); uart_putu(eeprom_read_byte((const uint8_t *)0)); uart_puts("\r\n");
        }
    }
}
