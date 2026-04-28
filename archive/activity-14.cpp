#include <Arduino.h>

#define ECHO_PIN 3
#define TRIGGER_PIN 4

unsigned long lastTimeUltrasoncTrigger = 0;
unsigned long ultrasonicTriggerDelay = 100;

void triggerUltrasonicSensor() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
}

double getUltrasonicDistance() {
    unsigned long codeDuration = micros();
    double durationMicros = pulseIn(ECHO_PIN, HIGH);
    codeDuration = micros() - codeDuration;
    Serial.print("Duration: ");
    Serial.println(codeDuration);
    double distance = durationMicros / 58.0;
    // distance = duration * speed
    // speed = 340 m/s --> 0.034 cm/㎲
    // duration * (0.034 /2) = duration / 58
    return distance;
}

void setup() {
    Serial.begin(115200);
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
}

void loop() {
    unsigned long timeNow = millis();
    if (timeNow - lastTimeUltrasoncTrigger > ultrasonicTriggerDelay) {
        lastTimeUltrasoncTrigger += ultrasonicTriggerDelay;
        // trigger sensor
        triggerUltrasonicSensor();
        // read pulse on echo pine
        Serial.println(getUltrasonicDistance());
    }
    // action
    delay(300);
}
