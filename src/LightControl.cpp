#include "LightControl.h"
#include "MqttClient.h" // Để dùng tb object

// Biến trạng thái và cấu hình
volatile bool growLightState = false;
volatile bool autoLightMode = true;
volatile float lightThresholdLow = 200.0f;
volatile float lightThresholdHigh = 400.0f;
volatile float dliTarget = 20.0f;
volatile bool alertEnabled = true;
volatile bool attributesChanged = false;

// Biến tracking DLI và alert
float totalDLI = 0.0f;
static unsigned long lastDLIUpdate = 0;
unsigned long dayStartTime = 0;
static bool lowLightAlertSent = false;
static bool highLightAlertSent = false;

void initLightControl() {
    pinMode(GROW_LIGHT_PIN, OUTPUT);
    digitalWrite(GROW_LIGHT_PIN, growLightState);
    dayStartTime = millis();
    lastDLIUpdate = millis();
    totalDLI = 0.0f;
    lowLightAlertSent = false;
    highLightAlertSent = false;
}

void controlGrowLight(float lightIntensity) {
    Serial.printf("DEBUG: controlGrowLight called - Light: %.1f lux, AutoMode: %s, Current State: %s\n", 
                  lightIntensity, autoLightMode ? "ON" : "OFF", growLightState ? "ON" : "OFF");
    
    if (!autoLightMode) {
        Serial.println("DEBUG: Auto mode disabled, skipping control");
        return;  // Manual mode - don't auto control
    }
    
    bool shouldTurnOn = false;
    
    // Hysteresis control to prevent frequent switching
    if (!growLightState && lightIntensity < lightThresholdLow) {
        shouldTurnOn = true;
        Serial.printf("DEBUG: Should turn ON - Light %.1f < threshold %.1f\n", lightIntensity, lightThresholdLow);
    } else if (growLightState && lightIntensity > lightThresholdHigh) {
        shouldTurnOn = false;
        Serial.printf("DEBUG: Should turn OFF - Light %.1f > threshold %.1f\n", lightIntensity, lightThresholdHigh);
    } else {
        shouldTurnOn = growLightState;  // Keep current state
        Serial.printf("DEBUG: Keep current state - Light %.1f in hysteresis range\n", lightIntensity);
    }
    
    if (shouldTurnOn != growLightState) {
        growLightState = shouldTurnOn;
        digitalWrite(GROW_LIGHT_PIN, growLightState);
        
        Serial.printf("Grow light %s (Auto mode) - Light: %.1f lux\n", 
                      growLightState ? "ON" : "OFF", lightIntensity);
        
        // Send state update to ThingsBoard
        attributesChanged = true;
    } else {
        Serial.printf("DEBUG: No state change needed - shouldTurnOn: %s, current: %s\n", 
                      shouldTurnOn ? "ON" : "OFF", growLightState ? "ON" : "OFF");
    }
}

void updateDLI(float lightIntensity) {
    unsigned long currentTime = millis();
    
    // Keep normal 24-hour reset cycle but accelerate accumulation
    if (dayStartTime == 0 || (currentTime - dayStartTime) >= 86400000) {  // 24 hours in ms
        dayStartTime = currentTime;
        totalDLI = 0.0f;
        Serial.println("New day started - DLI reset to 0 (normal 24-hour cycle)");
    }
    
    if (lastDLIUpdate != 0) {
        // Convert lux to PPFD (approximate: 1 lux ≈ 0.0185 µmol/m²/s for sunlight)
        float ppfd = lightIntensity * 0.0185f;  // µmol/m²/s
        
        // Calculate time difference in seconds
        float timeDiff = (currentTime - lastDLIUpdate) / 1000.0f;
        
        // DEMO MODE: Apply moderate acceleration factor for easier observation
        const float DEMO_ACCELERATION_FACTOR = 10.0f;  // 10x faster than normal (reasonable for demo)
        float acceleratedTimeDiff = timeDiff * DEMO_ACCELERATION_FACTOR;
        
        // Add to DLI: PPFD * accelerated_time * conversion factor
        // 1 mol/m²/day = 1,000,000 µmol/m²/s * 86400 s = 86.4 billion µmol/m²/day
        float dliIncrement = (ppfd * acceleratedTimeDiff) / 1000000.0f;  // Convert µmol to mol
        totalDLI += dliIncrement;
        
        // Debug info for demo monitoring (less frequent)
        static unsigned long lastDebugTime = 0;
        if (currentTime - lastDebugTime > 60000) {  // Debug every 60 seconds
            lastDebugTime = currentTime;
            Serial.printf("DLI Progress: %.3f mol/m²/day (%.1fx faster, %.1f%% of target)\n", 
                          totalDLI, DEMO_ACCELERATION_FACTOR, (totalDLI/dliTarget)*100.0);
        }
    }
    
    lastDLIUpdate = currentTime;
}

