// File: main.cpp
#define LED_PIN 48
#define SDA_PIN GPIO_NUM_11
#define SCL_PIN GPIO_NUM_12
#define LIGHT_SENSOR_PIN 1  // Cảm biến ánh sáng analog kết nối GPIO1 (ADC1_CH0)
#define GROW_LIGHT_PIN GPIO_NUM_6  // Pin điều khiển đèn LED grow light cho Smart Farm

#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include "DHT20.h"
#include "Wire.h"
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

constexpr char WIFI_SSID[] = "P4.11";               
constexpr char WIFI_PASSWORD[] = "123456788";      

constexpr char TOKEN[] = "lr3yyoh7v2b98pdmuj38";   // Device access token từ ThingsBoard

constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";   //server(broker) ThingsBoard
constexpr uint16_t THINGSBOARD_PORT = 1883U;

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";
constexpr char LED_MODE_ATTR[] = "ledMode";
constexpr char LED_STATE_ATTR[] = "ledState";

// Smart Farm attributes
constexpr char GROW_LIGHT_STATE_ATTR[] = "growLightState";
constexpr char LIGHT_THRESHOLD_LOW_ATTR[] = "lightThresholdLow";
constexpr char LIGHT_THRESHOLD_HIGH_ATTR[] = "lightThresholdHigh";
constexpr char AUTO_LIGHT_MODE_ATTR[] = "autoLightMode";
constexpr char DLI_TARGET_ATTR[] = "dliTarget";
constexpr char ALERT_ENABLED_ATTR[] = "alertEnabled";

// Smart Farm variables
volatile bool attributesChanged = false;
volatile int ledMode = 0;
volatile bool ledState = false;
volatile bool growLightState = false;
volatile bool autoLightMode = true;
volatile float lightThresholdLow = 200.0;   // lux - turn on grow light when below this
volatile float lightThresholdHigh = 300.0;  // lux - turn off grow light when above this
volatile float dliTarget = 20.0;            // mol/m²/day - Daily Light Integral target
volatile bool alertEnabled = true;

// Smart Farm tracking variables
float totalDLI = 0.0;                       // Current day DLI accumulation - Lượng DLI tích lũy trong ngày (mol/m²/day)
unsigned long lastDLIUpdate = 0;
unsigned long dayStartTime = 0;
bool lowLightAlertSent = false;
bool highLightAlertSent = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

uint32_t previousStateChange = 0;

constexpr int16_t telemetrySendInterval = 10000U;
// constexpr int16_t telemetrySendInterval = 500U;   // Gửi dữ liệu mỗi 0.5 giây (high real-time)
// constexpr int16_t telemetrySendInterval = 250U;   // Gửi dữ liệu mỗi 0.25 giây (ultra real-time)
// constexpr int16_t telemetrySendInterval = 100U;   // Gửi dữ liệu mỗi 0.1 giây (maximum real-time)

uint32_t previousDataSend = 0;

constexpr std::array<const char *, 8U> SHARED_ATTRIBUTES_LIST = {
  LED_STATE_ATTR,
  BLINKING_INTERVAL_ATTR,
  GROW_LIGHT_STATE_ATTR,
  LIGHT_THRESHOLD_LOW_ATTR,
  LIGHT_THRESHOLD_HIGH_ATTR,
  AUTO_LIGHT_MODE_ATTR,
  DLI_TARGET_ATTR,
  ALERT_ENABLED_ATTR
};

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

DHT20 dht20;

// Biến lưu trữ trạng thái cảm biến
bool sensorInitialized = false;
bool lightSensorInitialized = false;

// Thêm hàm DHT20JSON ở đây với tối ưu bộ nhớ - DEPRECATED: Sử dụng batch telemetry thay thế
/*
String DHT20JSON_temp(float temperature) {
  StaticJsonDocument<128> doc;  // Sử dụng StaticJsonDocument thay vì Dynamic để tránh heap fragmentation
  doc["id"] = "1";
  doc["name"] =  "temperature";
  doc["data"] = serialized(String(temperature, 2));  // Giới hạn 2 số thập phân
  doc["unit"] = "C";

  String output;
  output.reserve(100);  // Pre-allocate string capacity
  serializeJson(doc, output);
  return output;
}
*/

