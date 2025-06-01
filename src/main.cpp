#include <Arduino.h>
#include "Utils.h"
#include "Sensor.h"
#include "LightControl.h"
#include "MqttClient.h"

void setup() {
    Serial.begin(SERIAL_DEBUG_BAUD);
    Serial.println("[MAIN] setup() started");

    // 1) Khởi tạo cảm biến
    if (!initDHT20()) {
        Serial.println("Failed to initialize DHT20 sensor");
    }
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    Serial.println("Light sensor initialized");

    // 2) Khởi tạo LightControl
    initLightControl();

    // 3) Khởi tạo WiFi và MQTT
    initWiFi();

    // Tạo các FreeRTOS task
    xTaskCreatePinnedToCore(reconnectWiFiTask,  "WiFiCheck",     4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(connectThingsBoardTask, "ConnectCoreIoT",8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendAttributesTask, "SendAttributes",4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendTelemetryTask,  "SendTelemetry", 8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(TbLoopTask,         "TbLoop",        4096, NULL, 1, NULL, 1);
}

void loop() {
    // Trống: toàn bộ logic đã nằm trong các task
}