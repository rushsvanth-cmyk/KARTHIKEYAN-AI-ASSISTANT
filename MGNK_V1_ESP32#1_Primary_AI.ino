/*
  MGNK Robot V1 - ESP32 #1 (Main AI Brain - Basic Setup)
  Developer: Karthikeyan Chairman
  Language: C++
*/

#include <WiFi.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Communication Pins for ESP32 #2
#define RXD2 16
#define TXD2 17

void setup() {
  // Serial Monitor for Debugging
  Serial.begin(115200);
  
  // Serial2 for Communication with ESP32 #2
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("MGNK V1 - ESP32 #1 Starting...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Connected Successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Sending test pulse to ESP32 #2 every 5 seconds
  Serial.println("ESP32 #1: Active and Ready.");
  Serial2.println("ESP32_1_ONLINE");
  
  delay(5000);
}
