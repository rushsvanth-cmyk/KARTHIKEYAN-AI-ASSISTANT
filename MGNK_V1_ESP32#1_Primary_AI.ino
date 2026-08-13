/*
  Project: MGNK Robot V1 - ESP32 #1 (Main AI Brain)
  Developer: Karthikeyan Chairman
  Target Launch: September 25, 2026
  Architecture: 
    - ESP32 #1: Gemini AI + Wi-Fi Brain
    - ESP32 #2: Audio & TTS Engine
    - Arduino: Sensors & Hardware Control
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Google Gemini API Key
const char* gemini_api_key = "AIzaSyD-xxxxxx_உங்களிடன்_உள்ள_முழு_API_Key";

// Hardware Serial 2 Pins (Communicating with ESP32 #2 - Audio Engine)
#define TXD2 17
#define RXD2 16

// Hardware Serial 1 Pins (Communicating with Arduino - Sensors)
#define TXD1 10
#define RXD1 9

void sendToGemini(String userQuery) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(gemini_api_key);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Dynamic JSON Construction
    StaticJsonDocument<512> doc;
    JsonArray contents = doc.createNestedArray("contents");
    JsonObject contentObj = contents.createNestedObject();
    JsonArray parts = contentObj.createNestedArray("parts");
    JsonObject partObj = parts.createNestedObject();
    
    // Voice-optimized short response
    partObj["text"] = "Answer in 1 or 2 short sentences for voice output: " + userQuery;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      
      StaticJsonDocument<1024> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        String aiAnswer = responseDoc["candidates"][0]["content"]["parts"][0]["text"];

        Serial.println("\n[AI Output]: " + aiAnswer);
        
        // Send AI answer to ESP32 #2 for Voice Playback
        Serial2.println("TTS_TEXT:" + aiAnswer);
      } else {
        Serial.println("\n[Error]: JSON Parsing Failed!");
      }
    } else {
      Serial.print("\n[HTTP Error]: Code ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("\n[Wi-Fi Error]: Disconnected! Reconnecting...");
    WiFi.begin(ssid, password);
  }
}

void setup() {
  // Serial Monitor for Debugging
  Serial.begin(115200);
  
  // UART Connection to ESP32 #2 (Audio Output)
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // UART Connection to Arduino (Sensor Data)
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);

  Serial.println("\n==================================================");
  Serial.println("  MGNK V1 - ESP32 #1 Primary AI Brain Initialized ");
  Serial.println("==================================================");

  // Connecting to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nStatus: Wi-Fi Connected Successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("==================================================");
}

void loop() {
  // Check if Arduino sent sensor alert/trigger
  if (Serial1.available() > 0) {
    String sensorData = Serial1.readStringUntil('\n');
    Serial.println("[Arduino Sensor Event]: " + sensorData);
  }

  // Periodic AI System Test Run
  Serial.println("\n[System]: Requesting Gemini AI Response...");
  sendToGemini("Hello Gemini, introduce MGNK Robot V1 in 1 short line.");

  // Delay for testing stability
  delay(20000); 
}
