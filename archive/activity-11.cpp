#include <Arduino.h>

#define LED_1_PIN 12
#define LED_2_PIN 11
#define LED_3_PIN 10
#define BUTTON_PIN 2

// millis() is 0 or uncertain before setup()
// so, to make it safe, it is set to 0
unsigned long lastTimeButtonChanged = 0;
// This wasn't supposed to be changed, so const or constexpr
// constexpr are calculated at compile time
constexpr unsigned long debounceDelay = 50;

byte buttonState = LOW;

// Blink LED 1.
// millis() is 0 or uncertain before setup()
// so, to make it safe, it is set to 0
unsigned long previousTimeLED1Blink = 0;
// This wasn't supposed to be changed, so const or constexpr
// constexpr are calculated at compile time
constexpr unsigned long timeIntervalLED1Blink = 1000;
byte LED1State = LOW;

// Toggle LED 2 and LED 3
byte LED2State = HIGH;
byte LED3State = LOW;

void setup() {
    Serial.begin(115200);
    pinMode(LED_1_PIN, OUTPUT);
    pinMode(LED_2_PIN, OUTPUT);
    pinMode(LED_3_PIN, OUTPUT);

    pinMode(BUTTON_PIN, INPUT);
    buttonState = digitalRead(BUTTON_PIN);

    digitalWrite(LED_2_PIN, LED2State);
    digitalWrite(LED_3_PIN, LED3State);

    lastTimeButtonChanged = millis();
    previousTimeLED1Blink = millis();
}

void loop() {
    unsigned long timeNow = millis();
    if (timeNow - lastTimeButtonChanged >= debounceDelay) {
        byte newButtonState = digitalRead(BUTTON_PIN);
        if (newButtonState != buttonState) {
            lastTimeButtonChanged = timeNow;
            buttonState = newButtonState;
            // do action
            if (buttonState == HIGH) {
                LED2State = !LED2State;
                digitalWrite(LED_2_PIN, LED2State);

                LED3State = !LED3State;
                digitalWrite(LED_3_PIN, LED3State);
            }
        }
    }

    if (timeNow - previousTimeLED1Blink >= timeIntervalLED1Blink) {
        LED1State = !LED1State;
        digitalWrite(LED_1_PIN, LED1State);
        previousTimeLED1Blink += timeIntervalLED1Blink;
    }
}
