#include "pins_arduino.h"
#include <Arduino.h>

#define ECHO_PIN 3
#define TRIGGER_PIN 4

#define LED_GREEN 10
#define LED_YELLOW 11
#define LED_RED 12

unsigned long lastTimeUltrasoncTrigger = 0;
unsigned long ultrasonicTriggerDelay = 100;

volatile unsigned long pulseInTimeBegin;
volatile unsigned long pulseInTimeEnd;
volatile bool newDistanceAvailable = false;

void triggerUltrasonicSensor() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
}

double getUltrasonicDistance() {
    double durationMicros = pulseInTimeEnd - pulseInTimeBegin;
    double distance = durationMicros / 58.0;
    // distance = duration * speed
    // speed = 340 m/s --> 0.034 cm/㎲
    // duration * (0.034 /2) = duration / 58
    return distance;
}

void echoPinInterrupt() {
    if (digitalRead(ECHO_PIN) == HIGH) {
        // Signal is rising; start measuring
        pulseInTimeBegin = micros();
  } else {
        // Signal is falling; stop measuring
        pulseInTimeEnd = micros();
        newDistanceAvailable = true;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIGGER_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(ECHO_PIN),
                    echoPinInterrupt,
                    CHANGE);
}

void loop() {
    unsigned long timeNow = millis();
    if (timeNow - lastTimeUltrasoncTrigger > ultrasonicTriggerDelay) {
        lastTimeUltrasoncTrigger += ultrasonicTriggerDelay;
        // trigger sensor
        triggerUltrasonicSensor();
        // read pulse on echo pine
    }

    if (newDistanceAvailable) {
        newDistanceAvailable = false;
        double distance = getUltrasonicDistance();
        Serial.println(distance);

        // distance > 100 cm -> green
        // distance 15-100 cm -> yellow
        // distance < 15 cm -> red

        if (distance > 100) {
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_RED, LOW);
        } else if (distance < 100 && distance > 15) {
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_YELLOW, HIGH);
            digitalWrite(LED_RED, LOW);
        } else {
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_RED, HIGH);
        }
    }
    // action
    delay(300);
}
