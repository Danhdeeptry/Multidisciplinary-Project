"""
Quick ThingsBoard API Test
Test specific endpoints with different configurations
"""

import requests
import json

def quick_test():
    """Quick test of ThingsBoard connectivity using confirmed working URL"""
    
    # Confirmed working configuration
    url = "https://app.coreiot.io"
    token = "lr3yyoh7v2b98pdmuj38"
    headers = {'Content-Type': 'application/json'}
      # Smart Farm telemetry data for testing
    import time
    test_data = {
        "temperature": 23.5,
        "humidity": 65.2,
        "light_intensity": 425.7,
        "grow_light_active": True,
        "daily_light_integral": 12.34,
        "dli_progress_percent": 61.7,
        "auto_mode_active": True,
        "alert_type": "NORMAL",
        "test_timestamp": int(time.time() * 1000)
    }
    
    print("🌱 Smart Farm Quick Test")
    print("=" * 40)
    print(f"🔗 URL: {url}")
    print(f"🔑 Token: {token}")
    print(f"📊 Test Data: {json.dumps(test_data, indent=2)}")
    
    # Test telemetry
    print(f"\n📡 Testing Telemetry...")
    endpoint = f"{url}/api/v1/{token}/telemetry"
    
    try:
        response = requests.post(endpoint, json=test_data, headers=headers, timeout=10)
        print(f"Status: {response.status_code}")
        
        if response.status_code == 200:
            print("✅ TELEMETRY SUCCESS!")
            
            # Test attributes
            print(f"\n📋 Testing Attributes...")
            attr_data = {
                "light_threshold_low": 200,
                "light_threshold_high": 500,
                "dli_target": 18.0,
                "auto_light_mode": True,
                "light_alerts_enabled": True
            }
            
            attr_endpoint = f"{url}/api/v1/{token}/attributes"
            attr_response = requests.post(attr_endpoint, json=attr_data, headers=headers, timeout=5)
            
            if attr_response.status_code == 200:
                print("✅ ATTRIBUTES SUCCESS!")
                print("\n🎉 ALL TESTS PASSED!")
                return True
            else:
                print(f"❌ Attributes failed: {attr_response.status_code}")
                return False
                
        else:
            print(f"❌ Telemetry failed: {response.status_code}")
            print(f"Response: {response.text[:100]}")
            return False
            
    except Exception as e:
        print(f"❌ Connection Error: {str(e)}")
        return False

if __name__ == "__main__":
    result = quick_test()
    if result:
        print(f"\nWorking URL: {result}")
    else:
        print("\nNo working configuration found")
