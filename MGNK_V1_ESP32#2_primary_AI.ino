/*
 * Project: MGNK Robot V1 - Phase 2 (Audio Engine)
 * Date: August 26, 2026
 * Task: Audio.h library integration and MAX98357A I2S Amp setup
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

Audio audio;

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
}

void loop() {
  audio.loop(); // Keeps audio engine running
}

// ============================================================================
// AUG 27 TASK CODE WILL BE APPENDED BELOW THIS LINE TOMORROW
// ============================================================================
