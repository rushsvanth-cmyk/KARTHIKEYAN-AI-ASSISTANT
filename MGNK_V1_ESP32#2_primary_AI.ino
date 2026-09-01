/*
 * Project: MGNK Robot V1 - Phase 2 (Audio Engine & Voice Stream Handler)
 * Date: August 31, 2026
 * Task: Audio.h, MAX98357A, Inter-board UART, Google TTS Stream Handler & Dynamic Volume Control
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
// AUG 27, 28, 29, 30 & 31 TASK FUNCTIONS: UART, TTS Stream & Dynamic Controller
// ============================================================================

void initUARTReceiver() {
  // Initialize Serial2 at 9600 Baud Rate for communication with ESP32 #1
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("[Aug 27 Log] UART Serial Bridge Initialized on Pins 16/17!");
}

// Aug 28, 29, 30 & 31: Dynamic Audio Stream & Command Handler
void processAudioPlayback(String command) {
  // Instant Speech Interrupt Signal
  if (command == "STOP" || command == "HALT") {
    audio.stopSong(); // Instantly stops audio playback
    Serial.println("[Aug 27 Action] Speech Interrupted and Stopped Successfully!");
    Serial2.println("ACK:STOP_OK"); // Acknowledge interrupt back to ESP32 #1
  } 
  // Aug 30 Task: Stream Google TTS Audio Output sent by Primary AI Brain
  else if (command.startsWith("TTS_PLAY:")) {
    String filePath = command.substring(9);
    Serial.print("[Aug 30 Action] Converting & Playing Google TTS Voice Output: ");
    Serial.println(filePath);
    
    // Aug 31 Addition: File Existence Safety Verification
    if (SD.exists(filePath.c_str())) {
      audio.connecttoFS(SD, filePath.c_str());
      Serial2.println("ACK:TTS_PLAYING");
    } else {
      Serial.println("[Aug 31 Error] Requested Audio File Not Found on SD Card!");
      Serial2.println("ERR:FILE_NOT_FOUND");
    }
  }
  // Aug 29 Task: Dynamic Volume Adjustment via UART
  else if (command.startsWith("SET_VOL:")) {
    int volLevel = command.substring(8).toInt();
    volLevel = constrain(volLevel, 0, 21); // Keep volume within safe Audio.h limits
    audio.setVolume(volLevel);
    Serial.print("[Aug 29 Action] Speaker Volume Adjusted To: ");
    Serial.println(volLevel);
    Serial2.println("ACK:VOL_SET");
  }
}

void handleIncomingCommands() {
  // Check for incoming commands from Primary AI Brain (ESP32 #1)
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();

    Serial.print("[Log] Received Command from ESP32 #1: ");
    Serial.println(command);

    // Process incoming audio playback, volume adjustment and interrupt commands
    processAudioPlayback(command);
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
  audio.setVolume(18); // Tuned default high-clarity volume level
  Serial.println("[Aug 26 Task Complete] Audio.h & MAX98357A Amp Ready!");

  // UART & System Calls
  initUARTReceiver();
  Serial.println("[Aug 27 Task Complete] Inter-board UART System Online!");
  Serial.println("[Aug 28 Task Complete] Audio Stream Receiver System Ready!");
  Serial.println("[Aug 29 Task Complete] Phase 2 Audio Engine Firmware Ready!");
  Serial.println("[Aug 30 Task Complete] Google TTS Voice Output Handler Online!");
  Serial.println("[Aug 31 Task Complete] Telemetry ACK Protocol & Error Checking Active!");
}

void loop() {
  audio.loop();               // Keeps audio engine running continuously
  handleIncomingCommands();   // Listens for instant interrupt, TTS & Volume signals
}

// ============================================================================
