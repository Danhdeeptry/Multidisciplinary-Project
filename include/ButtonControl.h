#pragma once
#include <Arduino.h>


#define BUTTON_ON_PIN GPIO_NUM_8 // D5 - bật đèn
#define BUTTON_OFF_PIN GPIO_NUM_9 // D6 - tắt đèn

// Khởi tạo nút bấm (gọi trong setup)
void initButtonControl();

// Gọi trong loop để kiểm tra trạng thái nút
void updateButtonState(void (*onPress)(), void (*offPress)());