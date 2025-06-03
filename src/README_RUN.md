# Embedded Firmware Usage Guide

This document provides instructions for building, uploading, and using the embedded firmware for the Yolo UNO Smart Farm IoT system.

---

## 1. Directory Structure

- **src/**: Main source code (.cpp) for firmware modules (sensors, control, MQTT, etc.)
- **include/**: Header files (.h) for firmware modules
- **lib/**: Third-party and custom libraries (ArduinoJson, DHT20, ThingsBoard, etc.)
- **platformio.ini**: PlatformIO project configuration

---

## 2. Prerequisites

- [PlatformIO](https://platformio.org/) installed (VSCode extension recommended)
- Supported board: Yolo UNO (ESP32-based)
- USB cable for flashing firmware

---

## 3. Building & Uploading Firmware

### Using PlatformIO (VSCode)
1. Open the project root folder in VSCode.
2. Ensure PlatformIO extension is installed.
3. Connect the Yolo UNO board via USB.
4. Select the correct serial port if needed.
5. Click the "Build" (checkmark) button to compile.
6. Click the "Upload" (right arrow) button to flash firmware to the board.

### Using PlatformIO CLI
```sh
# Build firmware
platformio run
# Upload firmware
platformio run --target upload
```

---

## 4. Main Features
- Reads temperature, humidity (DHT20), light, and soil moisture sensors
- Controls grow light (auto/manual mode)
- Handles button input with debounce
- Sends telemetry to ThingsBoard via MQTT
- Receives remote commands (RPC) and attribute updates
- FreeRTOS-based multitasking for robust operation

---

## 5. Customization
- **Pin configuration:** Edit `include/Utils.h` or `config.h` for pin assignments
- **Sensor logic:** Edit `src/Sensor.cpp` and `include/Sensor.h`
- **Control logic:** Edit `src/LightControl.cpp` and `include/LightControl.h`
- **MQTT/ThingsBoard:** Edit `src/MqttClient.cpp` and `include/MqttClient.h`

---

## 6. Troubleshooting
- Check serial monitor (115200 baud) for debug output
- Ensure correct board and port are selected in PlatformIO
- If build fails, check library dependencies in `lib/` and `platformio.ini`

---

## 7. References
- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [ThingsBoard Docs](https://thingsboard.io/docs/)

---

For further details, see the technical reports in `docs/` and code comments in each module.
