#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

const int flashButtonPin = 0; 
const char* lineToken = "DXvQXNtlcGv5qX6XR60hYYrhCDp1EeGSEsG9fS0OyHY"; // Replace with your LINE Notify token
ESP8266WebServer server(80);

const int ledPin = LED_BUILTIN; // Built-in LED pin

void setup() {  
  Serial.begin(115200);
 
  pinMode(flashButtonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn off the LED initially (assuming active low)

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
  sendLineNotify("ESP8266 connected to Wi-Fi!"); // Send notification upon successful connection

  // Start the web server
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);
  server.begin();
  Serial.println("HTTP server started");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  
  if (digitalRead(flashButtonPin) == LOW) {
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.reset();
    Serial.println("WiFi settings reset");
    delay(1000); 
  }
  // Your code here
}

void handleRoot() {
  String html = "<h1>ESP8266 Web Server</h1><p><a href=\"/led/on\">Turn LED On</a></p><p><a href=\"/led/off\">Turn LED Off</a></p>";
  server.send(200, "text/html", html);
}

void handleLedOn() {
  digitalWrite(ledPin, LOW); // Turn the LED on (assuming active low)
  server.send(200, "text/plain", "LED is ON");
  sendLineNotify("LED is turned ON");
}

void handleLedOff() {
  digitalWrite(ledPin, HIGH); // Turn the LED off (assuming active low)
  server.send(200, "text/plain", "LED is OFF");
  sendLineNotify("LED is turned OFF");
}

void sendLineNotify(String message) {
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();
  http.begin(client, "https://notify-api.line.me/api/notify");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", String("Bearer ") + lineToken);

  String payload = "message=" + message;
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("Error code: %d\n", httpResponseCode);
  }
  
  http.end();
}
