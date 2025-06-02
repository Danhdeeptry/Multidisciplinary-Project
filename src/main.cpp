#include <Arduino.h>
#include "Utils.h"
#include "Sensor.h"
#include "LightControl.h"
#include "MqttClient.h"
#include "ButtonControl.h"

// Khai báo prototype cho callback nút bấm
void onButtonOnPressed();
void onButtonOffPressed();

void ButtonPollTask(void *pvParameters) {
    while (true) {
        updateButtonState(onButtonOnPressed, onButtonOffPressed);
        vTaskDelay(20 / portTICK_PERIOD_MS); // Poll every 20ms
    }
}


void setup() {
    Serial.begin(SERIAL_DEBUG_BAUD);
    Serial.println("[MAIN] setup() started");

    // 1) Khởi tạo cảm biến
    if (!initDHT20()) {
        Serial.println("Failed to initialize DHT20 sensor");
    }
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    Serial.println("Light sensor initialized");    // 2) Khởi tạo LightControl
    Serial.println("Soil moisture sensor initialized");
    initLightControl();

    // 3) Khởi tạo ButtonControl
    initButtonControl();
    Serial.println("Button control initialized");

    // 4) Khởi tạo WiFi và MQTT
    initWiFi();

    // Tạo các FreeRTOS task
    xTaskCreatePinnedToCore(reconnectWiFiTask,  "WiFiCheck",     4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(connectThingsBoardTask, "ConnectCoreIoT",8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendAttributesTask, "SendAttributes",4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendTelemetryTask,  "SendTelemetry", 8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(TbLoopTask,         "TbLoop",        4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(ButtonPollTask, "ButtonPoll", 2048, NULL, 1, NULL, 1);
}

void onButtonOnPressed() {
    // Toggle grow light
    growLightState = !growLightState;
    digitalWrite(GROW_LIGHT_PIN, growLightState ? HIGH : LOW);
    attributesChanged = true;
    Serial.printf("[Button] Grow light toggled: %s\n", growLightState ? "ON" : "OFF");
}

void onButtonOffPressed() {
    // Toggle auto/manual mode
    autoLightMode = !autoLightMode;
    attributesChanged = true;
    Serial.printf("[Button] Light mode toggled: %s\n", autoLightMode ? "AUTO" : "MANUAL");
}

void loop() {
    // logic nằm trong các task
}