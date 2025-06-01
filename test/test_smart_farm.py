"""
Smart Farm IoT System Test Script
Tests all major functionality including sensors, automation, and ThingsBoard integration
"""

import json
import time
import random
import requests
from datetime import datetime, timedelta

class SmartFarmTester:
    def __init__(self, thingsboard_url, device_token):
        self.tb_url = thingsboard_url
        self.token = device_token
        self.headers = {'Content-Type': 'application/json'}
        
    def simulate_sensor_data(self):
        """Simulate realistic sensor readings"""
        # Simulate daily light cycle (0-1000 lux)
        hour = datetime.now().hour
        if 6 <= hour <= 18:  # Daytime
            base_light = 400 + 200 * abs(12 - hour) / 6  # Peak at noon
        else:  # Nighttime
            base_light = 10 + random.uniform(0, 20)
        
        light_intensity = base_light + random.uniform(-50, 50)
        light_intensity = max(0, min(1000, light_intensity))
        
        # Simulate temperature (20-35°C)
        temperature = 25 + 5 * (light_intensity / 500) + random.uniform(-2, 2)
        
        # Simulate humidity (40-80%)
        humidity = 60 - (temperature - 25) * 2 + random.uniform(-5, 5)
        humidity = max(30, min(90, humidity))
        
        return {
            "temperature": round(temperature, 2),
            "humidity": round(humidity, 2),
            "light_intensity": round(light_intensity, 1)
        }
    
    def calculate_grow_light_state(self, light_intensity, current_state=False, 
                                 low_threshold=200, high_threshold=500):
        """Simulate grow light logic"""
        if not current_state and light_intensity < low_threshold:
            return True
        elif current_state and light_intensity > high_threshold:
            return False
        return current_state
    
    def test_connection(self):
        """Test basic connectivity to ThingsBoard"""
        print("🔌 Testing ThingsBoard connectivity...")
        
        # Test different possible endpoints
        test_urls = [
            f"{self.tb_url}/api/v1/{self.token}/telemetry",
            f"{self.tb_url.replace(':8080', '')}/api/v1/{self.token}/telemetry",  # Try without port
            f"{self.tb_url.replace('http://', 'https://')}/api/v1/{self.token}/telemetry",  # Try HTTPS
        ]
        
        for url in test_urls:
            try:
                print(f"   Trying: {url}")
                response = requests.post(url, json={"test": True}, 
                                       headers=self.headers, timeout=5)
                
                print(f"   Status: {response.status_code}")
                if response.status_code in [200, 201]:
                    print(f"✅ Connection successful to: {url}")
                    # Update the working URL
                    self.tb_url = url.replace('/api/v1/' + self.token + '/telemetry', '')
                    return True
                elif response.status_code == 401:
                    print("   ⚠️  Unauthorized - check device token")
                elif response.status_code == 404:
                    print("   ⚠️  Device not found")
                else:
                    print(f"   ⚠️  Unexpected status code: {response.status_code}")
                    
            except requests.exceptions.ConnectTimeout:
                print("   ❌ Connection timeout")
            except requests.exceptions.ConnectionError as e:
                print(f"   ❌ Connection error: {e}")
            except Exception as e:
                print(f"   ❌ Error: {e}")
                
        print("❌ Could not establish connection to ThingsBoard")
        return False
    
    def test_rpc_methods(self):
        """Test all RPC methods"""
        rpc_tests = [
            {"method": "readSensorData", "params": {}},
            {"method": "setGrowLightState", "params": True},
            {"method": "setAutoLightMode", "params": False},
            {"method": "resetDLI", "params": {}},
            {"method": "setGrowLightState", "params": False},
            {"method": "setAutoLightMode", "params": True},
        ]
        
        print("🧪 Testing RPC Methods...")
        for test in rpc_tests:
            try:
                url = f"{self.tb_url}/api/v1/{self.token}/rpc"
                response = requests.post(url, json=test, headers=self.headers, timeout=10)
                
                if response.status_code == 200:
                    print(f"✅ {test['method']}: SUCCESS")
                    if test['method'] == 'readSensorData':
                        data = response.json()
                        print(f"   📊 Sensor Data: {data}")
                else:
                    print(f"❌ {test['method']}: FAILED ({response.status_code})")
                    
            except Exception as e:
                print(f"❌ {test['method']}: ERROR - {e}")
            
            time.sleep(1)
    
    def test_shared_attributes(self):
        """Test shared attributes configuration"""
        attributes = {
            "lightThresholdLow": 180.0,
            "lightThresholdHigh": 520.0,
            "dliTarget": 25.0,
            "autoLightMode": True,
            "alertEnabled": True
        }
        
        print("🔧 Testing Shared Attributes...")
        try:
            url = f"{self.tb_url}/api/v1/{self.token}/attributes"
            response = requests.post(url, json=attributes, headers=self.headers, timeout=10)
            
            if response.status_code == 200:
                print("✅ Shared Attributes: SUCCESS")
                for key, value in attributes.items():
                    print(f"   🔹 {key}: {value}")
            else:
                print(f"❌ Shared Attributes: FAILED ({response.status_code})")
                
        except Exception as e:
            print(f"❌ Shared Attributes: ERROR - {e}")
    
    def simulate_daily_cycle(self, duration_minutes=5):
        """Simulate a full day cycle in compressed time"""
        print(f"🌅 Simulating {duration_minutes}-minute daily cycle...")
        
        grow_light_state = False
        total_dli = 0.0
        light_threshold_low = 200
        light_threshold_high = 500
        
        start_time = time.time()
        cycle_start = time.time()
        
        while (time.time() - start_time) < (duration_minutes * 60):
            # Calculate time position in 24-hour cycle
            elapsed = time.time() - cycle_start
            cycle_position = (elapsed / (duration_minutes * 60)) * 24  # 0-24 hours
            
            # Generate realistic light pattern
            if 6 <= cycle_position <= 18:  # Daytime
                base_light = 600 + 300 * abs(12 - cycle_position) / 6
            else:  # Nighttime  
                base_light = 5 + random.uniform(0, 15)
            
            light_intensity = base_light + random.uniform(-100, 100)
            light_intensity = max(0, min(1000, light_intensity))
            
            # Calculate grow light state
            grow_light_state = self.calculate_grow_light_state(
                light_intensity, grow_light_state, 
                light_threshold_low, light_threshold_high
            )
            
            # Calculate DLI increment
            ppfd = light_intensity * 0.0185  # Convert lux to PPFD
            dli_increment = (ppfd * 10) / 1000000.0  # 10 second intervals
            total_dli += dli_increment
            
            # Generate sensor data
            sensor_data = self.simulate_sensor_data()
            sensor_data['light_intensity'] = light_intensity
            
            # Add Smart Farm data
            telemetry_data = {
                **sensor_data,
                "grow_light_active": grow_light_state,
                "auto_mode_active": True,
                "daily_light_integral": round(total_dli, 3),
                "dli_progress_percent": round((total_dli / 20.0) * 100, 1),
                "day_elapsed_hours": round(cycle_position, 2)
            }
            
            # Send telemetry
            self.send_telemetry(telemetry_data)
            
            # Check for alerts
            if light_intensity < light_threshold_low:
                self.send_alert("LOW_LIGHT", f"Light intensity {light_intensity} lux below threshold")
            elif light_intensity > (light_threshold_high * 2):
                self.send_alert("HIGH_LIGHT", f"Light intensity {light_intensity} lux excessively high")
            
            # Print status
            status = f"⏰ Hour {cycle_position:5.1f} | "
            status += f"💡 {light_intensity:6.1f} lux | "
            status += f"🌱 {'ON ' if grow_light_state else 'OFF'} | "
            status += f"📈 DLI {total_dli:5.2f}/20.0"
            print(status)
            
            time.sleep(10)  # 10 second intervals
    
    def send_telemetry(self, data):
        """Send telemetry data to ThingsBoard"""
        try:
            url = f"{self.tb_url}/api/v1/{self.token}/telemetry"
            response = requests.post(url, json=data, headers=self.headers, timeout=5)
            return response.status_code == 200
        except:
            return False
    
    def send_alert(self, alert_type, message):
        """Send alert telemetry"""
        alert_data = {
            "alert_type": alert_type,
            "alert_message": message,
            "alert_timestamp": int(time.time() * 1000)
        }
        self.send_telemetry(alert_data)
    def run_comprehensive_test(self):
        """Run all tests"""
        print("🚀 Starting Smart Farm IoT System Comprehensive Test")
        print("=" * 60)
        
        # Test connection first
        print("\n0. Testing Connectivity")
        print("-" * 30)
        if not self.test_connection():
            print("💥 Cannot proceed - no connection to ThingsBoard")
            print("\n📋 Troubleshooting steps:")
            print("1. Check if ThingsBoard server is running")
            print("2. Verify device token is correct")
            print("3. Check firewall/network settings")
            print("4. Try different server URLs (HTTP/HTTPS, with/without port)")
            return
        
        print("\n1. Testing RPC Methods")
        print("-" * 30)
        self.test_rpc_methods()
        
        print("\n2. Testing Shared Attributes")
        print("-" * 30)
        self.test_shared_attributes()
        
        print("\n3. Testing Telemetry")
        print("-" * 30)
        test_data = self.simulate_sensor_data()
        if self.send_telemetry(test_data):
            print("✅ Telemetry: SUCCESS")
        else:
            print("❌ Telemetry: FAILED")
        
        print("\n4. Daily Cycle Simulation")
        print("-" * 30)
        self.simulate_daily_cycle(duration_minutes=2)  # 2-minute compressed day
        
        print("\n✨ Test Complete!")

if __name__ == "__main__":
    # Configuration - Updated with working HTTPS URL from connection test
    THINGSBOARD_URL = "https://app.coreiot.io"  # HTTPS confirmed working
    DEVICE_TOKEN = "lr3yyoh7v2b98pdmuj38"      # Update with your device token
    
    # Alternative configurations for different ThingsBoard setups:
    # For HTTP: "http://app.coreiot.io:8080" 
    # For local: "http://localhost:8080" 
    # For ThingsBoard Cloud: "https://thingsboard.cloud"
    
    print(f"🌐 Testing connection to: {THINGSBOARD_URL}")
    print(f"🔑 Using device token: {DEVICE_TOKEN}")
    
    # Create tester instance
    tester = SmartFarmTester(THINGSBOARD_URL, DEVICE_TOKEN)
    
    # Run tests
    tester.run_comprehensive_test()