/*
String DHT20JSON_humid(float humidity) {
  StaticJsonDocument<128> doc;  // Sử dụng StaticJsonDocument thay vì Dynamic
  doc["id"] = "2";
  doc["name"] =  "humidity";
  doc["data"] = serialized(String(humidity, 2));  // Giới hạn 2 số thập phân
  doc["unit"] = "%";

  String output;
  output.reserve(100);  // Pre-allocate string capacity
  serializeJson(doc, output);
  return output;
}
*/

// Hàm đọc cảm biến ánh sáng và chuyển đổi thành lux (ước lượng)
float readLightSensor() {
  int rawValue = analogRead(LIGHT_SENSOR_PIN);
  
  // Chuyển đổi giá trị ADC (0-4095) thành lux ước lượng
  // Công thức này có thể cần hiệu chỉnh dựa trên đặc tính thực tế của cảm biến
  float voltage = (rawValue * 3.3) / 4095.0;  // ESP32 ADC 12-bit, Vref = 3.3V
  float lux = rawValue * 0.1;  // Ước lượng: 1 ADC count ≈ 0.1 lux (cần hiệu chỉnh)
  
  return lux;
}

// Smart Farm functions
void controlGrowLight(float lightIntensity) {
  Serial.printf("DEBUG: controlGrowLight called - Light: %.1f lux, AutoMode: %s, Current State: %s\n", 
                lightIntensity, autoLightMode ? "ON" : "OFF", growLightState ? "ON" : "OFF");
  
  if (!autoLightMode) {
    Serial.println("DEBUG: Auto mode disabled, skipping control");
    return; // Manual mode - don't auto control
  }
  
  bool shouldTurnOn = false;
  
  // Hysteresis control to prevent frequent switching
  if (!growLightState && lightIntensity < lightThresholdLow) {
    shouldTurnOn = true;
    Serial.printf("DEBUG: Should turn ON - Light %.1f < threshold %.1f\n", lightIntensity, lightThresholdLow);
  } else if (growLightState && lightIntensity > lightThresholdHigh) {
    shouldTurnOn = false;
    Serial.printf("DEBUG: Should turn OFF - Light %.1f > threshold %.1f\n", lightIntensity, lightThresholdHigh);
  } else {
    shouldTurnOn = growLightState; // Keep current state
    Serial.printf("DEBUG: Keep current state - Light %.1f in hysteresis range\n", lightIntensity);
  }
  
  if (shouldTurnOn != growLightState) {
    growLightState = shouldTurnOn;
    digitalWrite(GROW_LIGHT_PIN, growLightState);
    
    Serial.printf("Grow light %s (Auto mode) - Light: %.1f lux\n", 
                  growLightState ? "ON" : "OFF", lightIntensity);
    
    // Send state update to ThingsBoard
    attributesChanged = true;
  } else {
    Serial.printf("DEBUG: No state change needed - shouldTurnOn: %s, current: %s\n", 
                  shouldTurnOn ? "ON" : "OFF", growLightState ? "ON" : "OFF");
  }
}

// Calculate Daily Light Integral (DLI) in mol/m²/day
void updateDLI(float lightIntensity) {
  unsigned long currentTime = millis();
  
  // Reset DLI at start of new day (every 24 hours)
  if (dayStartTime == 0 || (currentTime - dayStartTime) >= 86400000) { // 24 hours in ms
    dayStartTime = currentTime;
    totalDLI = 0.0;
    Serial.println("New day started - DLI reset to 0");
  }
  
  if (lastDLIUpdate != 0) {
    // Convert lux to PPFD (approximate: 1 lux ≈ 0.0185 µmol/m²/s for sunlight)
    float ppfd = lightIntensity * 0.0185; // µmol/m²/s
    
    // Calculate time difference in seconds
    float timeDiff = (currentTime - lastDLIUpdate) / 1000.0;
    
    // Add to DLI: PPFD * time * conversion factor
    // 1 mol/m²/day = 1,000,000 µmol/m²/s * 86400 s = 86.4 billion µmol/m²/day
    float dliIncrement = (ppfd * timeDiff) / 1000000.0; // Convert µmol to mol
    totalDLI += dliIncrement;
  }
  
  lastDLIUpdate = currentTime;
}

