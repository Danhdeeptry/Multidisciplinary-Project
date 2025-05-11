// File: main.cpp
#define LED_PIN 48
#define SDA_PIN GPIO_NUM_11
#define SCL_PIN GPIO_NUM_12

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "DHT20.h"
#include <ThingsBoard.h>

// WiFi and ThingsBoard credentials
constexpr char WIFI_SSID[] = "P4.11";
constexpr char WIFI_PASSWORD[] = "123456788";

constexpr char TOKEN[] = "lr3yyoh7v2b98pdmuj38"; 

constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;

// General configuration
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";
constexpr char LED_MODE_ATTR[] = "ledMode";
constexpr char LED_STATE_ATTR[] = "ledState";

volatile bool attributesChanged = false;
volatile bool ledState = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

constexpr int16_t telemetrySendInterval = 2000U;
uint32_t previousDataSend = 0;

// WiFi client và ThingsBoard
WiFiClient wifiClient;
ThingsBoard tb(wifiClient);

// Sensor DHT20
DHT20 dht20;

// RPC Callback
RPC_Response processLedControl(const RPC_Data &data) {
  Serial.println("Received led control command");
  bool led_state = data;
  Serial.print("LED state: ");
  Serial.println(led_state);
  
  digitalWrite(LED_PIN, led_state);
  return RPC_Response("ledControl", led_state);
}

// Callback array
RPC_Callback callbacks[] = {
  { "ledControl", processLedControl }
};

// Khởi tạo WiFi
void InitWiFi() {
  Serial.println("Connecting to AP...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to AP");
}

// Kiểm tra kết nối WiFi
bool reconnect() {
  // Kiểm tra kết nối WiFi
  if (WiFi.status() != WL_CONNECTED) {
    InitWiFi();
    return false;
  }
  
  // Kiểm tra kết nối ThingsBoard
  if (!tb.connected()) {
    Serial.printf("Connecting to ThingsBoard node at %s...\n", THINGSBOARD_SERVER);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
      Serial.println("Failed to connect to ThingsBoard");
      return false;
    }
    Serial.println("Connected to ThingsBoard");
    
    // Đăng ký RPC callbacks
    if (!tb.RPC_Subscribe(callbacks[0])) {
      Serial.println("Failed to subscribe for RPC");
      return false;
    }
    
    Serial.println("Subscribed for RPC");
  }
  
  return true;
}

void setup() {
  // Khởi tạo Serial, GPIO
  Serial.begin(SERIAL_DEBUG_BAUD);
  pinMode(LED_PIN, OUTPUT);
  
  // Delay để ổn định
  delay(1000);
  
  // Khởi tạo WiFi
  InitWiFi();
  
  // Khởi tạo cảm biến DHT20
  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();
}

void loop() {
  if (!reconnect()) {
    delay(2000);
    return;
  }
  
  // Xử lý TB Connection
  tb.loop();
  
  // Gửi dữ liệu theo định kỳ
  if (millis() - previousDataSend > telemetrySendInterval) {
    previousDataSend = millis();
    
    // Đọc nhiệt độ và độ ẩm
    dht20.read();
    float temperature = dht20.getTemperature();
    float humidity = dht20.getHumidity();
    
    if (!isnan(temperature) && !isnan(humidity)) {
      // Gửi dữ liệu telemetry
      tb.sendTelemetryData("temperature", temperature);
      tb.sendTelemetryData("humidity", humidity);
      
      // In thông tin ra serial
      Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", temperature, humidity);
      
      // Gửi một số thông tin WiFi
      tb.sendAttributeData("ipAddress", WiFi.localIP().toString().c_str());
      tb.sendAttributeData("rssi", WiFi.RSSI());
    } else {
      Serial.println("Failed to read from DHT sensor!");
    }
  }
}