/*
 * Project: MGNK Robot V1 - Phase 3 (Audio Engine & Sensor/Motion Logic)
 * Date: September 04, 2026
 * Task: Audio.h, Google TTS, System Health, Recovery & MPU6050 Tilt Safety Telemetry
 * Developer: Karthikeyan Chairman
 */

#include <Arduino.h> 
#include <SPI.h>
#include <SD.h> 
#include <Audio.h>
#include <Wire.h>

// MPU6050 I2C Address
#define MPU6050_ADDR 0x68

Audio audio;
bool isMuted = false;
int previousVolume = 18;
int16_t accelX, accelY, accelZ;

// ============================================================================
// PHASE 3 SENSOR FUNCTIONS: MPU6050 IMU TILT & SAFETY TELEMETRY
// ============================================================================

void initIMUSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Wake up MPU6050
  Wire.endTransmission(true);
}

void checkTiltAndSafetyStatus() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B); // Accelerometer Data Register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);

  accelX = Wire.read() << 8 | Wire.read();
  accelY = Wire.read() << 8 | Wire.read();
  accelZ = Wire.read() << 8 | Wire.read();

  // Tilt/Fall Alert Detection Logic
  if (abs(accelX) > 15000 || abs(accelY) > 15000) {
    Serial2.println("STATUS:TILT_WARNING_TRIGGERED");
  }
}

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
  // System Health Inquiry
  else if (command == "GET_STATUS") {
    Serial2.println("STATUS:AUDIO_ENGINE_ONLINE");
  }
  // Mute & Unmute Commands
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
  // Soft Reset Audio Engine
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
  
  // Phase 3 Sensor Initialization
  initIMUSensor();
}

void loop() {
  audio.loop();               // Keeps audio engine running continuously
  handleIncomingCommands();   // Handles Interrupt, TTS, Volume, Mute & Reset signals
  checkTiltAndSafetyStatus(); // Monitors MPU6050 IMU tilt status
}

// ============================================================================