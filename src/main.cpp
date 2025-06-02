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
    Serial.println("Light sensor initialized");    // 2) Khởi tạo LightControl
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
    growLightState = true;
    digitalWrite(GROW_LIGHT_PIN, HIGH);
    attributesChanged = true;
    Serial.println("[Button] Grow light turned ON by button");
}

void onButtonOffPressed() {
    growLightState = false;
    digitalWrite(GROW_LIGHT_PIN, LOW);
    attributesChanged = true;
    Serial.println("[Button] Grow light turned OFF by button");
}

void loop() {
    // logic nằm trong các task
}