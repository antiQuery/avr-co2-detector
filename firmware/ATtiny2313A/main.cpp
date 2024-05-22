#include <pins_arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include <SendOnlySoftwareSerial.h> //for send only
#include <util/delay.h>

// RS, E, DB4, DB5, DB6, DB7
LiquidCrystal lcd(PIN_PD6, PIN_PB4, PIN_PD2, PIN_PD3, PIN_PD4, PIN_PD5);

SendOnlySoftwareSerial mySerial(PIN_PB2);  // Tx pin

uint8_t cmd_s8[7] = {0xFE, 0x44, 0x00, 0x08, 0x02, 0x9F, 0x25};

unsigned short sendRequest_s8() {
    uint8_t response_s8[] = {0, 0, 0, 0, 0, 0, 0};
    
    for (int i = 0; i < 7; i++) {
        while ((UCSRA & 0x20) != 0x20) { //wait for high UDRE bit
        }
        
        UDR = cmd_s8[i];
    }
    
    int i = 0;
    while(i < 7) {
        if ((UCSRA & 0x80) == 0x80) { //wait for high RXC bit
            response_s8[i] = UDR;
            i++;
        }
    }
    
    uint8_t high = response_s8[3];
    uint8_t low = response_s8[4];
 
    unsigned short val = high * 256 + low;
    return val;
}

void loop() {
    char rate[9];
    
    unsigned short s8_co2 = sendRequest_s8();
    
    lcd.clear();
    itoa(s8_co2, rate, 10);
    strcat(rate, " ppm");
    lcd.print(rate);
    
    if (s8_co2 >= 1200) {
        strcpy(rate, "BAD");
        mySerial.write('2'); //send BAD message to ESP-07
    }
    else if (s8_co2 > 600 && s8_co2 < 1200) {
        strcpy(rate, "OK");
        mySerial.write('1'); //send OK message to ESP-07
    } 
    else {
        strcpy(rate, "FINE");
        mySerial.write('0'); //send FINE message to ESP-07
    }
    
    lcd.setCursor(0, 1);
    strcat(rate, " CO2");
    lcd.print(rate);
    
    _delay_ms(1000);
}

int main() {
    //Initialize Hardware UART for receive from SenseAir S8
    UCSRB = UCSRB | 0x18; //Enable Receive and Transmit (set RXEN, TXEN bit)
    UBRRL = 25; // u2x = 0; fosc = 4.0000 MHz; Baud Rate = 9600 => ubrr = 25
    
    lcd.begin(8, 2);
    
    mySerial.begin(9600);
    
    while (true) {
        loop();
    }
}
