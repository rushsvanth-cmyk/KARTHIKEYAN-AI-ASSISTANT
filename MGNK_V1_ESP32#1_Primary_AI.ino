/*
  Project: MGNK Robot V1 - Primary AI Brain (Version 1.6 Final Master Release)
  File Title: MGNK_V1_ESP32_Primary_AI_Brain.ino
  Developer: Karthikeyan Chairman
  Architecture: 
    - ESP32 #1: Gemini AI + Dual-Mode Wi-Fi Architecture (Master)
    - ESP32 #2: Audio & TTS Engine (Slave)
    - Arduino: Hardware Sensors
*/

#include <WiFi.h> 
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Preferences.h>

Preferences prefs; // Permanent Memory (NVS) to store Wi-Fi details

// Google Gemini API Key
const char* gemini_api_key = "YOUR_GEMINI_API_KEY_HERE";

// Hardware Serial Pins
#define TXD2 17
#define RXD2 16
#define RXD1 18
#define TXD1 19

WiFiManager wm;

void sendToGemini(String userQuery) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(gemini_api_key);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000); 

    JsonDocument doc;
    JsonArray contents = doc["contents"].to<JsonArray>();
    JsonObject contentObj = contents.add<JsonObject>();
    JsonArray parts = contentObj["parts"].to<JsonArray>();
    JsonObject partObj = parts.add<JsonObject>();

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
    Serial.println("\n[Wi-Fi Error]: Not connected to Wi-Fi.");
  }
}

// Function to scan surrounding Wi-Fi networks via Voice/Command
void scanWiFiNetworks() {
  Serial.println("[Wi-Fi System]: Scanning nearby networks...");
  Serial2.println("TTS_TEXT:Scanning nearby Wi-Fi networks, please wait.");
  
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial2.println("TTS_TEXT:No Wi-Fi networks found.");
  } else {
    String foundString = "TTS_TEXT:Found " + String(n) + " networks. First available is " + WiFi.SSID(0);
    Serial2.println(foundString);
    Serial.println("\n--- Available Wi-Fi Networks ---");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s (%d dBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
  }
}

// Function to manual/voice connect using SSID and Password
void connectWiFiManual(String ssid, String pass) {
  Serial.println("[Wi-Fi System]: Attempting connection to " + ssid);
  Serial2.println("TTS_TEXT:Connecting to " + ssid);
  
  WiFi.begin(ssid.c_str(), pass.c_str());
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 20) {
    delay(500);
    Serial.print(".");
    count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[Wi-Fi System]: Connected Successfully!");
    Serial2.println("TTS_TEXT:Wi-Fi connected successfully!");
    
    // Save to permanent NVS Memory
    prefs.begin("wifi_store", false);
    prefs.putString("ssid", ssid);
    prefs.putString("pass", pass);
    prefs.end();
  } else {
    Serial.println("\n[Wi-Fi System]: Connection Failed!");
    Serial2.println("TTS_TEXT:Connection failed. Please check password.");
  }
}

// Function to trigger Web Portal Mode
void startWebPortal() {
  Serial2.println("TTS_TEXT:Starting Web Portal Setup. Connect to MGNK V1 Setup Hotspot.");
  wm.setConfigPortalTimeout(180); // 3 minutes timeout
  if (!wm.startConfigPortal("MGNK_V1_Setup")) {
    Serial.println("Portal Timeout, returning to main loop.");
  } else {
    Serial.println("Connected via Web Portal!");
    Serial2.println("TTS_TEXT:Wi-Fi Connected via Web Portal!");
  }
}

void processQuery(String query) {
  query.toLowerCase();

  // Local Wi-Fi Commands
  if (query == "scan wifi" || query == "scan_wifi") {
    scanWiFiNetworks();
  } 
  else if (query.startsWith("connect:")) { // Format: CONNECT:SSID:PASSWORD
    int firstColon = query.indexOf(':');
    int secondColon = query.indexOf(':', firstColon + 1);
    if (secondColon > 0) {
      String newSSID = query.substring(firstColon + 1, secondColon);
      String newPass = query.substring(secondColon + 1);
      connectWiFiManual(newSSID, newPass);
    }
  }
  else if (query == "enable portal" || query == "web portal") {
    startWebPortal();
  }
  // Personal Info Triggers
  else if (query.indexOf("who created you") >= 0 || query.indexOf("who is your creator") >= 0) {
    Serial2.println("LOCAL_SD_PLAY:creator_info.mp3");
  } 
  else if (query.indexOf("father name") >= 0 || query.indexOf("creator father") >= 0) {
    Serial2.println("LOCAL_SD_PLAY:father_info.mp3");
  } 
  else {
    sendToGemini(query);
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);

  Serial.println("\n==================================================");
  Serial.println("  MGNK V1 - Primary AI Brain (v1.6 Dual Wi-Fi)");
  Serial.println("  Developer: Karthikeyan Chairman");
  Serial.println("==================================================");

  // Try auto-connect from saved NVS memory
  prefs.begin("wifi_store", true);
  String savedSSID = prefs.getString("ssid", "");
  String savedPass = prefs.getString("pass", "");
  prefs.end();

  bool connected = false;

  if (savedSSID != "") {
    Serial.println("[Memory]: Found saved Wi-Fi: " + savedSSID);
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    
    // 10 Seconds Timeout Check
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
      delay(500);
      Serial.print(".");
      retry++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
    }
  }

  // If NVS memory fails or empty, fallback to WiFiManager
  if (!connected) {
    Serial.println("\n[Fallback]: Starting WiFiManager Provisioning...");
    wm.autoConnect("MGNK_V1_Setup");
  }

  // Boot Greeting
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nStatus: Wi-Fi Connected Successfully!");
    sendToGemini("System startup complete. Greet Karthikeyan Chairman politely.");
  }
}

void loop() {
  if (Serial1.available() > 0) {
    String sensorData = Serial1.readStringUntil('\n');
    sensorData.trim();
    if (sensorData.indexOf("Human Motion Detected") >= 0) {
      processQuery("A person just walked in front of you. Welcome them warmly.");
    }
  }

  if (Serial.available() > 0) {
    String manualQuery = Serial.readStringUntil('\n');
    manualQuery.trim();
    if (manualQuery.length() > 0) {
      processQuery(manualQuery);
    }
  }
  delay(100); 
}