// Send alerts based on light conditions
void checkLightAlerts(float lightIntensity) {
  if (!alertEnabled) {
    return;
  }
  
  // Low light alert
  if (lightIntensity < lightThresholdLow && !lowLightAlertSent) {
    Telemetry alertData[3] = {
      Telemetry("alert_type", "LOW_LIGHT"),
      Telemetry("alert_message", "Light intensity below threshold"),
      Telemetry("alert_value", lightIntensity)
    };
    tb.sendTelemetry(alertData, 3);
    lowLightAlertSent = true;
    highLightAlertSent = false; // Reset opposite alert
    Serial.printf("LOW LIGHT ALERT: %.1f lux < %.1f lux threshold\n", lightIntensity, lightThresholdLow);
  }
  
  // High light alert (excessive light can damage plants)
  else if (lightIntensity > (lightThresholdHigh * 2) && !highLightAlertSent) {
    Telemetry alertData[3] = {
      Telemetry("alert_type", "HIGH_LIGHT"),
      Telemetry("alert_message", "Light intensity excessively high"),
      Telemetry("alert_value", lightIntensity)
    };
    tb.sendTelemetry(alertData, 3);
    highLightAlertSent = true;
    lowLightAlertSent = false; // Reset opposite alert
    Serial.printf("HIGH LIGHT ALERT: %.1f lux > %.1f lux threshold\n", lightIntensity, lightThresholdHigh * 2);
  }
  
  // Reset alerts when light returns to normal range
  else if (lightIntensity >= lightThresholdLow && lightIntensity <= lightThresholdHigh * 2) {
    if (lowLightAlertSent || highLightAlertSent) {
      lowLightAlertSent = false;
      highLightAlertSent = false;
      Telemetry alertData[2] = {
        Telemetry("alert_type", "NORMAL"),
        Telemetry("alert_message", "Light intensity returned to normal")
      };
      tb.sendTelemetry(alertData, 2);
      Serial.println("Light alerts cleared - intensity normal");
    }
  }
}

/*
String LightSensorJSON(float lightIntensity) {
  StaticJsonDocument<128> doc;
  doc["id"] = "3";
  doc["name"] = "light_intensity";
  doc["data"] = serialized(String(lightIntensity, 1));  // 1 số thập phân
  doc["unit"] = "lux";

  String output;
  output.reserve(100);
  serializeJson(doc, output);
  return output;
}
*/

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

// RPC để đọc dữ liệu cảm biến với tối ưu bộ nhớ
RPC_Response readSensorData(const RPC_Data &data) {
  float temperature = dht20.getTemperature();
  float humidity = dht20.getHumidity();
  float lightIntensity = readLightSensor();
  
  StaticJsonDocument<300> doc;  // Tăng size để chứa thêm Smart Farm data
  doc["temperature"] = serialized(String(temperature, 2));
  doc["humidity"] = serialized(String(humidity, 2));
  doc["light_intensity"] = serialized(String(lightIntensity, 1));
  doc["grow_light_state"] = growLightState;
  doc["auto_light_mode"] = autoLightMode;
  doc["daily_light_integral"] = serialized(String(totalDLI, 3));
  doc["timestamp"] = millis();
  
  String response;
  response.reserve(250);  // Pre-allocate
  serializeJson(doc, response);
  
  Serial.print("Manual sensor read: ");
  Serial.println(response);
  
  return RPC_Response("readSensorData", response.c_str());
}

// RPC for Smart Farm control
RPC_Response setGrowLightState(const RPC_Data &data) {
  bool newState = data;
  growLightState = newState;
  digitalWrite(GROW_LIGHT_PIN, growLightState);
  attributesChanged = true;
  
  Serial.printf("Grow light manually set to: %s\n", growLightState ? "ON" : "OFF");
  return RPC_Response("setGrowLightState", growLightState);
}

RPC_Response setAutoLightMode(const RPC_Data &data) {
  bool newMode = data;
  autoLightMode = newMode;
  attributesChanged = true;
  
  Serial.printf("Auto light mode set to: %s\n", autoLightMode ? "ENABLED" : "DISABLED");
  return RPC_Response("setAutoLightMode", autoLightMode);
}

RPC_Response resetDLI(const RPC_Data &data) {
  totalDLI = 0.0;
  dayStartTime = millis();
  
  Serial.println("Daily Light Integral manually reset");
  return RPC_Response("resetDLI", "DLI reset successfully");
}

