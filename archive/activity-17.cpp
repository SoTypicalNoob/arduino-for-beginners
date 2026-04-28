#include <Arduino.h>
#include <LiquidCrystal.h>
#include "serial_input.h"

// ultrasonic
#define ECHO_PIN 3
#define TRIGGER_PIN 4

// lcd
#define LCD_RS_PIN A5
#define LCD_E_PIN  A4
#define LCD_D4_PIN 6
#define LCD_D5_PIN 7
#define LCD_D6_PIN 8
#define LCD_D7_PIN 9

LiquidCrystal lcd(LCD_RS_PIN,
                  LCD_E_PIN,
                  LCD_D4_PIN,
                  LCD_D5_PIN,
                  LCD_D6_PIN,
                  LCD_D7_PIN);

unsigned long lastTimeUltrasoncTrigger = 0;
unsigned long ultrasonicTriggerDelay = 500;

volatile unsigned long pulseInTimeBegin;
volatile unsigned long pulseInTimeEnd;
volatile bool newDistanceAvailable = false;

void triggerUltrasonicSensor() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
}

double getUltrasonicDistance() {
    double durationMicros = pulseInTimeEnd - pulseInTimeBegin;
    // double distance = durationMicros / 58.0;
    // distance = duration * speed
    // speed = 340 m/s --> 0.034 cm/㎲
    // duration * (0.034 /2) = duration / 58
    return durationMicros;
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
    lcd.begin(16, 2);

    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIGGER_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(ECHO_PIN),
                    echoPinInterrupt,
                    CHANGE);

    lcd.setCursor(0, 0);
}

void loop() {
    // line 1: "rate: x ms"
    // line 2: "distance: x cm"

    unsigned long timeNow = millis();
    if (timeNow - lastTimeUltrasoncTrigger > ultrasonicTriggerDelay) {
        lastTimeUltrasoncTrigger += ultrasonicTriggerDelay;
        // trigger sensor
        triggerUltrasonicSensor();
        // read pulse on echo pine
    }

    if (newDistanceAvailable) {
        newDistanceAvailable = false;
        double durationMicros = getUltrasonicDistance();
        double distance = durationMicros / 58.0;
        lcd.setCursor(0, 0);
        lcd.print("rate: ");
        lcd.print(durationMicros / 1000.0);
        lcd.print(" ms");
        lcd.print("    ");
        lcd.setCursor(0, 1);
        lcd.print("dst.: ");
        lcd.print(distance);
        lcd.print(" cm");
        lcd.print("    ");
    }
}
