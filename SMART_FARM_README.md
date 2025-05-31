# Smart Farm IoT System - Feature Documentation

## Overview
This Smart Farm IoT system integrates multiple sensors and automated control features for optimal plant growing conditions. The system is built on ESP32 with ThingsBoard IoT platform integration.

## Hardware Components

### Sensors
- **DHT20**: Temperature and humidity sensor (I2C interface)
- **Photoresistor/LDR**: Light intensity sensor (analog, GPIO1/ADC1_CH0)

### Actuators  
- **Grow Light**: LED grow light controlled via GPIO2
- **Status LED**: System status indicator (GPIO48)

## Smart Farm Features

### 1. Automatic Grow Light Control
- **Threshold-based control**: Automatically turns grow light ON/OFF based on light intensity
- **Hysteresis control**: Prevents frequent switching with separate LOW and HIGH thresholds
- **Manual override**: Can be controlled manually via ThingsBoard dashboard or RPC calls
- **Default thresholds**: 
  - Low: 200 lux (turn ON grow light)
  - High: 500 lux (turn OFF grow light)

### 2. Daily Light Integral (DLI) Calculation
- **Real-time tracking**: Continuously calculates accumulated light exposure
- **Units**: mol/m²/day (standard horticultural measurement)
- **Target setting**: Configurable DLI target (default: 20 mol/m²/day)
- **Progress monitoring**: Shows percentage of daily target achieved
- **Auto reset**: Resets every 24 hours automatically

### 3. Alert System
- **Low light alerts**: Triggered when light drops below threshold
- **High light alerts**: Triggered when light exceeds 2x high threshold
- **Alert clearing**: Automatically clears when conditions return to normal
- **Configurable**: Can be enabled/disabled via shared attributes

### 4. Real-time Telemetry
- **Environmental data**: Temperature, humidity, light intensity
- **System status**: Grow light state, auto mode status
- **Smart Farm metrics**: DLI progress, day elapsed time
- **High-frequency updates**: 10-second intervals for responsive monitoring

### 5. Remote Control (RPC Methods)
- `setGrowLightState(boolean)`: Manual grow light control
- `setAutoLightMode(boolean)`: Enable/disable automatic control
- `resetDLI()`: Reset Daily Light Integral counter
- `readSensorData()`: Get comprehensive sensor readings
- `setLedSwitchValue(boolean)`: Control status LED

### 6. Configurable Parameters (Shared Attributes)
- `lightThresholdLow`: Low light threshold (lux)
- `lightThresholdHigh`: High light threshold (lux)
- `dliTarget`: Target Daily Light Integral (mol/m²/day)
- `autoLightMode`: Enable automatic light control
- `alertEnabled`: Enable alert system
- `growLightState`: Current grow light state

## Pin Configuration
```cpp
#define LED_PIN 48              // Status LED
#define SDA_PIN GPIO_NUM_11     // DHT20 I2C Data
#define SCL_PIN GPIO_NUM_12     // DHT20 I2C Clock
#define LIGHT_SENSOR_PIN 1      // Photoresistor (ADC1_CH0)
#define GROW_LIGHT_PIN 2        // Grow light control
```

## Telemetry Data Points
| Data Point | Unit | Description |
|------------|------|-------------|
| temperature | °C | Environment temperature |
| humidity | % | Relative humidity |
| light_intensity | lux | Current light level |
| grow_light_active | boolean | Grow light status |
| auto_mode_active | boolean | Auto control status |
| daily_light_integral | mol/m²/day | Accumulated DLI |
| dli_progress_percent | % | Progress toward target |
| day_elapsed_hours | hours | Hours since day start |

## Alert Types
- **LOW_LIGHT**: Light intensity below threshold
- **HIGH_LIGHT**: Light intensity excessively high
- **NORMAL**: Conditions returned to normal range

## ThingsBoard Dashboard Setup

### Required Widgets
1. **Latest Values**: Environmental sensors display
2. **Time Series Charts**: Temperature, humidity, and light trends
3. **Control Switches**: Grow light and auto mode control
4. **Digital Gauge**: DLI progress visualization
5. **State Indicators**: System status display
6. **Alarm Panel**: Alert management

### Configuration Steps
1. Import dashboard configuration from `thingsboard_dashboard_config.json`
2. Create device with token matching `main.cpp`
3. Configure shared attributes with desired default values
4. Set up alarm rules for automated notifications
5. Test RPC methods functionality

## Installation and Setup

### 1. Hardware Connections
- Connect DHT20 to I2C pins (SDA: GPIO11, SCL: GPIO12)
- Connect photoresistor to GPIO1 with pull-down resistor
- Connect grow light relay/MOSFET to GPIO2
- Ensure proper power supply for all components

### 2. Software Configuration
- Update WiFi credentials in `main.cpp`
- Set ThingsBoard server and device token
- Adjust sensor calibration if needed
- Configure default thresholds and targets

### 3. ThingsBoard Setup
- Create device and copy access token
- Import dashboard configuration
- Configure shared attributes
- Set up alarm rules and notifications

## Usage Examples

### Automatic Operation
The system automatically:
- Monitors environmental conditions
- Controls grow light based on light levels
- Tracks daily light accumulation
- Sends alerts for abnormal conditions
- Updates dashboard in real-time

### Manual Control
Via ThingsBoard dashboard:
- Toggle grow light manually
- Switch between auto/manual modes
- Reset DLI counter
- Adjust thresholds and targets
- View historical data and trends

## Troubleshooting

### Common Issues
1. **DHT20 read errors**: Check I2C connections and timing
2. **Light sensor readings**: Verify ADC pin configuration
3. **Grow light not responding**: Check GPIO2 connections
4. **ThingsBoard connectivity**: Verify WiFi and token settings

### Debug Information
- Enable serial monitoring for detailed logs
- Check WiFi connection status
- Verify ThingsBoard connection
- Monitor sensor initialization status

## Advanced Features (Future Enhancements)

### Planned Additions
- **Machine Learning**: Pattern recognition for optimal growing conditions
- **Multi-sensor Arrays**: Support for multiple light sensors
- **Environmental Control**: Temperature and humidity regulation
- **Nutrient Monitoring**: pH and EC sensor integration
- **Weather Integration**: External weather data correlation
- **Plant Growth Tracking**: Time-lapse and measurement features

### API Extensions
- RESTful API for external integrations
- MQTT topic customization
- Data export capabilities
- Historical analysis tools

## Performance Specifications

### Real-time Performance
- **Telemetry interval**: 10 seconds (configurable)
- **Sensor reading rate**: 1 Hz maximum
- **Response time**: < 1 second for manual controls
- **Data accuracy**: ±0.5°C temperature, ±3% humidity, ±10% light

### System Reliability
- **WiFi reconnection**: Automatic with retry logic
- **Sensor error handling**: Graceful degradation
- **Memory management**: Optimized for 24/7 operation
- **Power consumption**: < 500mA average

## Support and Maintenance

### Regular Maintenance
- Clean light sensor surface monthly
- Calibrate sensors quarterly
- Update firmware as needed
- Monitor system performance metrics

### Support Resources
- Hardware documentation in `/boards/`
- Software examples in `/simulator/`
- Database tools in `/simple_server+backend/`
- Test utilities in `/test/`

---

**Version**: 1.0  
**Last Updated**: May 31, 2025  
**Compatible with**: ESP32, ThingsBoard CE/PE  
**License**: MIT
