"""
Simple ThingsBoard Connection Test
Quick test to verify ThingsBoard connectivity and device token
"""

import requests
import json
import time

def test_thingsboard_connection():
    """Test various ThingsBoard connection configurations"""
    
    # Configuration
    DEVICE_TOKEN = "lr3yyoh7v2b98pdmuj38"
    
    # Different possible ThingsBoard URLs to test
    test_configs = [
        {"name": "HTTP with port 8080", "url": "http://app.coreiot.io:8080"},
        {"name": "HTTPS", "url": "https://app.coreiot.io"},
        {"name": "HTTP default", "url": "http://app.coreiot.io"},
        {"name": "HTTP with port 80", "url": "http://app.coreiot.io:80"},
        {"name": "Local development", "url": "http://localhost:8080"},
    ]
    
    headers = {'Content-Type': 'application/json'}
    test_data = {
        "temperature": 25.5,
        "humidity": 60.0,
        "light_intensity": 350.2,
        "test_timestamp": int(time.time() * 1000)
    }
    
    print("🧪 ThingsBoard Connection Test")
    print("=" * 50)
    print(f"🔑 Device Token: {DEVICE_TOKEN}")
    print(f"📊 Test Data: {test_data}")
    print()
    
    for config in test_configs:
        print(f"Testing: {config['name']}")
        print(f"URL: {config['url']}")
        
        try:
            # Test telemetry endpoint
            telemetry_url = f"{config['url']}/api/v1/{DEVICE_TOKEN}/telemetry"
            print(f"   📡 Telemetry: {telemetry_url}")
            
            response = requests.post(
                telemetry_url, 
                json=test_data, 
                headers=headers, 
                timeout=10
            )
            
            print(f"   Status: {response.status_code}")
            print(f"   Response: {response.text[:100]}...")
            
            if response.status_code == 200:
                print("   ✅ SUCCESS - Telemetry sent successfully!")
                
                # Test attributes endpoint
                attrs_url = f"{config['url']}/api/v1/{DEVICE_TOKEN}/attributes"
                attr_data = {"test_connection": True, "timestamp": int(time.time())}
                
                attr_response = requests.post(
                    attrs_url, 
                    json=attr_data, 
                    headers=headers, 
                    timeout=10
                )
                
                if attr_response.status_code == 200:
                    print("   ✅ Attributes also working!")
                
                # Test RPC endpoint
                rpc_url = f"{config['url']}/api/v1/{DEVICE_TOKEN}/rpc"
                rpc_data = {"method": "ping", "params": {}}
                
                try:
                    rpc_response = requests.post(
                        rpc_url, 
                        json=rpc_data, 
                        headers=headers, 
                        timeout=5
                    )
                    print(f"   🔧 RPC Status: {rpc_response.status_code}")
                except:
                    print("   ⚠️  RPC timeout (normal for test)")
                
                print(f"\n🎉 WORKING CONFIGURATION FOUND!")
                print(f"Use this URL in your applications: {config['url']}")
                return config['url']
                
            elif response.status_code == 401:
                print("   ❌ UNAUTHORIZED - Check device token")
            elif response.status_code == 404:
                print("   ❌ NOT FOUND - Device doesn't exist or wrong URL")
            else:
                print(f"   ⚠️  Unexpected status: {response.status_code}")
                
        except requests.exceptions.ConnectTimeout:
            print("   ❌ CONNECTION TIMEOUT")
        except requests.exceptions.ConnectionError as e:
            print(f"   ❌ CONNECTION ERROR: {str(e)[:100]}...")
        except Exception as e:
            print(f"   ❌ ERROR: {str(e)[:100]}...")
        
        print("-" * 50)
    
    print("❌ No working configuration found!")
    print("\n🔧 Troubleshooting suggestions:")
    print("1. Check if ThingsBoard server is accessible")
    print("2. Verify device token in ThingsBoard dashboard")
    print("3. Check firewall/proxy settings")
    print("4. Try from different network")
    print("5. Contact ThingsBoard administrator")
    
    return None

def test_esp32_connectivity():
    """Test if ESP32 device is online and sending data"""
    print("\n📱 ESP32 Device Status Check")
    print("-" * 30)
    
    # This would require ThingsBoard API credentials to check device status
    # For now, just provide instructions
    print("To check if your ESP32 is connected:")
    print("1. Open ThingsBoard dashboard")
    print("2. Go to Devices > ESP32 Smart Farm")
    print("3. Check 'Last Activity' timestamp")
    print("4. Look for recent telemetry data")
    print("5. Check device attributes for connection status")

if __name__ == "__main__":
    print("🌐 Smart Farm IoT - Connection Diagnostic Tool")
    print("This tool helps identify the correct ThingsBoard configuration")
    print()
    
    # Test ThingsBoard connection
    working_url = test_thingsboard_connection()
    
    if working_url:
        print(f"\n✅ Success! Update your test script with:")
        print(f'THINGSBOARD_URL = "{working_url}"')
    
    # Check ESP32 status
    test_esp32_connectivity()
    
    print("\n📚 Next steps:")
    print("1. If connection works, update test_smart_farm.py")
    print("2. If no connection, check ThingsBoard server status")
    print("3. Verify ESP32 is powered on and connected to WiFi")
    print("4. Check serial monitor for ESP32 connection logs")
