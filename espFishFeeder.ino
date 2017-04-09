/*  Fish feeder alexa AND web control */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <fauxmoESP.h>
#include <ESPAsyncTCP.h>
#include <Ticker.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
Ticker defer;
#define SERIAL_BAUDRATE                 115200
const char* ssid = "FiOS-SGVFL";            // Replace with your network credentials
const char* password = "jib7584rows3699tan";
int openPin = 14; //pin for pressing the open button on vent miser
int closePin = 12; //pin for pressing the close button on vent miser
fauxmoESP fauxmo;
MDNSResponder mdns;
ESP8266WebServer server(80);

String webPage = "";
String webPage2 = "";

void wifiSetup() {
 
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);
 
    // Connect
    Serial.printf("[WIFI] Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
 
    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
 
}
void setup() {
    webPage += "<h1>Fish Feeder</h1><p><a href=\"feedfish\"><button>Feed</button></a>&nbsp;</p>";
    pinMode(openPin, OUTPUT);
    digitalWrite(openPin, LOW);
    pinMode(closePin, OUTPUT);
    digitalWrite(closePin, LOW);
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();
 
    // Wifi
    wifiSetup();

      if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
    }

    server.on("/", [](){
    server.send(200, "text/html", webPage);
    });

   server.on("/feedfish", [](){
    webPage2 = "<h1>Fish Feeder</h1><p><a href=\"feedfish\"><button>Feed</button></a>&nbsp;</p><p><h2>Fish fed!</h2><p>";
    server.send(200, "text/html", webPage2);
    digitalWrite(openPin, HIGH);
  defer.once_ms(250, turnOFF, openPin);
  Serial.println("Fish fed!");
  });
  
  server.begin();
  Serial.println("HTTP server started");
 
    // Fauxmo v2.0 fish feeder "feed" command
    fauxmo.addDevice("fish feeder");
    fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
        if (state) {
  digitalWrite(openPin, HIGH);
  defer.once_ms(3000, turnOFF, openPin);
}
        if (!state) {
  digitalWrite(closePin, HIGH);
  defer.once_ms(3000, turnOFF, closePin);
}    
    });
         
}

void loop() {
    server.handleClient();
    fauxmo.handle();
}
void turnON(int sendPin) {
  digitalWrite(sendPin, HIGH);
}
void turnOFF(int sendPin) {
  digitalWrite(sendPin, LOW);
}
