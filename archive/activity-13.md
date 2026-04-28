# main.c — Pure AVR C Port: Detailed Comparison with Arduino Version

This document explains every block in `main.c` and how it replaces the corresponding Arduino API from `main.cpp`.

---

## Includes

### Arduino version
```cpp
#include <Arduino.h>
#include <EEPROM.h>
```
`Arduino.h` pulls in the entire Arduino core: pin abstractions, Serial class, timing functions, etc. `EEPROM.h` provides the Arduino EEPROM library (a C++ class).

### Pure C version
```c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
```
- `avr/io.h` — defines all hardware register names (DDRB, PORTB, ADMUX, TCCR1A, etc.) for the target MCU.
- `avr/interrupt.h` — provides `ISR()` macro, `sei()`, `cli()` for interrupt handling.
- `avr/eeprom.h` — low-level EEPROM access functions (`eeprom_read_byte`, `eeprom_update_byte`).
- `stdlib.h` — for `strtol()` and `utoa()`.

---

## Millisecond Timer (`millis()` replacement)

### Arduino version
Arduino provides `millis()` out of the box. Internally it uses Timer0 with an overflow ISR — but this is hidden from the user.

### Pure C version
```c
static volatile unsigned long ms_count;

ISR(TIMER0_COMPA_vect) { ms_count++; }

static unsigned long millis(void) {
    unsigned long m;
    cli(); m = ms_count; sei();
    return m;
}

static void timer0_init(void) {
    TCCR0A = (1 << WGM01);              /* CTC mode */
    TCCR0B = (1 << CS01) | (1 << CS00); /* prescaler 64 */
    OCR0A  = 249;                        /* 16MHz / 64 / 250 = 1kHz */
    TIMSK0 = (1 << OCIE0A);
}
```
- Timer0 is configured in CTC (Clear Timer on Compare) mode.
- With a 64 prescaler and compare value of 249, the interrupt fires every 1ms: `16,000,000 / 64 / 250 = 1000 Hz`.
- `ms_count` is `volatile` because it's modified inside an ISR.
- `millis()` disables interrupts (`cli()`) while reading the 4-byte counter to prevent a torn read, then re-enables (`sei()`).

---

## USART (Serial replacement)

### Arduino version
```cpp
Serial.begin(115200);
Serial.print("text");
Serial.println(value);
Serial.available();
Serial.read();
```
The Arduino `Serial` object is a C++ class wrapping the USART hardware with ring buffers and interrupt-driven TX/RX.

### Pure C version
```c
static void uart_init(void) {
    UBRR0 = 8;                        /* 115200 baud with U2X=1 */
    UCSR0A = (1 << U2X0);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8N1 */
}
```
- `UBRR0 = 8` sets the baud rate register. With double-speed mode (`U2X0`), the formula is: `UBRR = F_CPU / (8 * baud) - 1 = 16000000 / (8 * 115200) - 1 ≈ 8`.
- `UCSR0B` enables the receiver (`RXEN0`) and transmitter (`TXEN0`).
- `UCSR0C` sets 8 data bits, no parity, 1 stop bit (8N1).

```c
static void uart_putc(char c) { while (!(UCSR0A & (1 << UDRE0))); UDR0 = c; }
static void uart_puts(const char *s) { while (*s) uart_putc(*s++); }
static void uart_putu(unsigned int v) { char b[6]; utoa(v, b, 10); uart_puts(b); }
```
- `uart_putc` — polls the UDRE0 (Data Register Empty) flag, then writes to `UDR0`. This replaces `Serial.write()`.
- `uart_puts` — sends a null-terminated string character by character. Replaces `Serial.print(const char*)`.
- `uart_putu` — converts an unsigned int to string via `utoa()` and prints it. Replaces `Serial.print(int)` / `Serial.println(int)`.
- Line endings: we explicitly send `"\r\n"` instead of relying on `Serial.println()`.

Note: this is a polling (blocking) implementation. Arduino uses interrupt-driven buffered TX, which is non-blocking until the buffer fills. For this application the difference is negligible.

---

## ADC (analogRead replacement)

### Arduino version
```cpp
int potentiometerValue = analogRead(POTENTIOMETER_PIN);
```

### Pure C version
```c
static void adc_init(void) {
    ADMUX  = (1 << REFS0);        /* AVcc as reference voltage */
    ADCSRA = (1 << ADEN) | 0x07;  /* enable ADC, prescaler 128 */
}

static uint16_t adc_read(uint8_t ch) {
    ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);  /* select channel, keep ref */
    ADCSRA |= (1 << ADSC);                  /* start conversion */
    while (ADCSRA & (1 << ADSC));            /* wait for completion */
    return ADC;                              /* read 10-bit result */
}
```
- `REFS0` selects AVcc (5V) as the ADC reference — same as Arduino's default.
- Prescaler 128 gives an ADC clock of `16MHz / 128 = 125kHz`, within the recommended 50–200kHz range.
- `adc_read(2)` selects channel 2 (pin A2), starts a conversion, busy-waits, and returns the 10-bit result (0–1023) — identical behavior to `analogRead(A2)`.

---

## PWM (analogWrite replacement)

### Arduino version
```cpp
constexpr int LED_PIN = 10;
pinMode(LED_PIN, OUTPUT);
analogWrite(LED_PIN, currentBrightness);
```
Arduino's `analogWrite()` automatically configures the appropriate timer for the pin.

