/*
 * Project: MGNK Robot V1 - Phase 2 (Audio Engine & Core Logic)
 * Date: September 02, 2026
 * Task: Pure Logic Code for Audio.h, Google TTS Handler, Status Telemetry & Phase 2 Completion
 * Developer: Karthikeyan Chairman
 */

#include <Arduino.h> 
#include <SPI.h>
#include <SD.h>
#include <Audio.h>

Audio audio;

// ============================================================================
// CORE LOGIC FUNCTIONS: UART RECEIVER, TTS, TELEMETRY & HEALTH CHECK
// ============================================================================

void processAudioPlayback(String command) {
  // Speech Interrupt Signal
  if (command == "STOP" || command == "HALT") {
    audio.stopSong();
    Serial2.println("ACK:STOP_OK");
  } 
  // Stream Google TTS Audio Output
  else if (command.startsWith("TTS_PLAY:")) {
    String filePath = command.substring(9);
    
    if (SD.exists(filePath.c_str())) {
      audio.connecttoFS(SD, filePath.c_str());
      Serial2.println("ACK:TTS_PLAYING");
    } else {
      Serial2.println("ERR:FILE_NOT_FOUND");
    }
  }
  // Dynamic Volume Adjustment
  else if (command.startsWith("SET_VOL:")) {
    int volLevel = command.substring(8).toInt();
    volLevel = constrain(volLevel, 0, 21);
    audio.setVolume(volLevel);
    Serial2.println("ACK:VOL_SET");
  }
  // Sep 02 Addition: System Health & Readiness Status Inquiry
  else if (command == "GET_STATUS") {
    Serial2.println("STATUS:AUDIO_ENGINE_ONLINE");
  }
}

void handleIncomingCommands() {
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();
    processAudioPlayback(command);
  }
}

// Audio Status Callback (Triggers automatically when speech finishes playing)
void audio_eof_speech(const char *info) {
  Serial2.println("STATUS:SPEECH_FINISHED");
}

// Audio Engine Error Callback (Triggers on playback or stream failures)
void audio_error(const char *info) {
  Serial2.println("ERR:AUDIO_PLAYBACK_ERROR");
}

// ============================================================================
// MAIN SYSTEM SETUP & LOOP
// ============================================================================

void setup() {
  Serial.begin(115200);

  // Core System Initializations
  SD.begin(5); // SD CS Pin Only
  audio.setVolume(18);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Communication Bridge Only
}

void loop() {
  audio.loop();               // Keeps audio engine running continuously
  handleIncomingCommands();   // Listens for instant interrupt, TTS, Volume & Status signals
}

// ============================================================================
