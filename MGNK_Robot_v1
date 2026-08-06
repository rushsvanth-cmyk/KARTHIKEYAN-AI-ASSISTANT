/*
  MGNK Robot V1 - ESP32 #2 (Audio, SD Card & RTC Core)
  Developer: Karthikeyan Chairman
  Language: C++ (Arduino IDE)
*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

// Pin Definitions for ESP32 #2
#define SD_CS_PIN 5
#define RXD2 16
#define TXD2 17

RTC_DS3231 rtc;
bool isSdPresent = false;

void setup() {
  // Serial Monitors
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // Communication with ESP32 #1

  // Initialize I2C for RTC
  Wire.begin(21, 22); // SDA = G21, SCL = G22

  // Check RTC Module
  if (!rtc.begin()) {
    Serial.println("RTC Module Not Found!");
  } else {
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, setting the time!");
      // Sets time to compilation time
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }

  // Check SD Card Module
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card Mount Failed!");
    isSdPresent = false;
  } else {
    Serial.println("SD Card Initialized Successfully!");
    isSdPresent = true;
  }

  Serial.println("ESP32 #2 System Ready!");
}

void loop() {
  // Check if data received from ESP32 #1
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    command.trim();
    
    Serial.print("Command from ESP32 #1: ");
    Serial.println(command);

    if (command == "GET_TIME") {
      DateTime now = rtc.now();
      String timeStr = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
      Serial2.println("TIME_IS:" + timeStr);
      Serial.print("Sent Time: ");
      Serial.println(timeStr);
    }
    else if (command == "PLAY_AUDIO" && isSdPresent) {
      Serial.println("Playing audio file from SD Card...");
      // Audio playback logic goes here
    }
  }
  
  delay(100);
}
