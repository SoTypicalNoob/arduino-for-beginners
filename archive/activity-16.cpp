#include <Arduino.h>
#include <LiquidCrystal.h>
#include "serial_input.h"

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

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);

    lcd.setCursor(0, 0);
    (void)lcd.print("Hello");
}

void loop() {
    static SerialReader reader = {0};
    static int activeLine = 0;
    char line[17];
    static char backupLine[17];
    if (checkSerial(&reader, line, sizeof(line))) {
        lcd.clear();
        if (activeLine == 0) {
            (void)lcd.setCursor(0, activeLine);
            (void)lcd.print(line);
            (void)lcd.setCursor(0, 1);
            (void)lcd.print(backupLine);
            activeLine = 1;
            strcpy(backupLine, line);
        } else if (activeLine == 1) {
            (void)lcd.setCursor(0, activeLine);
            (void)lcd.print(line);
            (void)lcd.setCursor(0, 0);
            (void)lcd.print(backupLine);
            activeLine = 0;
            strcpy(backupLine, line);
        }
    }
}
