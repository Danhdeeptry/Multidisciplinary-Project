#include "MqttClient.h"
extern unsigned long dayStartTime;

// WiFi/MQTT client objects
WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// RPC callback implementations
RPC_Response setLedSwitchState(const RPC_Data &data) {
    bool newState = data;
    digitalWrite(LED_PIN, newState);
    attributesChanged = true;
    return RPC_Response("setLedSwitchValue", newState);
}

RPC_Response readSensorData(const RPC_Data &data) {
    float temperature = readTemperature();
    float humidity = readHumidity();
    float lightIntensity = readLightSensor();
    float dli = getTotalDLI();

    StaticJsonDocument<300> doc;
    doc["temperature"] = serialized(String(temperature, 2));
    doc["humidity"] = serialized(String(humidity, 2));
    doc["light_intensity"] = serialized(String(lightIntensity, 1));
    doc["grow_light_state"] = growLightState;
    doc["auto_light_mode"] = autoLightMode;
    doc["daily_light_integral"] = serialized(String(dli, 3));
    doc["timestamp"] = millis();

    String response;
    response.reserve(250);
    serializeJson(doc, response);

    return RPC_Response("readSensorData", response.c_str());
}

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
    return RPC_Response("setAutoLightMode", autoLightMode);
}

// Callback arrays
std::array<RPC_Callback, 4> callbacks = {
    RPC_Callback{"setLedSwitchValue", setLedSwitchState},
    RPC_Callback{"readSensorData", readSensorData},
    RPC_Callback{"setGrowLightState", setGrowLightState},
    RPC_Callback{"setAutoLightMode", setAutoLightMode}
};

void processSharedAttributes(const Shared_Attribute_Data &data) {
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (strcmp(it->key().c_str(), BLINKING_INTERVAL_ATTR) == 0) {
            // ... xử lý nếu cần
        } else if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0) {
            digitalWrite(LED_PIN, it->value().as<bool>());
        } else if (strcmp(it->key().c_str(), GROW_LIGHT_STATE_ATTR) == 0) {
            growLightState = it->value().as<bool>();
            digitalWrite(GROW_LIGHT_PIN, growLightState);
        } else if (strcmp(it->key().c_str(), AUTO_LIGHT_MODE_ATTR) == 0) {
            autoLightMode = it->value().as<bool>();
        } else if (strcmp(it->key().c_str(), LIGHT_THRESHOLD_LOW_ATTR) == 0) {
            float newThreshold = it->value().as<float>();
            if (newThreshold >= 0 && newThreshold <= 2000) lightThresholdLow = newThreshold;
        } else if (strcmp(it->key().c_str(), LIGHT_THRESHOLD_HIGH_ATTR) == 0) {
            float newThreshold = it->value().as<float>();
            if (newThreshold >= 0 && newThreshold <= 2000) lightThresholdHigh = newThreshold;
        } else if (strcmp(it->key().c_str(), DLI_TARGET_ATTR) == 0) {
            float newTarget = it->value().as<float>();
            if (newTarget >= 1.0 && newTarget <= 100.0) dliTarget = newTarget;
        } else if (strcmp(it->key().c_str(), ALERT_ENABLED_ATTR) == 0) {
            alertEnabled = it->value().as<bool>();
        }
    }
    attributesChanged = true;
}

std::array<const char *, 8> SHARED_ATTRIBUTES_LIST = {
    LED_STATE_ATTR,
    BLINKING_INTERVAL_ATTR,
    GROW_LIGHT_STATE_ATTR,
    LIGHT_THRESHOLD_LOW_ATTR,
    LIGHT_THRESHOLD_HIGH_ATTR,
    AUTO_LIGHT_MODE_ATTR,
    DLI_TARGET_ATTR,
    ALERT_ENABLED_ATTR
};

Shared_Attribute_Callback attributes_callback(
    &processSharedAttributes,
    SHARED_ATTRIBUTES_LIST.cbegin(),
    SHARED_ATTRIBUTES_LIST.cend()
);

Attribute_Request_Callback attribute_shared_request_callback(
    &processSharedAttributes,
    SHARED_ATTRIBUTES_LIST.cbegin(),
    SHARED_ATTRIBUTES_LIST.cend()
);

// WiFi/MQTT init
void initWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

bool reconnectWiFi() {
    if (WiFi.status() == WL_CONNECTED) return true;
    initWiFi();
    return (WiFi.status() == WL_CONNECTED);
}

