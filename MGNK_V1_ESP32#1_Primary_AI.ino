/*
  Project: MGNK Robot V1 - Primary AI Brain (Version 1.2 Update)
  File Title: MGNK_V1_ESP32_Primary_AI_Brain.ino
  Developer: Karthikeyan Chairman
  Architecture: 
    - ESP32 #1: Gemini AI + Wi-Fi Brain (Master)
    - ESP32 #2: Audio & TTS Engine (Slave)
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
    http.setTimeout(10000); 

    // Dynamic JSON Construction with Persona Setup
    JsonDocument doc;
    JsonArray contents = doc["contents"].to<JsonArray>();
    JsonObject contentObj = contents.add<JsonObject>();
    JsonArray parts = contentObj["parts"].to<JsonArray>();
    JsonObject partObj = parts.add<JsonObject>();
    
    // Updated System Prompting: Friendly Assistant Persona
    String promptInstruction = "You are MGNK Robot V1, created by Karthikeyan Chairman. Respond in 1 short, enthusiastic sentence for voice playback: " + userQuery;
    partObj["text"] = promptInstruction;

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
  Serial.println("  MGNK V1 - Primary AI Brain (v1.2 Dynamic Update)");
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

  // Boot-up AI Greeting
  sendToGemini("System startup complete. Greet Karthikeyan Chairman politely.");
}

void loop() {
  // 1. Process Hardware Sensor Inputs from Arduino
  if (Serial1.available() > 0) {
    String sensorData = Serial1.readStringUntil('\n');
    sensorData.trim();
    
    Serial.println("[Arduino Event Received]: " + sensorData);

    if (sensorData.indexOf("Human Motion Detected") >= 0) {
      sendToGemini("A person just walked in front of you. Welcome them warmly.");
    } else if (sensorData.indexOf("Object Near Robot") >= 0) {
      sendToGemini("An obstacle is detected very close. Say a quick caution message.");
    }
  }

  // 2. NEW: Process Manual/Voice Command Input via Serial Monitor
  if (Serial.available() > 0) {
    String manualQuery = Serial.readStringUntil('\n');
    manualQuery.trim();
    if (manualQuery.length() > 0) {
      Serial.println("[Direct Query Received]: " + manualQuery);
      sendToGemini(manualQuery);
    }
  }

  delay(100); 
}
