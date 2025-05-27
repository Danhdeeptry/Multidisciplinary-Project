// File: main.cpp
#define LED_PIN 48
#define SDA_PIN GPIO_NUM_11
#define SCL_PIN GPIO_NUM_12

#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include "DHT20.h"
#include "Wire.h"
#include <ArduinoOTA.h>
#include <random>  // Thêm thư viện random để tạo dữ liệu mô phỏng

constexpr char WIFI_SSID[] = "P4.1111";               // Thay bằng tên WiFi của bạn
constexpr char WIFI_PASSWORD[] = "123456788123456788";      // Thay bằng mật khẩu WiFi của bạn

constexpr char TOKEN[] = "lr3yyoh7v2b98pdmuj38";   // Device access token từ ThingsBoard

constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";   //server(broker) ThingsBoard
constexpr uint16_t THINGSBOARD_PORT = 1883U;

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";
constexpr char LED_MODE_ATTR[] = "ledMode";
constexpr char LED_STATE_ATTR[] = "ledState";

volatile bool attributesChanged = false;
volatile int ledMode = 0;
volatile bool ledState = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

uint32_t previousStateChange = 0;

// constexpr int16_t telemetrySendInterval = 10000U;
constexpr int16_t telemetrySendInterval = 2000U;  // Giảm xuống 2 giây cho mô phỏng

uint32_t previousDataSend = 0;

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
  LED_STATE_ATTR,
  BLINKING_INTERVAL_ATTR
};

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

DHT20 dht20;

// Tạo biến để lưu trữ dữ liệu mô phỏng
float simulated_temp = 28.0;       // Nhiệt độ ban đầu (28°C)
float simulated_humid = 65.0;      // Độ ẩm ban đầu (65%)
unsigned long lastSimChange = 0;   // Thời gian cuối cùng thay đổi giá trị mô phỏng
const int SIM_UPDATE_INTERVAL = 5000; // Cập nhật giá trị mô phỏng mỗi 5 giây

// Hàm mô phỏng dữ liệu cảm biến
void updateSimulatedSensorValues() {
  if (millis() - lastSimChange >= SIM_UPDATE_INTERVAL) {
    lastSimChange = millis();
    
    // Thêm một số dao động ngẫu nhiên
    simulated_temp += random(-100, 100) / 100.0;  // Dao động ±1°C
    simulated_humid += random(-50, 50) / 100.0;   // Dao động ±0.5%
    
    // Giới hạn giá trị trong khoảng hợp lý
    simulated_temp = constrain(simulated_temp, 15.0, 40.0);  // 15-40°C
    simulated_humid = constrain(simulated_humid, 30.0, 90.0); // 30-90%
    
    Serial.println("Updated simulated values:");
    Serial.printf("  Temperature: %.2f C\n", simulated_temp);
    Serial.printf("  Humidity: %.2f %%\n", simulated_humid);
  }
}

// Thêm hàm DHT20JSON ở đây
String DHT20JSON_temp(float temperature) {
  DynamicJsonDocument doc(200);
  doc["id"] = "1";
  doc["name"] =  "temperature";
  doc["data"] = String(temperature);
  doc["unit"] = "C";

  String output;
  serializeJson(doc, output);  // Serialize the JSON document to a string.
  return output;
}

String DHT20JSON_humid(float humidity) {
  DynamicJsonDocument doc(200);
  doc["id"] = "2";
  doc["name"] =  "humidity";
  doc["data"] = String(humidity);
  doc["unit"] = "%";

  String output;
  serializeJson(doc, output);  // Serialize the JSON document to a string.
  return output;
}

// ---------------- RPC Callback ----------------
RPC_Response setLedSwitchState(const RPC_Data &data) {
  Serial.println("Received Switch state");
  bool newState = data;  // parse boolean
  Serial.print("Switch state change: ");
  Serial.println(newState);

  digitalWrite(LED_PIN, newState);
  attributesChanged = true;
  return RPC_Response("setLedSwitchValue", newState);
}

// RPC để thiết lập nhiệt độ mô phỏng trực tiếp
RPC_Response setSimulatedTemp(const RPC_Data &data) {
  float newTemp = data;
  Serial.print("Setting simulated temperature to: ");
  Serial.println(newTemp);
  
  simulated_temp = newTemp;
  return RPC_Response("setSimulatedTemp", newTemp);
}

// RPC để thiết lập độ ẩm mô phỏng trực tiếp
RPC_Response setSimulatedHumid(const RPC_Data &data) {
  float newHumid = data;
  Serial.print("Setting simulated humidity to: ");
  Serial.println(newHumid);
  
  simulated_humid = newHumid;
  return RPC_Response("setSimulatedHumid", newHumid);
}

const std::array<RPC_Callback, 3U> callbacks = {
  RPC_Callback{"setLedSwitchValue", setLedSwitchState},
  RPC_Callback{"setSimulatedTemp", setSimulatedTemp},
  RPC_Callback{"setSimulatedHumid", setSimulatedHumid}
};

