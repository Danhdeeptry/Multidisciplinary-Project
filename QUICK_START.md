# 🚀 Smart Farm IoT - Quick Start Guide

## 📋 Prerequisites
- ESP32 board (Yolo UNO or compatible)
- DHT20 temperature/humidity sensor
- Photoresistor/LDR light sensor
- Grow light (LED strip/bulb with relay/MOSFET)
- ThingsBoard account
- PlatformIO IDE

## ⚡ 5-Minute Setup

### 1. Hardware Connections
```
DHT20 Sensor:
- VCC → 3.3V
- GND → GND  
- SDA → GPIO11
- SCL → GPIO12

Light Sensor (LDR):
- One leg → GPIO1
- Other leg → GND
- Add 10kΩ pull-up resistor (GPIO1 to 3.3V)

Grow Light:
- Control signal → GPIO2
- Use relay or MOSFET for high-power loads
- Power supply appropriate for your grow light

Status LED:
- LED positive → GPIO48
- LED negative → GND (through 330Ω resistor)
```

### 2. Software Setup
1. **Clone/Download** this project
2. **Open** in PlatformIO IDE
3. **Update** WiFi credentials in `src/main.cpp`:
   ```cpp
   constexpr char WIFI_SSID[] = "YourWiFi";
   constexpr char WIFI_PASSWORD[] = "YourPassword";
   ```

### 3. ThingsBoard Setup
1. **Create Device** in ThingsBoard
2. **Copy Device Token** 
3. **Update Token** in `src/main.cpp`:
   ```cpp
   constexpr char TOKEN[] = "your_device_token_here";
   ```

### 4. Upload & Run
```bash
platformio run --target upload
platformio device monitor
```

## 📊 ThingsBoard Dashboard Setup

### Quick Dashboard Creation
1. **Import** dashboard configuration from `thingsboard_dashboard_config.json`
2. **Add Widgets**:
   - Latest Values (Temperature, Humidity, Light)
   - Time Series Charts (Trends)
   - Control Switches (Grow Light, Auto Mode)
   - DLI Progress Gauge

### Essential Widgets
```json
{
  "widgets": [
    "Environmental Sensors Display",
    "Grow Light Control Switch", 
    "Auto Mode Toggle",
    "DLI Progress Gauge",
    "Alert Panel",
    "Trend Charts"
  ]
}
```

## 🎮 Testing Your System

### 1. Basic Function Test
```python
# Run the test script
python test_smart_farm.py
```

### 2. Manual Testing
- **Check Serial Monitor**: Verify sensor readings
- **Test RPC Commands**: Use ThingsBoard RPC tab
- **Verify Alerts**: Cover light sensor to trigger alerts
- **Check Automation**: Observe grow light auto-control

### 3. Expected Serial Output
```
Smart Farm IoT system activated with comprehensive features:
- Real-time sensor monitoring (Temperature, Humidity, Light)
- Automatic grow light control with thresholds  
- Daily Light Integral (DLI) calculation
- Alert system for abnormal conditions
- Remote control via ThingsBoard RPC

Sent telemetry - Temperature: 25.30 C, Humidity: 65.20 %, Light: 345.2 lux
Smart Farm - Grow Light: OFF, Auto Mode: AUTO, DLI: 12.450/20.0 (62.3%)
```

## 🔧 Configuration

### Smart Farm Settings (Shared Attributes)
| Parameter | Default | Range | Description |
|-----------|---------|--------|-------------|
| lightThresholdLow | 200.0 | 0-2000 | Grow light ON threshold (lux) |
| lightThresholdHigh | 500.0 | 0-2000 | Grow light OFF threshold (lux) |
| dliTarget | 20.0 | 1-100 | Daily light target (mol/m²/day) |
| autoLightMode | true | true/false | Enable automation |
| alertEnabled | true | true/false | Enable alerts |

### Remote Commands (RPC)
```javascript
// Turn grow light ON
{"method": "setGrowLightState", "params": true}

// Enable auto mode  
{"method": "setAutoLightMode", "params": true}

// Reset DLI counter
{"method": "resetDLI", "params": {}}

// Get all sensor data
{"method": "readSensorData", "params": {}}
```

## 📱 Mobile Dashboard

### Quick Mobile Setup
1. **Install ThingsBoard Mobile App**
2. **Login** with your credentials
3. **Open** Smart Farm dashboard
4. **Pin** to favorites for quick access

### Key Mobile Features
- Real-time sensor monitoring
- One-tap grow light control
- Push notifications for alerts
- Historical data viewing

## 🚨 Troubleshooting

### Common Issues & Fixes

#### ❌ DHT20 Read Errors
```
Solution: Check I2C connections (SDA/SCL)
Verify: 3.3V power supply stable
```

#### ❌ Light Sensor Not Working  
```
Solution: Check GPIO1 connection and pull-up resistor
Verify: ADC readings in serial monitor
```

#### ❌ Grow Light Not Responding
```
Solution: Check GPIO2 connection and relay/MOSFET
Verify: Power supply adequate for grow light
```

#### ❌ ThingsBoard Connection Failed
```
Solution: Verify WiFi credentials and device token
Check: ThingsBoard server URL and port
```

#### ❌ No Telemetry Data
```
Solution: Check internet connection and ThingsBoard status
Verify: Device token is correct and device is active
```

## 📊 Performance Monitoring

### Key Metrics to Watch
- **Telemetry Rate**: Should be consistent 10-second intervals
- **Error Rate**: < 0.1% for stable operation
- **Memory Usage**: Monitor for memory leaks
- **WiFi Signal**: RSSI should be > -70 dBm
- **Sensor Accuracy**: Compare readings with known references

### Health Check Commands
```cpp
// Check WiFi status
Serial.println(WiFi.status());

// Check free memory
Serial.println(ESP.getFreeHeap());

// Check uptime
Serial.println(millis() / 1000);
```

## 🌱 Smart Farm Best Practices

### Optimal Growing Conditions
- **Temperature**: 20-28°C for most plants
- **Humidity**: 50-70% for vegetative growth
- **DLI**: 15-25 mol/m²/day for leafy greens
- **Light Quality**: Full spectrum LED recommended

### Automation Tips
- Set lower threshold during early growth (100-150 lux)
- Increase DLI target for flowering plants (25-35 mol/m²/day)
- Monitor trends for optimal scheduling
- Use alerts to prevent plant stress

## 🎯 Next Steps

### System Expansion
1. **Add More Sensors**: pH, EC, soil moisture
2. **Multiple Zones**: Control different areas independently  
3. **Camera Integration**: Time-lapse plant growth
4. **Weather Integration**: External conditions correlation

### Advanced Features
1. **Machine Learning**: Predictive control algorithms
2. **Mobile App**: Custom native application
3. **Voice Control**: Alexa/Google Assistant integration
4. **Professional Dashboard**: Advanced analytics and reporting

---

**🎉 Congratulations! Your Smart Farm IoT system is now running!**

For detailed documentation, see `SMART_FARM_README.md`  
For complete implementation details, see `IMPLEMENTATION_SUMMARY.md`

**Happy Growing! 🌿🚀**
