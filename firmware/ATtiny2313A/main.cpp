#include <pins_arduino.h>
#include <LiquidCrystal.h>
#include <string.h>

int main() {
    // RS, E, DB4, DB5, DB6, DB7
    LiquidCrystal lcd(PIN_PD6, PIN_PB4, PIN_PD2, PIN_PD3, PIN_PD4, PIN_PD5);
    lcd.print("Starting");
}