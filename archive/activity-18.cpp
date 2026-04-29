#include <Arduino.h>
#include <IRremote.h>
#include <LiquidCrystal.h>

// LED
#define LED_PIN_1 12
#define LED_PIN_2 11
#define LED_PIN_3 10

// IR Receiver
#define IR_RECEIVER_PIN 5

// IR remote controller's buttons
#define IR_BUTTON_0 22
#define IR_BUTTON_1 12
#define IR_BUTTON_2 24
#define IR_BUTTON_3 94
#define IR_BUTTON_FUNC_STOP 71

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

const uint8_t LED_PINS[] = {LED_PIN_1, LED_PIN_2, LED_PIN_3};

void toggleLEDsOff() {
    for (uint8_t p : LED_PINS) digitalWrite(p, LOW);
}

void lcdClean() {
    static const char fillup[] = "                ";
    lcd.setCursor(0, 0);
    (void)lcd.print(fillup);
    lcd.setCursor(0, 1);
    (void)lcd.print(fillup);
    lcd.setCursor(0, 0);
}

void showLED(uint16_t command, uint8_t idx) {
    toggleLEDsOff();
    digitalWrite(LED_PINS[idx], HIGH);
    lcdClean();
    (void)lcd.print("Data: ");
    (void)lcd.print(command);
    lcd.setCursor(0, 1);
    (void)lcd.print("Toggle LED ");
    (void)lcd.print(idx + 1);
}

void setup() {
    Serial.begin(115200);

    // IR RECEIVER
    IrReceiver.begin(IR_RECEIVER_PIN);

    // LCD screen
    lcd.begin(16, 2);
    lcd.clear();

    // LED
    for (uint8_t p : LED_PINS) {
        pinMode(p, OUTPUT);
        digitalWrite(p, LOW);
    }
}

void loop() {
    // DONE 0, 1, 2, 3, Func/Stop
    // DONE 0 -> Power off all LEDs
    // DONE 1 -> Toggle LED 1 pin 12
    // DONE 2 -> Toggle LED 2 pin 11
    // DONE 3 -> Toggle LED 3 pin 10

    // DONE LCD: 1st line -> data (command)
    // DONE      2nd line -> "Toggle LED 1"
    // DONE FUNC/STOP -> clear LCD
    // DONE LEDs -> use arays & functions

    if (IrReceiver.decode()) {
        uint16_t command = IrReceiver.decodedIRData.command;
        IrReceiver.resume();

        switch (command) {
            case IR_BUTTON_0:
                toggleLEDsOff();
                lcdClean();
                (void)lcd.print("All LEDs off");
                break;
            case IR_BUTTON_1:
                showLED(command, 0);
                break;
            case IR_BUTTON_2:
                showLED(command, 1);
                break;
            case IR_BUTTON_3:
                showLED(command, 2);
                break;
            case IR_BUTTON_FUNC_STOP:
                lcd.clear();
                break;
            default:
                (void)Serial.print("Wrong button: ");
                (void)Serial.println(command);
                lcdClean();
                (void)lcd.print("Wrong button: ");
                lcd.setCursor(0, 1);
                (void)lcd.print(command);
                break;
        }
    }
}
