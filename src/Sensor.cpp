#include "Sensor.h"

namespace {
    DHT20 dht20;
    float lastValidTemperature = 0.0f;
    float lastValidHumidity = 0.0f;
    unsigned long lastDHT20Read = 0;
    constexpr unsigned long DHT20_READ_INTERVAL = 1000; // ms
}

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