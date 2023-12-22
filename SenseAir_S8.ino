// #include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <stdint.h>


unsigned long s8_co2_sum = 0;
unsigned int polls = 0;
unsigned long s8_co2 = 0;

// RS, E, DB4, DB5, DB6, DB7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Arduino Mega | 48 rxPin, 46 txPin
// Arduino Uno | 8 rxPin, 9 txPin
// AltSoftSerial mySerial_s8;
SoftwareSerial mySerial_s8(A2, A3);

uint8_t cmd_s8[7] = {0xFE, 0x44, 0x00, 0x08, 0x02, 0x9F, 0x25}; 
uint8_t response_s8[] = {0, 0, 0, 0, 0, 0, 0};

unsigned int current_minute, lastminute;
int measurement_delay = 10000;
int work_period = 3;
String rate;


void sendRequest_s8(uint8_t packet[]) { 
    mySerial_s8.begin(9600);
    while(!mySerial_s8.available()) {
        mySerial_s8.write(cmd_s8, 7);
        delay(50);
    }
  
    int timeout = 0;
    while(mySerial_s8.available() < 7 ) {
        timeout++; 
        if(timeout > 10) {
            while(mySerial_s8.available())
                mySerial_s8.read(); 
                break;
        } 
        delay(50); 
    } 
    for (int i=0; i < 7; i++) { 
        response_s8[i] = mySerial_s8.read();
    }
    mySerial_s8.end();
}                      

unsigned long getValue_s8(uint8_t packet[]) { 
    int high = packet[3];
    int low = packet[4];
 
    unsigned long val = high * 256 + low;
    return val; 
}

void setup(void) {
    Serial.begin(9600);
    lcd.begin(8, 2);
}

void loop() {
    lcd.print(String("Starting"));
    sendRequest_s8(cmd_s8);
    s8_co2 = getValue_s8(response_s8);

    if (s8_co2 >= 1200) {
        rate = "BAD";
    }
    else if (s8_co2 > 600 && s8_co2 < 1200) {
        rate = "OK";
    } else {
          rate = "FINE";
    }
    
    lcd.clear();
    lcd.print(String(s8_co2) + String(" ppm"));
    lcd.setCursor(0, 1);
    lcd.print(String("CO2 ") + rate);
 
    Serial.println(String(s8_co2));
    delay(measurement_delay);
}

