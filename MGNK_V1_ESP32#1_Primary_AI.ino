/*
  MGNK Robot V1 - ESP32 #1 (Main AI Brain)
  Step 2: Wi-Fi + INMP441 I2S Microphone Setup
  Developer: Karthikeyan Chairman
  Language: C++
*/

#include <WiFi.h>
#include <driver/i2s.h> // Header for Audio Input

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Serial Communication Pins for ESP32 #2
#define RXD2 16
#define TXD2 17

// INMP441 I2S Microphone Pins
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
#define I2S_PORT I2S_NUM_0

void setupMicrophone() {
  // Configures I2S for Audio Recording
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  Serial.println("INMP441 Microphone Configured!");
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("MGNK V1 - ESP32 #1 System Initializing...");

  // Setup Mic
  setupMicrophone();

  // Connect Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
}

void loop() {
  // Mic is listening for speech...
  Serial.println("ESP32 #1: Listening for Voice Input...");
  delay(3000);
}
