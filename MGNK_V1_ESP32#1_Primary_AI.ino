/*
  MGNK Robot V1 - ESP32 #1 (Main AI Brain)
  Step 4: Gemini AI Integration + Dual Serial
  Developer: Karthikeyan Chairman
  Language: C++
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Your Google Gemini API Key
const char* gemini_api_key = "YOUR_GEMINI_API_KEY";

// Hardware Serial 2 Pins for Communication with ESP32 #2
#define RXD2 16
#define TXD2 17

void sendToGemini(String userQuery) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key=" + String(gemini_api_key);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Constructing JSON Payload
    StaticJsonDocument<512> doc;
    JsonArray contents = doc.createNestedArray("contents");
    JsonObject contentObj = contents.createNestedObject();
    JsonArray parts = contentObj.createNestedArray("parts");
    JsonObject partObj = parts.createNestedObject();
    partObj["text"] = userQuery;

    String requestBody;
    serializeJson(doc, requestBody);

    // Sending POST request to Gemini
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      
      // Parsing Response
      StaticJsonDocument<1024> responseDoc;
      deserializeJson(responseDoc, response);
      String aiAnswer = responseDoc["candidates"][0]["content"]["parts"][0]["text"];

      Serial.println("Gemini Response: " + aiAnswer);
      
      // Sending AI Answer to ESP32 #2 via UART
      Serial2.println("AI_RESP:" + aiAnswer);
    } else {
      Serial.print("Error on HTTP Request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi Disconnected!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("MGNK V1 - ESP32 #1 AI System Initializing...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
}

void loop() {
  // Test Question to Gemini AI
  Serial.println("Asking Gemini AI...");
  sendToGemini("Hello Gemini, introduce MGNK Robot V1 in 1 line.");

  delay(10000); // 10 Seconds Delay between requests
}
