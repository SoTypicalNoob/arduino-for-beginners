#include <Arduino.h>

#define LED_1_PIN 12
#define LED_2_PIN 11

unsigned long previousTimeLED1Blink = millis();
unsigned long timeIntervalLED1Blink = 470;
int LED1State = LOW;


void setup() {
    Serial.begin(115200);
    Serial.setTimeout(300);
    delay(500);

    pinMode(LED_1_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    if (Serial.available() > 0) {
        int var = Serial.parseInt();
        Serial.println(var);
        if ((var >= 100) && (var <= 1000)) {
            timeIntervalLED1Blink = var;
        };
    };

    unsigned long timeNow = millis();
    if (timeNow - previousTimeLED1Blink > timeIntervalLED1Blink) {
        if (LED1State == LOW) {
            LED1State = HIGH;
        }
        else {
            LED1State = LOW;
        }
        digitalWrite(LED_1_PIN, LED1State);
        previousTimeLED1Blink += timeIntervalLED1Blink;
    }
}
