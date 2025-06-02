#include "Sensor.h"
#include "MqttClient.h" // For tb object to send telemetry

namespace {
    DHT20 dht20;
    float lastValidTemperature = 0.0f;
    float lastValidHumidity = 0.0f;
    unsigned long lastDHT20Read = 0;
    constexpr unsigned long DHT20_READ_INTERVAL = 1000; // ms
    
    // Alert state tracking
    bool lowTempAlertSent = false;
    bool highTempAlertSent = false;
    bool lowHumidityAlertSent = false;
    bool highHumidityAlertSent = false;
}

// Temperature and humidity thresholds
volatile float tempThresholdMin = 25.0f;     // Default minimum temperature (°C)
volatile float tempThresholdMax = 34.0f;     // Default maximum temperature (°C)
volatile float humidityThresholdMin = 50.0f; // Default minimum humidity (%)
volatile float humidityThresholdMax = 70.0f; // Default maximum humidity (%)

bool initDHT20() {
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);
    delay(100);
    if (!dht20.begin()) return false;
    delay(100);
    return dht20.isConnected();
}

// Chỉ gọi hàm này một lần mỗi chu kỳ telemetry
bool updateDHT20() {
    unsigned long now = millis();
    if (now - lastDHT20Read >= DHT20_READ_INTERVAL) {
        if (dht20.read() == DHT20_OK) {
            float t = dht20.getTemperature();
            float h = dht20.getHumidity();
            if (!isnan(t) && t != 0.0f) lastValidTemperature = t;
            if (!isnan(h) && h != 0.0f) lastValidHumidity = h;
            lastDHT20Read = now;
            return true;
        }
        lastDHT20Read = now;
    }
    return false;
}

float readTemperature() {
    return lastValidTemperature;
}

float readHumidity() {
    return lastValidHumidity;
}

float readLightSensor() {
    int rawValue = analogRead(LIGHT_SENSOR_PIN);
    float lux = rawValue * 0.1f;
    return lux;
}

int readSoilMoisture() {
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    return analogRead(SOIL_MOISTURE_PIN);
}

void checkTempHumidityAlerts(float temperature, float humidity) {
    extern ThingsBoard tb;
    extern volatile bool alertEnabled;
    
    // Only send alerts if alert system is enabled
    if (!alertEnabled) {
        Serial.println("[TEMP/HUMIDITY DEBUG] Alert system disabled");
        return;
    }
    
    Serial.printf("[TEMP/HUMIDITY DEBUG] Checking alerts - Temp: %.1f°C (%.1f-%.1f), Humidity: %.1f%% (%.1f-%.1f)\n", 
                  temperature, tempThresholdMin, tempThresholdMax, 
                  humidity, humidityThresholdMin, humidityThresholdMax);
    
    // Temperature alerts
    if (temperature < tempThresholdMin && !lowTempAlertSent) {
        Serial.println("[ALERT DEBUG] Triggering LOW_TEMPERATURE alert");
        Telemetry alertData[3] = {
            Telemetry("alert_type", "LOW_TEMPERATURE"),
            Telemetry("alert_message", "Temperature too low for optimal plant growth"),
            Telemetry("alert_value", temperature)
        };
        tb.sendTelemetry(alertData, 3);
        lowTempAlertSent = true;
        highTempAlertSent = false; // Reset opposite alert
        Serial.printf("LOW TEMPERATURE ALERT: %.1f°C < %.1f°C threshold\n", temperature, tempThresholdMin);
    }
    else if (temperature > tempThresholdMax && !highTempAlertSent) {
        Serial.println("[ALERT DEBUG] Triggering HIGH_TEMPERATURE alert");
        Telemetry alertData[3] = {
            Telemetry("alert_type", "HIGH_TEMPERATURE"),
            Telemetry("alert_message", "Temperature too high for optimal plant growth"),
            Telemetry("alert_value", temperature)
        };
        tb.sendTelemetry(alertData, 3);
        highTempAlertSent = true;
        lowTempAlertSent = false; // Reset opposite alert
        Serial.printf("HIGH TEMPERATURE ALERT: %.1f°C > %.1f°C threshold\n", temperature, tempThresholdMax);
    }
    // Reset temperature alerts when temperature returns to normal range
    else if (temperature >= tempThresholdMin && temperature <= tempThresholdMax) {
        if (lowTempAlertSent || highTempAlertSent) {
            Serial.println("[ALERT DEBUG] Clearing temperature alerts (NORMAL)");
            lowTempAlertSent = false;
            highTempAlertSent = false;
            Telemetry alertData[2] = {
                Telemetry("alert_type", "TEMP_NORMAL"),
                Telemetry("alert_message", "Temperature returned to normal range")
            };
            tb.sendTelemetry(alertData, 2);
            Serial.println("Temperature alerts cleared - returned to normal");
        }
    }
    
    // Humidity alerts
    if (humidity < humidityThresholdMin && !lowHumidityAlertSent) {
        Serial.println("[ALERT DEBUG] Triggering LOW_HUMIDITY alert");
        Telemetry alertData[3] = {
            Telemetry("alert_type", "LOW_HUMIDITY"),
            Telemetry("alert_message", "Humidity too low for optimal plant growth"),
            Telemetry("alert_value", humidity)
        };
        tb.sendTelemetry(alertData, 3);
        lowHumidityAlertSent = true;
        highHumidityAlertSent = false; // Reset opposite alert
        Serial.printf("LOW HUMIDITY ALERT: %.1f%% < %.1f%% threshold\n", humidity, humidityThresholdMin);
    }
    else if (humidity > humidityThresholdMax && !highHumidityAlertSent) {
        Serial.println("[ALERT DEBUG] Triggering HIGH_HUMIDITY alert");
        Telemetry alertData[3] = {
            Telemetry("alert_type", "HIGH_HUMIDITY"),
            Telemetry("alert_message", "Humidity too high, risk of fungal diseases"),
            Telemetry("alert_value", humidity)
        };
        tb.sendTelemetry(alertData, 3);
        highHumidityAlertSent = true;
        lowHumidityAlertSent = false; // Reset opposite alert
        Serial.printf("HIGH HUMIDITY ALERT: %.1f%% > %.1f%% threshold\n", humidity, humidityThresholdMax);
    }
    // Reset humidity alerts when humidity returns to normal range
    else if (humidity >= humidityThresholdMin && humidity <= humidityThresholdMax) {
        if (lowHumidityAlertSent || highHumidityAlertSent) {
            Serial.println("[ALERT DEBUG] Clearing humidity alerts (NORMAL)");
            lowHumidityAlertSent = false;
            highHumidityAlertSent = false;
            Telemetry alertData[2] = {
                Telemetry("alert_type", "HUMIDITY_NORMAL"),
                Telemetry("alert_message", "Humidity returned to normal range")
            };
            tb.sendTelemetry(alertData, 2);
            Serial.println("Humidity alerts cleared - returned to normal");
        }
    }
}