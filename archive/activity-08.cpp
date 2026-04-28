#include <Arduino.h>

int blink = 100;
int LEDState = LOW;

void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.setTimeout(1000);
    pinMode(12, OUTPUT);
    digitalWrite(12, LOW);
}

// the loop function runs over and over again forever
void loop() {
    if (Serial.available() > 0) {
        int var = Serial.parseInt();
        Serial.println(var);
        if ((var >= 100) && (var <= 1000)) {
            blink = var;
        };
    };

    if (LEDState == LOW) {
        LEDState = HIGH;
    } else {
        LEDState = LOW;
    }
    digitalWrite(12, LEDState);
    delay(blink);
}
