#include "ButtonControl.h"

// Biến lưu trạng thái trước đó để phát hiện nhấn mới
static bool lastButtonOnState = HIGH;
static bool lastButtonOffState = HIGH;
static bool debouncedButtonOnState = HIGH;
static bool debouncedButtonOffState = HIGH;
static unsigned long lastDebounceTimeOn = 0;
static unsigned long lastDebounceTimeOff = 0;
const unsigned long debounceDelay = 30; // ms

void initButtonControl() {
    pinMode(BUTTON_ON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_OFF_PIN, INPUT_PULLUP);
    lastButtonOnState = digitalRead(BUTTON_ON_PIN);
    lastButtonOffState = digitalRead(BUTTON_OFF_PIN);
    debouncedButtonOnState = lastButtonOnState;
    debouncedButtonOffState = lastButtonOffState;
}

// Gọi trong loop, truyền vào 2 callback cho từng nút
void updateButtonState(void (*onPress)(), void (*offPress)()) {
    bool readingOn = digitalRead(BUTTON_ON_PIN);
    bool readingOff = digitalRead(BUTTON_OFF_PIN);
    unsigned long now = millis();

    // Serial.printf("[DEBUG] Button ON (GPIO %d): %d, Button OFF (GPIO %d): %d\n", BUTTON_ON_PIN, readingOn, BUTTON_OFF_PIN, readingOff);

    // Debounce cho nút ON
    if (readingOn != lastButtonOnState) {
        // Serial.printf("[DEBUG] ON state changed: %d -> %d\n", lastButtonOnState, readingOn);
        lastDebounceTimeOn = now;
    }
    if ((now - lastDebounceTimeOn) > debounceDelay) {
        if (debouncedButtonOnState != readingOn) {
            debouncedButtonOnState = readingOn;
            if (debouncedButtonOnState == LOW) {
                // Serial.println("[DEBUG] ON button pressed - gọi callback");
                if (onPress) onPress();
            }
        }
    }
    lastButtonOnState = readingOn;

    // Debounce cho nút OFF
    if (readingOff != lastButtonOffState) {
        // Serial.printf("[DEBUG] OFF state changed: %d -> %d\n", lastButtonOffState, readingOff);
        lastDebounceTimeOff = now;
    }
    if ((now - lastDebounceTimeOff) > debounceDelay) {
        if (debouncedButtonOffState != readingOff) {
            debouncedButtonOffState = readingOff;
            if (debouncedButtonOffState == LOW) {
                // Serial.println("[DEBUG] OFF button pressed - gọi callback");
                if (offPress) offPress();
            }
        }
    }
    lastButtonOffState = readingOff;
}
