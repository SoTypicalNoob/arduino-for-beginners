#include <Arduino.h>

#define BUTTON_PIN 2

volatile unsigned long lastTimeButtonReleased = 0; // millis()
constexpr unsigned long debounceDelay = 200;

volatile bool buttonPushed = false;

int buttonPushCounter = 0;

void buttonPushSignal() {
    buttonPushCounter++;
    Serial.println(buttonPushCounter);
}

void buttonPushedInterrupt() {
    unsigned long timeNow = millis();
    if (timeNow - lastTimeButtonReleased >= debounceDelay) {
        lastTimeButtonReleased = timeNow;
        buttonPushed = true;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                    buttonPushedInterrupt,
                    RISING);
}

void loop() {
    noInterrupts();
    bool pressed = buttonPushed;
    buttonPushed = false;
    interrupts();
    if (pressed) {
        buttonPushSignal();
    }
}
