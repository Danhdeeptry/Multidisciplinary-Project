#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "config.h"

#define LED_PIN 48
#define SDA_PIN GPIO_NUM_11
#define SCL_PIN GPIO_NUM_12
#define LIGHT_SENSOR_PIN 1
#define GROW_LIGHT_PIN GPIO_NUM_6

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000;
constexpr int16_t TELEMETRY_SEND_INTERVAL = 5000;       // time send telemetry (ms)

// Remove secret values from here, now loaded from config.h
// constexpr char WIFI_SSID[] = "P4.11";
// constexpr char WIFI_PASSWORD[] = "123456788";
// constexpr char TOKEN[] = "lr3yyoh7v2b98pdmuj38";
// constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
// constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

// Attribute keys
constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";
constexpr char LED_MODE_ATTR[] = "ledMode";
constexpr char LED_STATE_ATTR[] = "ledState";
constexpr char GROW_LIGHT_STATE_ATTR[] = "growLightState";
constexpr char LIGHT_THRESHOLD_LOW_ATTR[] = "lightThresholdLow";
constexpr char LIGHT_THRESHOLD_HIGH_ATTR[] = "lightThresholdHigh";
constexpr char AUTO_LIGHT_MODE_ATTR[] = "autoLightMode";
constexpr char DLI_TARGET_ATTR[] = "dliTarget";
constexpr char ALERT_ENABLED_ATTR[] = "alertEnabled";

// Temperature and humidity alarm thresholds
constexpr char TEMP_THRESHOLD_MIN_ATTR[] = "tempThresholdMin";
constexpr char TEMP_THRESHOLD_MAX_ATTR[] = "tempThresholdMax";
constexpr char HUMIDITY_THRESHOLD_MIN_ATTR[] = "humidityThresholdMin";
constexpr char HUMIDITY_THRESHOLD_MAX_ATTR[] = "humidityThresholdMax";

// Helper function (nếu cần)
inline float convertLuxToPPFD(float lux) { return lux * 0.0185f; }