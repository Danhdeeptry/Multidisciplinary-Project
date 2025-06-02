#pragma once
#include <Arduino.h>

// // Chân nút bấm trên Yolo UNO
// #define BUTTON_ON_PIN GPIO_NUM_3 // D5 - bật đèn
// #define BUTTON_OFF_PIN GPIO_NUM_4  // D6 - tắt đèn

#define BUTTON_ON_PIN GPIO_NUM_8 // D5 - bật đèn
#define BUTTON_OFF_PIN GPIO_NUM_9 // D6 - tắt đèn

// Khởi tạo nút bấm (gọi trong setup)
void initButtonControl();

// Gọi trong loop để kiểm tra trạng thái nút
void updateButtonState(void (*onPress)(), void (*offPress)());


// void ButtonPollTask(void *pvParameters) {
//     while (true) {
//         int onState = digitalRead(BUTTON_ON_PIN);
//         int offState = digitalRead(BUTTON_OFF_PIN);
//         Serial.printf("D5: %d, D6: %d\n", onState, offState);
//         vTaskDelay(200 / portTICK_PERIOD_MS);
//     }
// }
