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

constexpr char WIFI_SSID[] = "Redmi Note 9S";
constexpr char WIFI_PASSWORD[] = "88888888";

constexpr char TOKEN[] = "lr3yyoh7v2b98pdmuj38"; 

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
constexpr int16_t telemetrySendInterval = 100U;

uint32_t previousDataSend = 0;

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
  LED_STATE_ATTR,
  BLINKING_INTERVAL_ATTR
};

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

DHT20 dht20;

// // Thêm hàm DHT20JSON ở đây
// String DHT20JSON(float temperature, float humidity) {
//   DynamicJsonDocument doc(200);
//   doc["id"] = "1";
//   doc["name"] =  "temp-humid";
//   doc["data"] = String(temperature) + "-" + String(humidity);
//   doc["unit"] = "C-%";

//   String output;
//   serializeJson(doc, output);  // Serialize the JSON document to a string.
//   return output;
// }


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

const std::array<RPC_Callback, 1U> callbacks = {
  RPC_Callback{"setLedSwitchValue", setLedSwitchState}
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

  // Khởi tạo WiFi, sensor
  InitWiFi();
  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();

  // Tạo các task
  
  xTaskCreatePinnedToCore(taskReconnectWiFi,  "WiFiCheck",     2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskConnectCoreIoT, "ConnectCoreIoT",2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendAttributes, "SendAttributes",4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendTelemetry,  "SendTelemetry", 4096, NULL, 1, NULL, 1);
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

// 4) Task gửi dữ liệu telemetry (mỗi ~10s)
void taskSendTelemetry(void *pvParameters) {
  for (;;) {
    if (tb.connected()) {
      if (millis() - previousDataSend > telemetrySendInterval) {
        previousDataSend = millis();

        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();

        if (isnan(temperature) || isnan(humidity)) {
          Serial.println("Failed to read from DHT20 sensor!");
        } else {
          // Tạo JSON theo format
          // String jsonPayload = DHT20JSON(temperature, humidity);
          String jsonPayload_humid = DHT20JSON_humid(humidity);
          String jsonPayload_temp = DHT20JSON_temp(temperature);


          // tb.sendTelemetryData("", jsonPayload.c_str());
          // tb.sendTelemetryData("humidity", jsonPayload_humid.c_str());
          // tb.sendTelemetryData("temperature", jsonPayload_temp.c_str());
          // tb.sendTelemetryData("humidity", DHT20JSON_humid(humidity));
          // tb.sendTelemetryData("temperature", DHT20JSON_temp(temperature));
          // tb.sendTelemetryData("v1/devices/me/attributes", jsonPayload_humid.c_str());
          // tb.sendTelemetryData("v2/devices/me/attributes", jsonPayload_temp.c_str());
          // Serial.println(jsonPayload_temp.c_str());
          // Serial.println(jsonPayload_humid.c_str());

          tb.sendTelemetryData("temperature", temperature);     //{"key1":"value1",}
          tb.sendTelemetryData("humidity", humidity);           //{"key2":"value2",}
          // Serial.println(temperature);
          // Serial.println(humidity);
          Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n", temperature, humidity);
  
          


          // Gửi thêm một số attribute liên quan WiFi
          tb.sendAttributeData("rssi", WiFi.RSSI());
          tb.sendAttributeData("channel", WiFi.channel());
          tb.sendAttributeData("bssid", WiFi.BSSIDstr().c_str());
          tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
          tb.sendAttributeData("ssid", WiFi.SSID().c_str());
        } // Thêm dấu đóng ngoặc nhọn này
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
} // Thêm dấu đóng ngoặc nhọn nàyv1/devices/me/attributes