// ---------------- Shared Attributes -----------
void processSharedAttributes(const Shared_Attribute_Data &data) {
  for (auto it = data.begin(); it != data.end(); ++it) {
    if (strcmp(it->key().c_str(), BLINKING_INTERVAL_ATTR) == 0) {
      uint16_t new_interval = it->value().as<uint16_t>();
      if (new_interval >= BLINKING_INTERVAL_MS_MIN && new_interval <= BLINKING_INTERVAL_MS_MAX) {
        blinkingInterval = new_interval;
        Serial.print("Blinking interval is set to: ");
        Serial.println(new_interval);
      }
    } else if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0) {
      ledState = it->value().as<bool>();
      digitalWrite(LED_PIN, ledState);
      Serial.print("LED state is set to: ");
      Serial.println(ledState);
    }
  }
  attributesChanged = true;
}

const Shared_Attribute_Callback attributes_callback(
  &processSharedAttributes,
  SHARED_ATTRIBUTES_LIST.cbegin(),
  SHARED_ATTRIBUTES_LIST.cend()
);

const Attribute_Request_Callback attribute_shared_request_callback(
  &processSharedAttributes,
  SHARED_ATTRIBUTES_LIST.cbegin(),
  SHARED_ATTRIBUTES_LIST.cend()
);

// ---------------- WiFi helpers ----------------
void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to AP");
}

bool reconnectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  InitWiFi();
  return (WiFi.status() == WL_CONNECTED);
}

// --------------- Task prototypes --------------
void taskReconnectWiFi(void *pvParameters);
void taskConnectCoreIoT(void *pvParameters);
void taskSendAttributes(void *pvParameters);
void taskSendTelemetry(void *pvParameters);
void taskTbLoop(void *pvParameters);

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  pinMode(LED_PIN, OUTPUT);
  delay(1000);

  // Khởi tạo WiFi, khởi tạo random
  InitWiFi();
  randomSeed(analogRead(0));  // Khởi tạo random generator
  Serial.println("Simulation mode activated - No physical sensors required");

  // Tạo các task
  
  xTaskCreatePinnedToCore(taskReconnectWiFi,  "WiFiCheck",     2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskConnectCoreIoT, "ConnectCoreIoT",4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendAttributes, "SendAttributes",2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendTelemetry,  "SendTelemetry", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskTbLoop,         "TbLoop",        2048, NULL, 1, NULL, 1);
}

void loop() {
  
}

// 1) Task kiểm tra kết nối wifi
void taskReconnectWiFi(void *pvParameters) {
  for (;;) {
    reconnectWiFi();
    // Kiểm tra 2s/lần
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// 2) Task kiểm tra kết nối CoreIoT, subscribe 1 lần
void taskConnectCoreIoT(void *pvParameters) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED && !tb.connected()) {
      Serial.print("Connecting to: ");
      Serial.print(THINGSBOARD_SERVER);
      Serial.print(" with token ");
      Serial.println(TOKEN);

      if (tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
        Serial.println("Connected to CoreIOT");
        tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

        // Subscribe 1 lần sau khi connect
        
        Serial.println("Subscribing for RPC...");
        if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
          Serial.println("Failed to subscribe for RPC");
        }

        if (!tb.Shared_Attributes_Subscribe(attributes_callback)) {
          Serial.println("Failed to subscribe for shared attribute updates");
        }

        Serial.println("Subscribe done");
        if (!tb.Shared_Attributes_Request(attribute_shared_request_callback)) {
          Serial.println("Failed to request for shared attributes");
        }
      } else {
        Serial.println("Failed to connect => retry in 5s");
        // Thử lại sau 5 giây
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        continue;
      }
    }
    // Khi đã kết nối hoặc chưa wifi => check 2s/lần
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// 3) Task gửi dữ liệu attributes (chỉ khi có thay đổi)
void taskSendAttributes(void *pvParameters) {
  for (;;) {
    if (tb.connected() && attributesChanged) {
      attributesChanged = false;
      // Gửi state LED
      tb.sendAttributeData(LED_STATE_ATTR, digitalRead(LED_PIN));
    }
    // Kiểm tra ~500ms/lần
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// 4) Task gửi dữ liệu telemetry (mỗi ~2s)
void taskSendTelemetry(void *pvParameters) {
  for (;;) {
    if (tb.connected()) {
      if (millis() - previousDataSend > telemetrySendInterval) {
        previousDataSend = millis();
        
        // Cập nhật giá trị mô phỏng
        updateSimulatedSensorValues();
        
        // Sử dụng giá trị mô phỏng thay vì đọc từ cảm biến
        float temperature = simulated_temp;
        float humidity = simulated_humid;
        
        // Tạo JSON theo format
        String jsonPayload_humid = DHT20JSON_humid(humidity);
        String jsonPayload_temp = DHT20JSON_temp(temperature);

        // Gửi dữ liệu telemetry lên ThingsBoard
        tb.sendTelemetryData("temperature", temperature);
        tb.sendTelemetryData("humidity", humidity);
        Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n", temperature, humidity);
        
        // Gửi thêm một số attribute liên quan WiFi
        tb.sendAttributeData("rssi", WiFi.RSSI());
        tb.sendAttributeData("channel", WiFi.channel());
        tb.sendAttributeData("bssid", WiFi.BSSIDstr().c_str());
        tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
        tb.sendAttributeData("ssid", WiFi.SSID().c_str());
      }
    }
    // Kiểm tra 500ms/lần
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// 5) Task tb.loop()
void taskTbLoop(void *pvParameters) {
  for (;;) {
    if (tb.connected()) {
      tb.loop();
    }
    // 10 ~ 100ms
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}