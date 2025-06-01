# 🌱 Smart Farm IoT - DEMO Mode Configuration

## 🚀 Demo Mode Features

### ⚡ **Accelerated DLI Calculation**
- **Normal Mode**: 24-hour day cycles
- **Demo Mode**: Normal 24-hour day cycles with 10x faster DLI accumulation
- **Acceleration Factor**: 10x speed for reasonable observation
- **Reset Frequency**: Normal 24-hour cycles (not shortened)

### 📊 **Quick Observation Benefits**
- Watch DLI accumulate 10x faster than normal
- Maintain realistic day cycles (24 hours)
- Perfect for demos and development
- Reach DLI targets in ~2.4 hours instead of 24 hours

## 🔧 **Technical Implementation**

### DLI Calculation Changes
```cpp
// DEMO MODE CONSTANTS
const float DEMO_ACCELERATION_FACTOR = 10.0;   // 10x faster accumulation

// Accelerated time calculation
float acceleratedTimeDiff = timeDiff * DEMO_ACCELERATION_FACTOR;

// Normal 24-hour reset cycle maintained
if (dayStartTime == 0 || (currentTime - dayStartTime) >= 86400000) { // 24 hours
    dayStartTime = currentTime;
    totalDLI = 0.0;
}
```

### Telemetry Updates
- Added `demo_mode: true` flag
- Normal day elapsed hours (0-24 hours)
- Enhanced logging with "[10x faster]" indicators
- DLI progress percentage calculation

## 📈 **Expected Behavior**

### Normal Light Conditions (300-400 lux)
- **Real Mode**: DLI reaches ~15-20 mol/m²/day in 24 hours
- **Demo Mode**: DLI reaches ~15-20 mol/m²/day in 10 minutes

### Demo Cycle Timeline
```
0-2 min:   DLI: 0-5 mol/m²/day    (Early morning simulation)
2-5 min:   DLI: 5-15 mol/m²/day   (Morning-midday simulation)
5-8 min:   DLI: 15-20 mol/m²/day  (Peak day simulation)
8-10 min:  DLI: 20+ mol/m²/day    (Late day simulation)
10 min:    Reset to 0             (New day starts)
```

## 🎮 **Demo Benefits**

### For Presentations
- ✅ Show complete day cycle in 10 minutes
- ✅ Demonstrate grow light automation quickly
- ✅ Display DLI progress in real-time
- ✅ Test alert system immediately

### For Development
- ✅ Rapid testing of thresholds
- ✅ Quick validation of automation logic
- ✅ Fast cycle testing
- ✅ Immediate feedback on changes

## 🔄 **Switching Back to Normal Mode**

To return to real 24-hour mode, modify these values in `main.cpp`:

```cpp
// Change these back to normal values:
const unsigned long DEMO_DAY_DURATION = 86400000; // 24 hours
const float DEMO_ACCELERATION_FACTOR = 1.0;       // No acceleration
```

## 🌟 **Demo Mode Indicators**

### Serial Monitor
```
🌱 Smart Farm IoT DEMO system activated
- DEMO MODE: 10-minute day cycles instead of 24-hour
DEMO: New day cycle started - DLI reset to 0 (10-minute cycle)
Smart Farm DEMO - DLI: 12.450/20.0 (62.3%) [144x Speed]
```

### ThingsBoard Telemetry
```json
{
  "daily_light_integral": 12.45,
  "demo_mode": true,
  "demo_cycle_minutes": 10,
  "day_elapsed_hours": 0.5
}
```

---

**Perfect for demos, presentations, and rapid development! 🚀**

**Created**: June 1, 2025  
**Version**: Demo Mode v1.0  
**Compatibility**: ESP32 Smart Farm IoT System