void initMqtt() {
    if (tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
        tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());
        tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend());
        tb.Shared_Attributes_Subscribe(attributes_callback);
        tb.Shared_Attributes_Request(attribute_shared_request_callback);
    }
}

// FreeRTOS task implementations
void reconnectWiFiTask(void *pvParameters) {
    Serial.println("[Task] reconnectWiFiTask started");
    for (;;) {
        reconnectWiFi();
        Serial.println("[Task] reconnectWiFiTask: WiFi checked");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void connectThingsBoardTask(void *pvParameters) {
    Serial.println("[Task] connectThingsBoardTask started");
    for (;;) {
        if (WiFi.status() == WL_CONNECTED && !tb.connected()) {
            Serial.println("[Task] connectThingsBoardTask: Connecting to ThingsBoard...");
            initMqtt();
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void sendAttributesTask(void *pvParameters) {
    Serial.println("[Task] sendAttributesTask started");
    for (;;) {
        if (tb.connected() && attributesChanged) {
            attributesChanged = false;
            tb.sendAttributeData(LED_STATE_ATTR, digitalRead(LED_PIN));
            tb.sendAttributeData(GROW_LIGHT_STATE_ATTR, growLightState);
            tb.sendAttributeData(AUTO_LIGHT_MODE_ATTR, autoLightMode);
            tb.sendAttributeData(LIGHT_THRESHOLD_LOW_ATTR, lightThresholdLow);
            tb.sendAttributeData(LIGHT_THRESHOLD_HIGH_ATTR, lightThresholdHigh);
            tb.sendAttributeData(DLI_TARGET_ATTR, dliTarget);
            tb.sendAttributeData(ALERT_ENABLED_ATTR, alertEnabled);
            Serial.println("[Task] sendAttributesTask: Attributes sent to ThingsBoard");
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void sendTelemetryTask(void *pvParameters) {
    Serial.println("[Task] sendTelemetryTask started");
    for (;;) {
        if (tb.connected()) {
            static uint32_t previousDataSend = 0;
            static uint8_t wifiAttrCounter = 0;
            if (millis() - previousDataSend > TELEMETRY_SEND_INTERVAL) {
                previousDataSend = millis();
                // Chỉ đọc DHT20 một lần mỗi chu kỳ
                updateDHT20();
                float temperature = readTemperature();
                float humidity = readHumidity();
                float lightIntensity = readLightSensor();

                controlGrowLight(lightIntensity);
                updateDLI(lightIntensity);
                checkLightAlerts(lightIntensity);

                StaticJsonDocument<512> telemetryDoc;
                telemetryDoc["temperature"] = temperature;
                telemetryDoc["humidity"] = humidity;
                telemetryDoc["light_intensity"] = lightIntensity;
                telemetryDoc["grow_light_active"] = growLightState;
                telemetryDoc["auto_mode_active"] = autoLightMode;
                telemetryDoc["daily_light_integral"] = getTotalDLI();
                telemetryDoc["lightThresholdLow"] = lightThresholdLow;
                telemetryDoc["lightThresholdHigh"] = lightThresholdHigh;
                telemetryDoc["dli_progress_percent"] = getDLIProgressPercent();
                unsigned long dayElapsed = (millis() - dayStartTime) / 1000;
                float dayHours = (float)(dayElapsed / 3600.0f);
                telemetryDoc["day_elapsed_hours"] = dayHours;
                telemetryDoc["demo_mode"] = true;

                String telemetryJson;
                telemetryJson.reserve(400);
                serializeJson(telemetryDoc, telemetryJson);

                Serial.printf("DEBUG: Sending complete JSON telemetry: %s\n", telemetryJson.c_str());
                bool success = tb.sendTelemetryJson(telemetryJson.c_str());
                Serial.printf("DEBUG: JSON telemetry send result: %s\n", success ? "SUCCESS" : "FAILED");

                if (++wifiAttrCounter >= 20) {
                    wifiAttrCounter = 0;
                    tb.sendAttributeData("rssi", WiFi.RSSI());
                    tb.sendAttributeData("channel", WiFi.channel());
                    tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
                    tb.sendAttributeData("ssid", WiFi.SSID().c_str());
                    Serial.println("[Task] sendTelemetryTask: WiFi attributes updated");
                }
            }
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void TbLoopTask(void *pvParameters) {
    Serial.println("[Task] TbLoopTask started");
    for (;;) {
        if (tb.connected()) tb.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}