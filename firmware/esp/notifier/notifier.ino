#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const String ssid = "CO2_Detector";
const String password = "12345678";

ESP8266WebServer server(80);

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
  Serial.println(server.arg("ssid"));
  Serial.println(server.arg("password"));

  server.send(200, "text/html", "<h1>Settings saved</h1>");
}

void setup() {
  Serial.begin(115200);
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