void checkLightAlerts(float lightIntensity) {
  // Serial debug chi tiết
  Serial.printf("[ALERT DEBUG] lightIntensity=%.1f, low=%.1f, high=%.1f, lowLightAlertSent=%s, highLightAlertSent=%s\n",
    lightIntensity, lightThresholdLow, lightThresholdHigh,
    lowLightAlertSent ? "true" : "false", highLightAlertSent ? "true" : "false");
  if (!alertEnabled) {
    Serial.println("[ALERT DEBUG] Alert system is DISABLED");
    return;
  }
  // Low light alert
  if (lightIntensity < lightThresholdLow && !lowLightAlertSent) {
    Serial.println("[ALERT DEBUG] Triggering LOW_LIGHT alert");
    Telemetry alertData[3] = {
      Telemetry("alert_type", "LOW_LIGHT"),
      Telemetry("alert_message", "Light intensity below threshold"),
      Telemetry("alert_value", lightIntensity)
    };
    tb.sendTelemetry(alertData, 3);
    lowLightAlertSent = true;
    highLightAlertSent = false; // Reset opposite alert
    Serial.printf("LOW LIGHT ALERT: %.1f lux < %.1f lux threshold\n", lightIntensity, lightThresholdLow);
  }
  // High light alert (excessive light can damage plants)
  else if (lightIntensity > lightThresholdHigh && !highLightAlertSent) {
    Serial.println("[ALERT DEBUG] Triggering HIGH_LIGHT alert");
    Telemetry alertData[3] = {
      Telemetry("alert_type", "HIGH_LIGHT"),
      Telemetry("alert_message", "Light intensity excessively high"),
      Telemetry("alert_value", lightIntensity)
    };
    tb.sendTelemetry(alertData, 3);
    highLightAlertSent = true;
    lowLightAlertSent = false; // Reset opposite alert
    Serial.printf("HIGH LIGHT ALERT: %.1f lux > %.1f lux threshold\n", lightIntensity, lightThresholdHigh);
  }
  // Reset alerts when light returns to normal range
  else if (lightIntensity >= lightThresholdLow && lightIntensity <= lightThresholdHigh * 2) {
    if (lowLightAlertSent || highLightAlertSent) {
      Serial.println("[ALERT DEBUG] Clearing alerts (NORMAL)");
      lowLightAlertSent = false;
      highLightAlertSent = false;
      Telemetry alertData[2] = {
        Telemetry("alert_type", "NORMAL"),
        Telemetry("alert_message", "Light intensity returned to normal")
      };
      tb.sendTelemetry(alertData, 2);
      Serial.println("Light alerts cleared - intensity normal");
    } else {
      Serial.println("[ALERT DEBUG] No alert to clear (NORMAL range)");
    }
  } else {
    Serial.println("[ALERT DEBUG] No alert condition met");
  }
}

float getTotalDLI() { return totalDLI; }

float getDLIProgressPercent() {
    if (dliTarget <= 0.0f) return 0.0f;
    return (totalDLI / dliTarget) * 100.0f;
}