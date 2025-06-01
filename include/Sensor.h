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