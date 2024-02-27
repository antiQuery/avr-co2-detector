#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <stdint.h>

#define DEBUG

const String ssid = "CO2_Detector";
const String password = "12345678";

#define SERIAL_MONITOR_BAUD_RATE  115200
#define EEPROM_SIZE               512
#define BASE_EEPROM_OFFSET        0
#define WEB_SERVER_PORT           80

ESP8266WebServer server(WEB_SERVER_PORT);

int writeStringToEEPROM(int addrOffset, const String &strToWrite) {
    uint8_t len = strToWrite.length();
    EEPROM.write(addrOffset, len);

    for (int i = 0; i < len; i++) {
        EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
        EEPROM.commit();
    }

    return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead) {
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];

    for (int i = 0; i < newStrLen; i++) {
        data[i] = EEPROM.read(addrOffset + 1 + i);
    }
    data[newStrLen] = '\0';

    *strToRead = String(data);
    return addrOffset + 1 + newStrLen;
}

void handleRoot() {
    String html = "\
    <html>\
        <head>\
            <title>Configuration Page</title>\
        </head>\
        <body>\
            <h1>Configure your settings</h1>\
            <form action='/save' method='post'>\
                <label for='ssid'>SSID:</label><br>\
                <input type='text' id='ssid' name='ssid'><br>\
                <label for='password'>Password:</label><br>\
                <input type='password' id='password' name='password'><br><br>\
                <input type='submit' value='Save'>\
            </form>\
        </body>\
    </html>";
  
    server.send(200, "text/html", html);
}

void handleSave() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    int ssidAddrOffset = writeStringToEEPROM(BASE_EEPROM_OFFSET, ssid);
    writeStringToEEPROM(ssidAddrOffset, password);

    server.send(200, "text/html", "<h1>Settings saved</h1>");
}

#ifdef DEBUG
void debugReadEEPROM() {
    static String newStr1;
    static String newStr2;

    String str1;
    String str2;

    int newStr1AddrOffset = readStringFromEEPROM(BASE_EEPROM_OFFSET, &str1);
    readStringFromEEPROM(newStr1AddrOffset, &str2);

    if ((str1 != newStr1) || (str2 != newStr2)) {
        newStr1 = str1;
        newStr2 = str2;

        Serial.println(newStr1);
        Serial.println(newStr2);
    }
}

void delayDebug() {
    static int count = 0;
    if (count++ == 50000) {
        count = 0;
        debugReadEEPROM();
    }
}
#endif // DEBUG

void setup() {
    EEPROM.begin(EEPROM_SIZE);
  
    Serial.begin(SERIAL_MONITOR_BAUD_RATE);
    Serial.print("Configuring access point...");
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    Serial.println("HTTP server started");
    
#ifdef DEBUG
    debugReadEEPROM();
#endif // DEBUG
}

void loop() {
    server.handleClient();

#ifdef DEBUG
    delayDebug();
#endif // DEBUG
}
