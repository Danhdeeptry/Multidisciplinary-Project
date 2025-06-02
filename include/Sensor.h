#pragma once
#include <DHT20.h>
#include <Wire.h>
#include <Arduino.h>
#include "Utils.h"

// Khởi tạo DHT20, trả về true nếu thành công
bool initDHT20();

// Đọc nhiệt độ (°C)
float readTemperature();

// Đọc độ ẩm (%)
float readHumidity();

// Đọc cảm biến ánh sáng analog, trả về lux (ước lượng)
float readLightSensor();

// Cập nhật giá trị nhiệt độ và độ ẩm từ DHT20, trả về true nếu thành công
bool updateDHT20();

// Temperature and humidity alarm configuration and state
extern volatile float tempThresholdMin;
extern volatile float tempThresholdMax;
extern volatile float humidityThresholdMin;
extern volatile float humidityThresholdMax;

// Check and send temperature/humidity alerts
void checkTempHumidityAlerts(float temperature, float humidity);

// Đọc cảm biến độ ẩm đất (soil moisture)
int readSoilMoisture();