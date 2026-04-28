#include <Arduino.h>
#include <EEPROM.h>

#define ASSERT(cond) do { if (!(cond)) { while (true) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(100); } } } while(0)

constexpr int LED_PIN = 10;
constexpr int POTENTIOMETER_PIN = A2;

constexpr unsigned long timeIntervalLEDFade = 50;
constexpr unsigned long timeIntervalPrintOut = 3000;

int readSerialNonBlocking() {
    static char buf[8];
    static uint8_t idx = 0;

    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (idx > 0) {
                ASSERT(idx < sizeof(buf));
                buf[idx] = '\0';
                char *end;
                long val = strtol(buf, &end, 10);
                idx = 0;
                if (end != buf && *end == '\0') {
                    return (int)val;
                }
                return -1;
          }
        } else if (c >= '0' && c <= '9' && idx < sizeof(buf) - 1) {
            buf[idx++] = c;
        }
    }
    return -1;
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    // ASSERT(false);
    pinMode(LED_PIN, OUTPUT);
    (void)Serial.print("Stored serialInput: ");
    (void)Serial.println(EEPROM.read(0));
    (void)Serial.println("Serial is ready for use.");
}

void loop() {
    // Requirements:
    // LED on pin 10 - fade in/out from potentiometer
    // max intensity from serial
    // save max intensity to EEPROM

    static unsigned long previousTimeLEDFade = millis();
    static unsigned long previousTimePrintOut = millis();
    unsigned long timeNow = millis();

    int potentiometerValue = analogRead(POTENTIOMETER_PIN);
    static int serialInput = EEPROM.read(0);
    int LEDBrightness = potentiometerValue / 4;
    ASSERT(LEDBrightness >= 0 && LEDBrightness <= 255);

    int var = readSerialNonBlocking();
    if (var >= 0 && var <= 255) {
        (void)Serial.print("Read value: ");
        (void)Serial.println(var);
        serialInput = var;
        EEPROM.update(0, serialInput);
    }

    if (LEDBrightness > serialInput) {
        LEDBrightness = serialInput;
    }

    static int currentBrightness = 0;
    static bool fadingIn = true;
    if (currentBrightness > LEDBrightness) {
        currentBrightness = LEDBrightness;

    }
    if (timeNow - previousTimeLEDFade >= timeIntervalLEDFade) {
        previousTimeLEDFade += timeIntervalLEDFade;
        if (fadingIn) {
            currentBrightness++;
            if (currentBrightness >= LEDBrightness) {
                fadingIn = false;
            }
        } else {
            currentBrightness--;
            if (currentBrightness <= 0) {
                fadingIn = true;
            }
        }
        analogWrite(LED_PIN, currentBrightness);
    }
    if (timeNow - previousTimePrintOut >= timeIntervalPrintOut) {
        (void)Serial.print("serialInput: ");
        (void)Serial.println(serialInput);
        (void)Serial.print("LEDBrightness: ");
        (void)Serial.println(LEDBrightness);
        (void)Serial.print("Stored serialInput: ");
        (void)Serial.println(EEPROM.read(0));
        previousTimePrintOut += timeIntervalPrintOut;
    }
}
