#pragma once
#include <Arduino.h>
#include "Utils.h"

// Biến cấu hình và trạng thái grow light
extern volatile bool growLightState;
extern volatile bool autoLightMode;
extern volatile float lightThresholdLow;
extern volatile float lightThresholdHigh;
extern volatile float dliTarget;
extern volatile bool alertEnabled;
extern volatile bool attributesChanged;

// Khởi tạo DLI, grow light, biến tracking
void initLightControl();

// Điều khiển grow light tự động (hysteresis)
void controlGrowLight(float lightIntensity);

// Cập nhật DLI (DEMO MODE 10x)
void updateDLI(float lightIntensity);

// Kiểm tra và gửi cảnh báo ánh sáng
void checkLightAlerts(float lightIntensity);

// Lấy giá trị DLI hiện tại
float getTotalDLI();

// Lấy % tiến độ DLI
float getDLIProgressPercent();

extern float totalDLI;
extern unsigned long dayStartTime;