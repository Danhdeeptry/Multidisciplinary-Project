import requests
import json
import time
import random
from datetime import datetime

# Thay đổi thông tin theo cấu hình thực tế của bạn
THINGSBOARD_HOST = "app.coreiot.io"
ACCESS_TOKEN = "lr3yyoh7v2b98pdmuj38"  # token của thiết bị trên ThingsBoard

# API endpoint của ThingsBoard
TELEMETRY_ENDPOINT = f"http://{THINGSBOARD_HOST}/api/v1/{ACCESS_TOKEN}/telemetry"
ATTRIBUTES_ENDPOINT = f"http://{THINGSBOARD_HOST}/api/v1/{ACCESS_TOKEN}/attributes"

class SimulatedDevice:
    def __init__(self):
        self.temperature = 28.0  # Nhiệt độ ban đầu (28°C)
        self.humidity = 65.0  # Độ ẩm ban đầu (65%)
        self.led_state = False
        
    def update_sensor_values(self):
        # Thêm dao động ngẫu nhiên như trong code ESP32
        self.temperature += random.randint(-100, 100) / 100.0  # Dao động ±1°C
        self.humidity += random.randint(-50, 50) / 100.0  # Dao động ±0.5%
        
        # Giới hạn giá trị
        self.temperature = max(15.0, min(self.temperature, 40.0))  # 15-40°C
        self.humidity = max(30.0, min(self.humidity, 90.0))  # 30-90%
        
        print(f"Updated simulated values: Temperature: {self.temperature:.2f}°C, Humidity: {self.humidity:.2f}%")
    
    def send_telemetry(self):
        # Cập nhật giá trị cảm biến
        self.update_sensor_values()
        
        # Tạo payload telemetry 
        telemetry = {
            "temperature": self.temperature,
            "humidity": self.humidity,
            "timestamp": int(time.time() * 1000)  # Thời gian hiện tại tính bằng ms
        }
        
        try:
            # Gửi dữ liệu telemetry đến ThingsBoard
            response = requests.post(TELEMETRY_ENDPOINT, json=telemetry)
            if response.status_code == 200:
                print(f"Telemetry sent successfully at {datetime.now().strftime('%H:%M:%S')}")
            else:
                print(f"Failed to send telemetry, status code: {response.status_code}")
                print(f"Response: {response.text}")
        except Exception as e:
            print(f"Error sending telemetry: {e}")
    
    def send_attributes(self):
        # Tạo payload attributes
        attributes = {
            "rssi": random.randint(-90, -30),  # Mô phỏng RSSI từ -90 đến -30 dBm
            "channel": random.randint(1, 13),
            "bssid": "AA:BB:CC:DD:EE:FF",
            "localIp": "192.168.1." + str(random.randint(2, 254)),
            "ssid": "P4.1111",
            "ledState": self.led_state
        }
        
        try:
            # Gửi dữ liệu attributes đến ThingsBoard
            response = requests.post(ATTRIBUTES_ENDPOINT, json=attributes)
            if response.status_code == 200:
                print(f"Attributes sent successfully at {datetime.now().strftime('%H:%M:%S')}")
            else:
                print(f"Failed to send attributes, status code: {response.status_code}")
                print(f"Response: {response.text}")
        except Exception as e:
            print(f"Error sending attributes: {e}")
    
    def run_simulation(self, interval_seconds=2, total_time_minutes=10):
        print(f"Starting device simulation. Will run for {total_time_minutes} minutes...")
        start_time = time.time()
        end_time = start_time + (total_time_minutes * 60)
        
        while time.time() < end_time:
            self.send_telemetry()
            self.send_attributes()
            time.sleep(interval_seconds)
        
        print("Simulation completed.")

if __name__ == "__main__":
    device = SimulatedDevice()
    
    # Thực hiện mô phỏng gửi dữ liệu trong 10 phút, cứ mỗi 2 giây
    device.run_simulation(interval_seconds=2, total_time_minutes=10)
