/*
 * Project: MGNK Robot V1 - Phase 2 (Audio Engine)
 * Date: August 27, 2026
 * Task: Audio.h Integration, MAX98357A Setup & UART Inter-board Command Receiver
 * Developer: Karthikeyan Chairman
 */

#include <Arduino.h> 
#include <SPI.h>
#include <SD.h>
#include <Audio.h>  // Audio.h Library

// MAX98357A I2S Wiring Pins
#define I2S_DOUT      22  // DIN Pin
#define I2S_BCLK      26  // BCLK Pin
#define I2S_LRC       25  // LRC Pin
#define SD_CS         5   // SD Card CS Pin

// Hardware Serial 2 Pins (Bridge to ESP32 #1 Primary AI Brain)
#define RX_PIN        16  // Receives commands from ESP32 #1
#define TX_PIN        17  // Sends status back to ESP32 #1

Audio audio;

// ============================================================================
// AUG 27 TASK FUNCTIONS: UART Communication & Interrupt System
// ============================================================================

void initUARTReceiver() {
  // Initialize Serial2 at 9600 Baud Rate for communication with ESP32 #1
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("[Aug 27 Log] UART Serial Bridge Initialized on Pins 16/17!");
}

void handleIncomingCommands() {
  // Check for incoming commands from Primary AI Brain
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();

    Serial.print("[Aug 27 Log] Received Command: ");
    Serial.println(command);

    // Immediate Speech Interrupt Logic
    if (command == "STOP" || command == "HALT") {
      audio.stopSong(); // Instantly stops audio playback
      Serial.println("[ACTION] Speech Interrupted and Stopped Successfully!");
    }
  }
}

// ============================================================================
// MAIN SYSTEM SETUP & LOOP
// ============================================================================

void setup() {
  Serial.begin(115200);
  
  // Initialize SD Card
  if (!SD.begin(SD_CS)) {
    Serial.println("[Aug 26 Log] SD Card Mount Failed!");
  } else {
    Serial.println("[Aug 26 Log] SD Card Mount Success!");
  }

  // MAX98357A I2S Audio Pinout Setup
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15); 
  Serial.println("[Aug 26 Task Complete] Audio.h & MAX98357A Amp Ready!");

  // Aug 27 Setup Call
  initUARTReceiver();
  Serial.println("[Aug 27 Task Complete] Inter-board UART System Online!");
}

void loop() {
  audio.loop();               // Keeps audio engine running continuously
  handleIncomingCommands();   // Listens for instant interrupt & trigger signals
}

// ============================================================================
// AUG 28 TASK CODE WILL BE APPENDED BELOW THIS LINE TOMORROW
// ============================================================================
