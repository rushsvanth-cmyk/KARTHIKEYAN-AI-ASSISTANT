/*
  Project: MGNK Robot V1 - Primary AI Brain
  File Title: MGNK_V1_ESP32_Primary_AI_Brain.ino
  Developer: Karthikeyan Chairman
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
const char* gemini_api_key = "YOUR_GEMINI_API_KEY_HERE";

// Hardware Serial Pins
#define TXD2 17
#define RXD2 16
#define TXD1 10
#define RXD1 9

void sendToGemini(String userQuery) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(gemini_api_key);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000); // 10-second connection timeout

    // Dynamic JSON Construction
    JsonDocument doc;
    JsonArray contents = doc["contents"].to<JsonArray>();
    JsonObject contentObj = contents.add<JsonObject>();
    JsonArray parts = contentObj["parts"].to<JsonArray>();
    JsonObject partObj = parts.add<JsonObject>();
    
    // Voice-optimized short response instruction
    partObj["text"] = "Answer in 1 short sentence for voice playback: " + userQuery;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      JsonDocument responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        const char* aiAnswer = responseDoc["candidates"][0]["content"]["parts"][0]["text"];

        if (aiAnswer) {
          Serial.print("\n[AI Output]: ");
          Serial.println(aiAnswer);
          
          // Send AI output to ESP32 #2 via UART
          Serial2.print("TTS_TEXT:");
          Serial2.println(aiAnswer);
        }
      } else {
        Serial.println("\n[Error]: JSON Parsing Failed!");
      }
    } else {
      Serial.print("\n[HTTP Error]: Code ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("\n[Wi-Fi Error]: Reconnecting...");
    WiFi.begin(ssid, password);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Serial2 for Audio ESP32
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // Serial1 for Arduino Sensors
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);

  Serial.println("\n==================================================");
  Serial.println("  MGNK V1 - Primary AI Brain (Updated Version)   ");
  Serial.println("==================================================");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nStatus: Wi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("==================================================");
}

void loop() {
  // Check for Sensor Triggers from Arduino
  if (Serial1.available() > 0) {
    String sensorData = Serial1.readStringUntil('\n');
    sensorData.trim();
    
    Serial.println("[Arduino Event Received]: " + sensorData);

    // Dynamic AI Prompting based on Sensor Events
    if (sensorData.indexOf("Human Motion Detected") >= 0) {
      sendToGemini("A person just walked in front of you. Greet them politely as MGNK Robot.");
    } else if (sensorData.indexOf("Object Near Robot") >= 0) {
      sendToGemini("An obstacle is detected close to you. Say a quick alert message.");
    }
  }

  delay(100); // Polling delay
}
