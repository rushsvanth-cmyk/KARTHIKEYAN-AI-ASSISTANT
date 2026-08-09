/*
  MGNK Robot V1 - ESP32 #1 (Main AI Brain)
  Step 3: Wi-Fi + INMP441 Mic + UART Serial Command System
  Developer: Karthikeyan Chairman
  Language: C++
*/

#include <WiFi.h>
#include <driver/i2s.h> // Audio Input Header

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Hardware Serial 2 Pins for Communication with ESP32 #2
#define RXD2 16
#define TXD2 17

// INMP441 I2S Microphone Pins
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
#define I2S_PORT I2S_NUM_0

void setupMicrophone() {
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
  // Serial Monitor for Laptop Debugging
  Serial.begin(115200);
  
  // Serial2 for Communication with ESP32 #2 (Baud Rate: 115200)
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.println("MGNK V1 - ESP32 #1 System Initializing...");

  // Setup Digital Microphone
  setupMicrophone();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected Successfully!");
}

void loop() {
  Serial.println("ESP32 #1: Active and Listening...");
  
  // Sending Command to ESP32 #2 via UART Serial2
  Serial2.println("CMD_PLAY_WELCOME_AUDIO");
  
  // Checking response from ESP32 #2
  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    Serial.print("Response from ESP32 #2: ");
    Serial.println(response);
  }

  delay(4000); // 4 Seconds Delay
}
