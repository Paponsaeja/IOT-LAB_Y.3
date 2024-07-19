#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

const int flashButtonPin = 0; 

void setup() {  
  Serial.begin(115200);
 
  pinMode(flashButtonPin, INPUT);

  WiFiManager wifiManager;
  
  // Reset settings if the flash button is pressed during boot
  if (digitalRead(flashButtonPin) == LOW) {
    wifiManager.resetSettings();
    Serial.println("WiFi settings reset");
  }
  
  if (!wifiManager.autoConnect("MyESP8266AP")) {
    Serial.println("Failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("Connected to Wi-Fi!");
}

void loop() {
  if (digitalRead(flashButtonPin) == LOW) {
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.reset();
    Serial.println("WiFi settings reset");
    delay(1000); 
  }
  // Your code here
}
