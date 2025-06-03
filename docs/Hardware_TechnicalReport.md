# TECHNICAL REPORT: SMART FARM IOT SYSTEM (YOLO UNO)

---

## 1. Hardware Architecture Overview

### Main Components
- **MCU:** Yolo UNO (ESP32-based board)
- **Sensors:**
  - DHT20 (I²C) – temperature and humidity
  - Light sensor (analog)
  - Grove soil moisture sensor (analog, D7)
- **Actuators:**
  - Grow light (relay/mosfet, controlled via GPIO)
  - Status LED (GPIO)
  - Buttons (Button 1, Button 2 – digital input)
- **Connectivity:** WiFi (ESP32), MQTT (ThingsBoard)
- **Power:** 5V/3.3V (depending on board)

### Wiring Diagram (description)
```
+-------------------+         +-------------------+
|    Yolo UNO       |         |   Sensor/Actuator |
+-------------------+         +-------------------+
| D7  <--- Soil Moisture (Grove, analog)         |
| I2C (SDA/SCL) <--- DHT20 (Temp/Humidity)       |
| A0  <--- Light Sensor (analog)                 |
| D2  <--- Button 1 (digital input)              |
| D3  <--- Button 2 (digital input)              |
| D4  ---> Grow Light Relay/Mosfet (digital out) |
| D5  ---> LED Status (digital out)              |
+------------------------------------------------+
```
- **Power:** 5V supplied to board, sensors use 3.3V/5V as required.

---

## 2. User & Hardware Functional Requirements

- Continuous monitoring: temperature, humidity, light, soil moisture.
- Smart grow light control (auto/manual via button or ThingsBoard).
- Switch auto/manual mode by button or remotely.
- Send complete telemetry to ThingsBoard, ensure realtime and reliability.
- Button debounce, noise immunity.
- Electrical safety (relay/mosfet isolation).
- Power optimization (if possible).
- Cost-effective (common sensors, low-cost ESP32 board).

---

## 3. Detailed Module/Code Analysis

### 3.1 src/ButtonControl.cpp & include/ButtonControl.h

#### Function
- Button handling (debounce, callback).
- Button 1: Toggle grow light.
- Button 2: Switch auto/manual mode.

#### Pin configuration
- Button 1: D2 (INPUT_PULLUP)
- Button 2: D3 (INPUT_PULLUP)

#### Debounce
- Timer-based, check stable state before triggering event.

#### Example
```cpp
void IRAM_ATTR handleButton1Interrupt() {
    // ...debounce logic...
    if (stable) callback();
}
```

---

### 3.2 src/Sensor.cpp & include/Sensor.h

#### Function
- Read sensors: temperature, humidity (DHT20), light (analog), soil moisture (analog).

#### Pin configuration
- DHT20: I²C (SDA/SCL)
- Light Sensor: A0 (ADC)
- Soil Moisture: D7 (ADC)

#### Protocols
- DHT20: I²C (init, read)
- Analog sensors: ESP32 ADC

#### Example
```cpp
float readSoilMoisture() {
    int value = analogRead(SOIL_MOISTURE_PIN); // D7
    return value;
}
```

---

### 3.3 src/MqttClient.cpp & include/MqttClient.h

#### Function
- WiFi, MQTT, ThingsBoard connection.
- Telemetry sending, RPC handling, shared attributes.
- FreeRTOS tasks: reconnect WiFi, connect ThingsBoard, send telemetry, send attributes, TbLoop.

#### Pin configuration
- No direct pin config, only logic control.

#### Protocols
- MQTT (ThingsBoard)
- WiFi (ESP32)

#### Timing
- Periodic telemetry (TELEMETRY_SEND_INTERVAL)
- Send attributes on change

#### Example
```cpp
telemetryDoc["soil_moisture"] = soilMoisture;
tb.sendTelemetryJson(telemetryJson.c_str());
```

---

### 3.4 src/LightControl.cpp & include/LightControl.h

#### Function
- Grow light control (auto/manual, light threshold).

#### Pin configuration
- GROW_LIGHT_PIN: D4 (OUTPUT)

#### Example
```cpp
void controlGrowLight(float lightIntensity) {
    if (autoMode && lightIntensity < threshold) {
        digitalWrite(GROW_LIGHT_PIN, HIGH);
    }
}
```

---

### 3.5 src/main.cpp

#### Function
- System init, pin setup, FreeRTOS task creation.
- Sensor, WiFi, MQTT init.

#### Pin configuration
- pinMode for sensors, buttons, relay, LED.

#### Example
```cpp
pinMode(SOIL_MOISTURE_PIN, INPUT);
xTaskCreate(sendTelemetryTask, ...);
```

---

## 4. Technical Specifications & Features

| Component         | Key Specs                                    |
|-------------------|----------------------------------------------|
| MCU (Yolo UNO)    | ESP32, 240MHz, 2 core, WiFi, 12-bit ADC      |
| DHT20             | I²C, 3.3V/5V, ±0.3°C, ±3%RH, 0.5Hz           |
| Soil Moisture     | Analog, 3.3V/5V, ADC range 0-4095 (12-bit)   |
| Light Sensor      | Analog, 3.3V/5V, ADC range 0-4095            |
| Grow Light        | Relay/Mosfet, digital control                |
| Button            | Digital input, software debounce             |
| WiFi              | 2.4GHz, 802.11 b/g/n                         |
| Power             | 5V/3.3V, total current < 500mA               |

---

## 5. Strengths – Weaknesses & Trade-offs

### 5.1 Sensor Communication
- **I²C (DHT20):**
  - Pros: Simple, multiple sensors per bus.
  - Cons: Moderate speed, susceptible to noise if long wires.
- **Analog (Soil, Light):**
  - Pros: Simple, no complex protocol.
  - Cons: Accuracy depends on ADC, noise-prone.

### 5.2 Actuator Control
- **Relay/Mosfet digital:**
  - Pros: Simple, safe if isolated.
  - Cons: No PWM/dimming (if needed).

### 5.3 Button Handling
- **Software debounce:**
  - Pros: Flexible, no extra hardware.
  - Cons: If not optimized, may miss events.

### 5.4 Data Transmission
- **MQTT/ThingsBoard:**
  - Pros: Industrial standard, scalable.
  - Cons: WiFi dependent, may lose connection.

---

## 6. Optimization Recommendations & Future Directions

- **ADC optimization:**
  - Use averaging to reduce noise:
    ```cpp
    int readSoilMoistureAvg() {
        int sum = 0;
        for (int i = 0; i < 10; ++i) sum += analogRead(SOIL_MOISTURE_PIN);
        return sum / 10;
    }
    ```
- **Enable CRC for DHT20:**
  - If supported by library, enable CRC for reliability.
- **Power optimization:**
  - Turn off WiFi when idle, use deep sleep if not realtime.
- **Relay/mosfet protection:**
  - Add opto-isolation, fuse.
- **Module expansion:**
  - Add pH, EC sensors, AI camera for disease detection.
- **Telemetry optimization:**
  - Send only on significant data change to save bandwidth.

---

## 7. Conclusion

The Smart Farm IoT system using Yolo UNO (ESP32) integrates full environmental sensors, smart grow light control, and ThingsBoard connectivity via MQTT. The codebase is well-organized, extensible, and meets requirements for measurement, control, and remote monitoring. The current solution is suitable for small to medium smart agriculture models, and can be expanded with more sensors, energy optimization, or AI-based disease detection in the future. This report summarizes, evaluates, and guides the system for maintenance, expansion, and technology transfer.
