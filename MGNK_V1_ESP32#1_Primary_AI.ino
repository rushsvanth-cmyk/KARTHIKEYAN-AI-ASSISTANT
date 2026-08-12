/*
  MGNK Robot V1 - ESP32 #1 (Main AI Brain)
  Developer: Karthikeyan Chairman
  Target Launch: September 25, 2026
  Purpose: Connects to Gemini AI & Sends Response to ESP32 #2 via UART
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi Details
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Your Google Gemini API Key
const char* gemini_api_key = "AIzaSyD-xxxxxx_உங்களிடன்_உள்ள_முழு_API_Key";

// Hardware Serial 2 Pins (Communicating with ESP32 #2)
#define RXD2 16
#define TXD2 17

void sendToGemini(String userQuery) {
  // Check Wi-Fi Connection before sending request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(gemini_api_key);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Constructing JSON Request
    StaticJsonDocument<512> doc;
    JsonArray contents = doc.createNestedArray("contents");
    JsonObject contentObj = contents.createNestedObject();
    JsonArray parts = contentObj.createNestedArray("parts");
    JsonObject partObj = parts.createNestedObject();
    
    // Prompting Gemini for short voice-friendly Tamil/English response
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

        Serial.println("Gemini Response: " + aiAnswer);
        
        // Sending text to ESP32 #2 for Voice Playback via UART
        Serial2.println("TTS_TEXT:" + aiAnswer);
      } else {
        Serial.println("JSON Parsing Failed!");
      }
    } else {
      Serial.print("HTTP Error Code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi Disconnected! Reconnecting...");
    WiFi.begin(ssid, password);
  }
}

void setup() {
  Serial.begin(115200);
  
  // UART Connection to ESP32 #2 setup
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("\n--------------------------------------------------");
  Serial.println("MGNK V1 - ESP32 #1 Primary AI Brain Initializing...");
  Serial.println("--------------------------------------------------");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nStatus: Wi-Fi Connected Successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("--------------------------------------------------");
}

void loop() {
  // Testing Request
  Serial.println("\nSending Request to Gemini AI...");
  sendToGemini("Hello Gemini, introduce MGNK Robot V1 in 1 short line.");

  // Delay between AI requests for testing
  delay(15000); 
}
