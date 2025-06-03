# SMART FARM - SEMESTER 242

---

# Multidisciplinary Project Architecture Overview

## 1. Project Structure

```
/ai_model/              # AI/ML model training, inference, and test scripts
/backend/               # Python backend and coreiot server, database, and API
/boards/                # Board configuration files (e.g., yolo_uno.json)
/data/                  # Sample data, sensor database, etc.
/docs/                  # Documentation, technical reports, architecture
/frontend/              # Web frontend (React/Vite), static assets
/include/               # C/C++ header files for embedded firmware
/lib/                   # Third-party and custom libraries for firmware
/simulator/             # Device and GUI simulators for ESP32
/src/                   # Embedded firmware source code (C/C++)
platformio.ini          # PlatformIO project config for embedded build
Pipfile, Pipfile.lock   # Python dependency management for backend/AI
```

## 2. Module Responsibilities

- **ai_model/**: Training, testing, and deploying plant disease AI models (Python, Keras, Jupyter Notebooks).
- **backend/**: Python server for API, database (SQLite), and business logic. Handles communication between frontend and IoT devices.
- **frontend/**: Web dashboard for monitoring and control (React, Vite, JS/TS).
- **src/**: Main embedded firmware for Yolo UNO (ESP32), including sensor reading, device control, and MQTT/ThingsBoard communication.
- **include/**: Header files for firmware modules (Button, Sensor, Light, MQTT, etc.).
- **lib/**: External and custom libraries (ArduinoJson, DHT20, ThingsBoard, etc.).
- **boards/**: Board configuration files for PlatformIO or hardware abstraction.
- **simulator/**: Scripts to simulate device behavior and test connectivity without real hardware.
- **data/**: Sensor data, database files, and other datasets.
- **docs/**: Documentation, technical reports, and architecture overview.

## 3. Data & Communication Flow

1. **Sensors (firmware/src)** read environment data (temperature, humidity, light, soil moisture).
2. **Firmware** sends telemetry via MQTT to ThingsBoard and/or backend server.
3. **Backend** stores data, provides API for frontend, and can relay commands to devices.
4. **Frontend** visualizes data, allows user control (manual/auto mode, grow light, etc.).
5. **AI Model** (optional) can be triggered for plant disease detection, results shown on frontend.

## 4. Build & Deployment

- **Firmware:** Built with PlatformIO, source in /src, headers in /include, libraries in /lib.
- **Backend:** Python 3.x, dependencies in requirements.txt or Pipfile.
- **Frontend:** Node.js, Vite, React; dependencies in package.json.
- **AI Model:** Python, Keras, Jupyter; run notebooks or scripts in /ai_model.

## 5. Extensibility

- Add new sensors/actuators: Implement in /src and /include, update backend/frontend as needed.
- Add new AI models: Place in /ai_model, integrate with backend/frontend.
- Add new API endpoints: Implement in /backend, document in /docs.

## 6. Documentation

- **docs/README.md**: General documentation and usage.
- **docs/SmartFarmIoT_TechnicalReport.md**: Technical report (Vietnamese).
- **docs/SmartFarmIoT_TechnicalReport_EN.md**: Technical report (English).
- **docs/ARCHITECTURE.md**: (This file) Project structure and architecture overview.

---

For more details, see README files in each module and technical reports in /docs.