### Pure C version
```c
static void pwm_init(void) {
    DDRB |= (1 << PB2);                          /* pin 10 = PB2, set as output */
    TCCR1A = (1 << COM1B1) | (1 << WGM10);       /* non-inverting PWM on OC1B, 8-bit fast PWM */
    TCCR1B = (1 << WGM12) | (1 << CS11);          /* fast PWM mode, prescaler 8 */
}

static void pwm_set(uint8_t val) { OCR1B = val; }
```
- Arduino pin 10 maps to port B, bit 2 (`PB2`), which is the Timer1 channel B output (`OC1B`).
- `DDRB |= (1 << PB2)` sets the pin as output — equivalent to `pinMode(10, OUTPUT)`.
- Timer1 is configured for 8-bit fast PWM mode (WGM10 + WGM12).
- `COM1B1` enables non-inverting PWM output on OC1B: the pin goes HIGH at timer bottom and LOW when the counter matches `OCR1B`.
- Writing to `OCR1B` sets the duty cycle (0–255) — equivalent to `analogWrite(10, value)`.

---

## Serial Input (readSerialNonBlocking replacement)

### Arduino version
```cpp
while (Serial.available() > 0) {
    char c = Serial.read();
    ...
}
```

### Pure C version
```c
while (UCSR0A & (1 << RXC0)) {
    char c = UDR0;
    ...
}
```
- `RXC0` flag in `UCSR0A` indicates a byte is available in the receive buffer — equivalent to `Serial.available() > 0`.
- Reading `UDR0` retrieves the byte and clears the flag — equivalent to `Serial.read()`.
- The rest of the parsing logic (buffering digits, converting with `strtol`) is identical.

---

## EEPROM

### Arduino version
```cpp
EEPROM.read(0)
EEPROM.update(0, serialInput)
```

### Pure C version
```c
eeprom_read_byte((const uint8_t *)0)
eeprom_update_byte((uint8_t *)0, serial_input)
```
- Direct 1:1 replacement. The `avr/eeprom.h` functions take a pointer to the EEPROM address (cast from integer 0).
- `eeprom_update_byte` only writes if the value differs (same as Arduino's `EEPROM.update`), reducing EEPROM wear.

---

## ASSERT Macro

### Arduino version
```cpp
#define ASSERT(cond) do { if (!(cond)) { while (true) { \
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(100); \
} } } while(0)
```
Uses `digitalWrite`/`digitalRead` to toggle the built-in LED and `delay()` for timing.

### Pure C version
```c
#define ASSERT(cond) do { if (!(cond)) { \
    DDRB |= (1<<PB5); \
    for(;;){ PINB = (1<<PB5); for(volatile uint16_t i=0;i<60000;i++); } \
} } while(0)
```
- `LED_BUILTIN` on the Uno is pin 13 = `PB5`.
- `DDRB |= (1<<PB5)` ensures the pin is set as output.
- Writing to `PINB` toggles the corresponding pin (ATmega328P hardware feature) — replaces `digitalWrite(pin, !digitalRead(pin))`.
- The inner `volatile` loop provides a rough delay (~100ms at 16MHz) instead of `delay(100)`. We can't use `delay()` or `_delay_ms()` here because in a failure state we want zero dependencies on timer interrupts.

---

## Program Entry Point

### Arduino version
```cpp
void setup() { ... }
void loop() { ... }
```
Arduino's hidden `main()` calls `setup()` once, then calls `loop()` repeatedly.

### Pure C version
```c
int main(void) {
    /* init code (was in setup) */
    timer0_init();
    uart_init();
    adc_init();
    pwm_init();
    DDRB |= (1 << PB5);
    sei();                  /* enable global interrupts */

    /* ... */

    for (;;) {
        /* loop code (was in loop) */
    }
}
```
- Standard C entry point. No hidden framework code.
- All peripheral initialization that Arduino did behind the scenes (`init()` function) must now be done explicitly: timers, UART, ADC, pin directions.
- `sei()` enables global interrupts — Arduino's `init()` does this automatically.
- `for (;;)` is the infinite loop, replacing Arduino's repeated calls to `loop()`.
- `DDRB |= (1 << PB5)` replaces `pinMode(LED_BUILTIN, OUTPUT)`.

---

## GPIO Pin Mapping Reference

| Arduino Name | AVR Port/Bit | Register |
|---|---|---|
| `LED_BUILTIN` (pin 13) | PB5 | DDRB/PORTB/PINB bit 5 |
| Pin 10 | PB2 (OC1B) | DDRB bit 2, Timer1 OCR1B |
| A2 | ADC channel 2 | ADMUX channel select |

---

## Summary of Removed Dependencies

| Arduino Feature | Status in Pure C |
|---|---|
| `Arduino.h` core | Replaced by direct register access |
| `Serial` class | Replaced by USART register polling |
| `EEPROM` class | Replaced by `avr/eeprom.h` functions |
| `millis()` | Reimplemented via Timer0 CTC ISR |
| `analogRead()` | Reimplemented via ADC registers |
| `analogWrite()` | Reimplemented via Timer1 PWM registers |
| `pinMode()` | Replaced by DDRx register writes |
| `digitalWrite()`/`digitalRead()` | Replaced by PORTx/PINx register access |
| `delay()` | Replaced by busy-wait loop (in ASSERT only) |
