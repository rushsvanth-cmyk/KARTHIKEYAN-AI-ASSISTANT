/*
  Project: MGNK Robot V1 - Audio & TTS Engine
  File Title: MGNK_V1_ESP32_Audio_TTS_Engine.ino
  Developer: Karthikeyan Chairman
  Architecture: 
    - ESP32 #1: Gemini AI + Wi-Fi Brain (Master)
    - ESP32 #2: Audio & TTS Engine (Slave)
    - Arduino: Sensors & Hardware Control
*/

#include <Arduino.h>

// Hardware Serial Pins for Communication with ESP32 #1
#define RXD2 16
#define TXD2 17

// Audio Status Flags
bool isSpeaking = false;

void playTTS(String textToSpeak) {
  isSpeaking = true;
  Serial.print("\n[Audio Engine Processing]: Converting Text to Speech -> ");
  Serial.println(textToSpeak);

  // Simulation of Text-to-Speech Output (I2S / DAC Output Stream)
  // Connect I2S Amplifier (MAX98357A) logic here
  delay(1500); 

  Serial.println("[Audio Engine Output]: Speech Playback Completed.");
  isSpeaking = false;
}

void setup() {
  Serial.begin(115200);

  // Initialize Hardware Serial 2 for communication with Primary Brain (ESP32 #1)
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("\n==================================================");
  Serial.println("   MGNK V1 - Audio & TTS Subsystem Initialized    ");
  Serial.println("==================================================");
  Serial.println("Status: Listening for incoming AI text from ESP32 #1...");
  Serial.println("==================================================");
}

void loop() {
  // Read incoming Serial data from Primary AI Brain
  if (Serial2.available() > 0) {
    String incomingData = Serial2.readStringUntil('\n');
    incomingData.trim();

    // Check if the received message has the TTS prefix
    if (incomingData.startsWith("TTS_TEXT:")) {
      String speechText = incomingData.substring(9); // Extract actual text
      
      Serial.println("\n[UART Event]: Received AI Response from Master Brain.");
      playTTS(speechText);
    }
  }

  delay(50); // Polling delay
}