const std::array<RPC_Callback, 5U> callbacks = {
  RPC_Callback{"setLedSwitchValue", setLedSwitchState},
  RPC_Callback{"readSensorData", readSensorData},
  RPC_Callback{"setGrowLightState", setGrowLightState},
  RPC_Callback{"setAutoLightMode", setAutoLightMode},
  RPC_Callback{"resetDLI", resetDLI}
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
    // Smart Farm attribute processing
    else if (strcmp(it->key().c_str(), GROW_LIGHT_STATE_ATTR) == 0) {
      growLightState = it->value().as<bool>();
      digitalWrite(GROW_LIGHT_PIN, growLightState);
      Serial.print("Grow light state is set to: ");
      Serial.println(growLightState);
    } else if (strcmp(it->key().c_str(), AUTO_LIGHT_MODE_ATTR) == 0) {
      autoLightMode = it->value().as<bool>();
      Serial.print("Auto light mode is set to: ");
      Serial.println(autoLightMode ? "ENABLED" : "DISABLED");
    } else if (strcmp(it->key().c_str(), LIGHT_THRESHOLD_LOW_ATTR) == 0) {
      float newThreshold = it->value().as<float>();
      if (newThreshold >= 0 && newThreshold <= 2000) {
        lightThresholdLow = newThreshold;
        Serial.print("Light threshold low is set to: ");
        Serial.println(lightThresholdLow);
      }
    } else if (strcmp(it->key().c_str(), LIGHT_THRESHOLD_HIGH_ATTR) == 0) {
      float newThreshold = it->value().as<float>();
      if (newThreshold >= 0 && newThreshold <= 2000) {
        lightThresholdHigh = newThreshold;
        Serial.print("Light threshold high is set to: ");
        Serial.println(lightThresholdHigh);
      }
    } else if (strcmp(it->key().c_str(), DLI_TARGET_ATTR) == 0) {
      float newTarget = it->value().as<float>();
      if (newTarget >= 1.0 && newTarget <= 100.0) {
        dliTarget = newTarget;
        Serial.print("DLI target is set to: ");
        Serial.println(dliTarget);
      }
    } else if (strcmp(it->key().c_str(), ALERT_ENABLED_ATTR) == 0) {
      alertEnabled = it->value().as<bool>();
      Serial.print("Alert system is set to: ");
      Serial.println(alertEnabled ? "ENABLED" : "DISABLED");
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
  // Khởi tạo I2C cho cảm biến DHT20
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // Set I2C frequency to 100kHz for better compatibility
  
  Serial.println("Scanning I2C bus for devices...");
  bool deviceFound = false;
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C device found at address 0x%02X\n", i);
      deviceFound = true;
    }
  }
  if (!deviceFound) {
    Serial.println("No I2C devices found! Check connections.");
  }
  
  // Khởi tạo cảm biến DHT20
  Serial.println("Initializing DHT20 sensor...");
  delay(100); // Give sensor time to power up
  
  if (dht20.begin()) {
    Serial.println("DHT20 sensor initialized successfully");
    
    // Check if sensor is properly calibrated
    if (dht20.isCalibrated()) {
      Serial.println("DHT20 sensor is calibrated");
    } else {
      Serial.println("DHT20 sensor is NOT calibrated");
    }
    
    // Check connection
    if (dht20.isConnected()) {
      Serial.println("DHT20 sensor is connected");
      sensorInitialized = true;
    } else {
      Serial.println("DHT20 sensor connection failed");
      sensorInitialized = false;
    }  } else {
    Serial.println("Failed to initialize DHT20 sensor");
    sensorInitialized = false;
  }
    // Khởi tạo cảm biến ánh sáng analog
  Serial.println("Initializing Light Sensor...");
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  
  // Test đọc cảm biến ánh sáng
  int testRead = analogRead(LIGHT_SENSOR_PIN);
  Serial.printf("Light sensor test read: %d (raw ADC value)\n", testRead);
  
  if (testRead >= 0) {  // ADC luôn trả về giá trị hợp lệ
    lightSensorInitialized = true;
    Serial.println("Light sensor initialized successfully");
  } else {
    lightSensorInitialized = false;
    Serial.println("Light sensor initialization failed");
  }
  
  // Khởi tạo Smart Farm grow light
  Serial.println("Initializing Smart Farm grow light...");
  pinMode(GROW_LIGHT_PIN, OUTPUT);
  digitalWrite(GROW_LIGHT_PIN, growLightState); // Initial state
  Serial.printf("Grow light pin %d initialized - State: %s\n", GROW_LIGHT_PIN, growLightState ? "ON" : "OFF");
  
  // Initialize DLI tracking
  dayStartTime = millis();
  lastDLIUpdate = millis();
  totalDLI = 0.0;
  Serial.println("Daily Light Integral tracking initialized");
  
  // Khởi tạo WiFi
  InitWiFi();
  Serial.println("Smart Farm IoT system activated with comprehensive features:");
  Serial.println("- Real-time sensor monitoring (Temperature, Humidity, Light)");
  Serial.println("- Automatic grow light control with thresholds");
  Serial.println("- Daily Light Integral (DLI) calculation");
  Serial.println("- Alert system for abnormal conditions");
  Serial.println("- Remote control via ThingsBoard RPC");
  Serial.println("Using DHT20 sensor for temperature and humidity");
  // Tạo các task với stack size tăng lên để tránh stack overflow
  
  xTaskCreatePinnedToCore(taskReconnectWiFi,  "WiFiCheck",     4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskConnectCoreIoT, "ConnectCoreIoT",8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendAttributes, "SendAttributes",4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendTelemetry,  "SendTelemetry", 8192, NULL, 1, NULL, 1);  // Tăng lên nhiều nhất vì xử lý JSON, float, MQTT
  xTaskCreatePinnedToCore(taskTbLoop,         "TbLoop",        4096, NULL, 1, NULL, 1);
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
      
      // Gửi Smart Farm attributes
      tb.sendAttributeData(GROW_LIGHT_STATE_ATTR, growLightState);
      tb.sendAttributeData(AUTO_LIGHT_MODE_ATTR, autoLightMode);
      tb.sendAttributeData(LIGHT_THRESHOLD_LOW_ATTR, lightThresholdLow);
      tb.sendAttributeData(LIGHT_THRESHOLD_HIGH_ATTR, lightThresholdHigh);
      tb.sendAttributeData(DLI_TARGET_ATTR, dliTarget);
      tb.sendAttributeData(ALERT_ENABLED_ATTR, alertEnabled);
      
      Serial.println("Smart Farm attributes updated to ThingsBoard");
    }
    // Kiểm tra ~500ms/lần
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// 4) Task gửi dữ liệu telemetry (mỗi ~5s)
void taskSendTelemetry(void *pvParameters) {
  static float lastValidTemperature = 0.0;
  static float lastValidHumidity = 0.0;
  static bool hasValidDHT20Data = false;
  static bool firstRun = true;
  for (;;) {
    if (tb.connected()) {
      if (millis() - previousDataSend > telemetrySendInterval) {
        previousDataSend = millis();
        Serial.printf("Telemetry interval: %d ms, sending data...\n", telemetrySendInterval);
        float temperature = lastValidTemperature;
        float humidity = lastValidHumidity;
        float lightIntensity = 0.0;
        bool dht20DataUpdated = false;
        if (firstRun) {
          // Đảm bảo lần đầu sẽ đọc DHT20 ngay
          firstRun = false;
        }        if (lightSensorInitialized) {
          lightIntensity = readLightSensor();
          
          Serial.printf("DEBUG: Light sensor read: %.1f lux (thresholds: %.1f - %.1f)\n", 
                        lightIntensity, lightThresholdLow, lightThresholdHigh);
          
          // Smart Farm features
          controlGrowLight(lightIntensity);  // Auto control grow light
          updateDLI(lightIntensity);         // Update Daily Light Integral
          checkLightAlerts(lightIntensity);  // Check for alerts
        }uint32_t timeSinceLastRead = millis() - dht20.lastRead();
        Serial.printf("Time since last DHT20 read: %d ms\n", timeSinceLastRead);
        if (sensorInitialized && (timeSinceLastRead >= 1000)) {
          Serial.println("Reading DHT20...");
          int result = dht20.read();
          // Không cần cập nhật lastDHT20Read, đã có dht20.lastRead() quản lý
          if (result == DHT20_OK) {
            float newTemperature = dht20.getTemperature();
            float newHumidity = dht20.getHumidity();
            if (!isnan(newTemperature) && !isnan(newHumidity) && newTemperature != 0.0 && newHumidity != 0.0) {
              temperature = newTemperature;
              humidity = newHumidity;
              lastValidTemperature = temperature;
              lastValidHumidity = humidity;
              hasValidDHT20Data = true;
              dht20DataUpdated = true;
            }          } else if (result != -15) {  // Không log lỗi -15 (DHT20_ERROR_LASTREAD)
            Serial.printf("DHT20 read error: %d\n", result);
            if (result == DHT20_ERROR_CONNECT) {
              Serial.println("Sensor connection lost, attempting to reinitialize...");
              if (dht20.begin()) {
                Serial.println("Sensor reinitialized successfully");
              } else {
                Serial.println("Sensor reinitialization failed");
                sensorInitialized = false;
              }
            }
          }
        }        // Create complete JSON string for all telemetry data
        StaticJsonDocument<512> telemetryDoc;
        
        // Add sensor data to JSON
        if (hasValidDHT20Data) {
          telemetryDoc["temperature"] = temperature;
          telemetryDoc["humidity"] = humidity;
        }
        if (lightSensorInitialized) {
          telemetryDoc["light_intensity"] = lightIntensity;
        }
        
        // Add Smart Farm telemetry data
        telemetryDoc["grow_light_active"] = growLightState;
        telemetryDoc["auto_mode_active"] = autoLightMode;
        telemetryDoc["daily_light_integral"] = totalDLI;
        
        // Calculate DLI progress percentage
        float dliProgress = (totalDLI / dliTarget) * 100.0;
        telemetryDoc["dli_progress_percent"] = dliProgress;
        
        // Send time-based data
        unsigned long dayElapsed = (millis() - dayStartTime) / 1000; // seconds
        telemetryDoc["day_elapsed_hours"] = (float)(dayElapsed / 3600.0);
        
        // Convert to JSON string
        String telemetryJson;
        telemetryJson.reserve(400);
        serializeJson(telemetryDoc, telemetryJson);
        
        // Send complete JSON as one telemetry message
        if (telemetryJson.length() > 2) { // Check if JSON has content
          Serial.printf("DEBUG: Sending complete JSON telemetry: %s\n", telemetryJson.c_str());
          bool success = tb.sendTelemetryJson(telemetryJson.c_str());
          Serial.printf("DEBUG: JSON telemetry send result: %s\n", success ? "SUCCESS" : "FAILED");
        } else {
          Serial.println("DEBUG: No telemetry JSON data to send");
        }
          // Log giá trị telemetry đã gửi
        Serial.printf("DEBUG: hasValidDHT20Data=%s, lightSensorInitialized=%s\n", 
                      hasValidDHT20Data ? "true" : "false", 
                      lightSensorInitialized ? "true" : "false");
        
        if (hasValidDHT20Data && lightSensorInitialized) {
          Serial.printf("Sent telemetry - Temperature: %.2f C, Humidity: %.2f %%, Light: %.1f lux\n", 
                        temperature, humidity, lightIntensity);
          Serial.printf("Smart Farm - Grow Light: %s, Auto Mode: %s, DLI: %.3f/%.1f (%.1f%%)\n",
                        growLightState ? "ON" : "OFF", 
                        autoLightMode ? "AUTO" : "MANUAL",
                        totalDLI, dliTarget, dliProgress);
        } else if (hasValidDHT20Data) {
          Serial.printf("Sent telemetry - Temperature: %.2f C, Humidity: %.2f %%\n", 
                        temperature, humidity);
          Serial.printf("Smart Farm (DHT20 only) - Grow Light: %s, Auto Mode: %s, DLI: %.3f/%.1f\n",
                        growLightState ? "ON" : "OFF", 
                        autoLightMode ? "AUTO" : "MANUAL",
                        totalDLI, dliTarget);
        } else if (lightSensorInitialized) {
          Serial.printf("Sent telemetry - Light: %.1f lux\n", lightIntensity);
          Serial.printf("Smart Farm (Light only) - Grow Light: %s, Auto Mode: %s\n",
                        growLightState ? "ON" : "OFF", 
                        autoLightMode ? "AUTO" : "MANUAL");
        }
        
        static uint8_t wifiAttrCounter = 0;
        if (++wifiAttrCounter >= 20) {
          wifiAttrCounter = 0;
          tb.sendAttributeData("rssi", WiFi.RSSI());
          tb.sendAttributeData("channel", WiFi.channel());
          tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
          tb.sendAttributeData("ssid", WiFi.SSID().c_str());
          Serial.println("WiFi attributes updated");
        }
      }
    }
    vTaskDelay(250 / portTICK_PERIOD_MS);
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