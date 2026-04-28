#include <Arduino.h>

#define LED_0_PIN 12
#define LED_1_PIN 11
#define LED_2_PIN 10
#define POTENTIOMETER_PIN A2
#define BUTTON_PIN 2

unsigned long previousTimeLED0Blink = millis();
unsigned long timeIntervalLED0Blink = 470;
int LED0State = LOW;
unsigned long previousTimeLED1Blink = millis();
unsigned long timeIntervalLED1Blink = 470;
int LED1State = 0;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(300);
    delay(500);

    pinMode(LED_0_PIN, OUTPUT);
    pinMode(LED_1_PIN, OUTPUT);
    pinMode(LED_2_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
}

// the loop function runs over and over again forever
void loop() {
    int potentiometerValue = analogRead(POTENTIOMETER_PIN);
    int LEDBrightness = potentiometerValue / 4;
    // analogWrite(LED_1_PIN, LEDBrightness);

    if (Serial.available() > 0) {
        int var = Serial.parseInt();
        Serial.println(var);
        if ((var >= 100) && (var <= 4000)) {
            timeIntervalLED0Blink = var;
        }
    }

    unsigned long timeNow = millis();
    if (timeNow - previousTimeLED0Blink >= timeIntervalLED0Blink) {
        if (LED0State == LOW) {
            LED0State = HIGH;
        }
        else {
            LED0State = LOW;
        }
        digitalWrite(LED_0_PIN, LED0State);
        previousTimeLED0Blink += timeIntervalLED0Blink;
    }

    if (timeNow - previousTimeLED1Blink >= timeIntervalLED1Blink) {
        if (LED1State == 0) {
            LED1State = LEDBrightness;
        }
        else {
            LED1State = 0;
        }
        analogWrite(LED_1_PIN, LED1State);
        previousTimeLED1Blink += timeIntervalLED1Blink;
    }

    if (digitalRead(BUTTON_PIN) == HIGH) {
        digitalWrite(LED_2_PIN, HIGH);
    }
    else {
        digitalWrite(LED_2_PIN, LOW);
    }

}
