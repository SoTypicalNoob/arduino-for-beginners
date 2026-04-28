#include <Arduino.h>

#define GREEN_LED_PIN 10
#define YELLOW_LED_PIN 11
#define RED_LED_PIN 12
#define BUTTON_PIN 2

int LEDBlinkState = 1;

byte LEDPinArray[] = {
                                        GREEN_LED_PIN,
                                        YELLOW_LED_PIN,
                                        RED_LED_PIN
                                        };
int LEDPinArraySize = sizeof(LEDPinArray) / sizeof(LEDPinArray[0]);

void setLEDPinModes(byte input[], int arraySize) {
    for (int i = 0; i < arraySize ; i++) {
        pinMode(input[i], OUTPUT);
  }
}

void turnOffAllLEDs(byte input[], int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        digitalWrite(input[i], LOW);
  }
}

void toggleLEDs() {
    if (LEDBlinkState == 1) {
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        LEDBlinkState = 2;
    }
    else {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        LEDBlinkState = 1;
    }
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    setLEDPinModes(LEDPinArray, LEDPinArraySize);
    turnOffAllLEDs(LEDPinArray, LEDPinArraySize);
}

// the loop function runs over and over again forever
void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        toggleLEDs();
    }
    delay(300);
}
