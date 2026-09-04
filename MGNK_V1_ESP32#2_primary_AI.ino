/*
 * Project: MGNK Robot V1 - Phase 2 & 3 Bridge (Audio Engine & Core Logic)
 * Date: September 03, 2026
 * Task: Pure Logic Code for Audio.h, Google TTS, System Health, Mute & Reset Recovery
 * Developer: Karthikeyan Chairman
 */

#include <Arduino.h> 
#include <SPI.h>
#include <SD.h> 
#include <Audio.h>

Audio audio;
bool isMuted = false;
int previousVolume = 18;

// ============================================================================
// CORE LOGIC FUNCTIONS: UART RECEIVER, TTS, TELEMETRY & RECOVERY
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
    previousVolume = volLevel;
    audio.setVolume(volLevel);
    Serial2.println("ACK:VOL_SET");
  }
  // Sep 02 Addition: System Health Inquiry
  else if (command == "GET_STATUS") {
    Serial2.println("STATUS:AUDIO_ENGINE_ONLINE");
  }
  // Sep 03 Addition: Mute & Unmute Commands
  else if (command == "MUTE") {
    isMuted = true;
    audio.setVolume(0);
    Serial2.println("ACK:MUTED");
  }
  else if (command == "UNMUTE") {
    isMuted = false;
    audio.setVolume(previousVolume);
    Serial2.println("ACK:UNMUTED");
  }
  // Sep 03 Addition: Soft Reset Audio Engine
  else if (command == "RESET_AUDIO") {
    audio.stopSong();
    audio.setVolume(18);
    Serial2.println("ACK:AUDIO_RESET_OK");
  }
}

void handleIncomingCommands() {
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();
    processAudioPlayback(command);
  }
}

// Audio Status Callback
void audio_eof_speech(const char *info) {
  Serial2.println("STATUS:SPEECH_FINISHED");
}

// Audio Engine Error Callback
void audio_error(const char *info) {
  Serial2.println("ERR:AUDIO_PLAYBACK_ERROR");
}

// ============================================================================
// MAIN SYSTEM SETUP & LOOP
// ============================================================================

void setup() {
  Serial.begin(115200);

  // Core System Initializations
  SD.begin(5); // SD CS Pin
  audio.setVolume(18);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // UART Bridge Pins
}

void loop() {
  audio.loop();               // Keeps audio engine running continuously
  handleIncomingCommands();   // Handles Interrupt, TTS, Volume, Mute & Reset signals
}

// ============================================================================